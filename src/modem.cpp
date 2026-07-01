#include "modem.h"

Modem::Modem(int rx, int tx, int pwrkey)
    : _serial(rx, tx), _pwrkey(pwrkey) {}

bool Modem::begin(const char *apn)
{
    pinMode(_pwrkey, OUTPUT);
    digitalWrite(_pwrkey, LOW);
    _serial.begin(9600);

    // check if already on
    sendAT("AT");
    if (!waitResponse("OK", 2000))
    {
        powerOn();

        delay(5000);
        while (_serial.available())
            _serial.read();

        sendAT("AT");
        if (!waitResponse("OK", 3000))
        {
            Serial.println("[SIM] Module not responding");
            return false;
        }
    }
    Serial.println("[SIM] Started");

    sendAT("ATE0");
    if (!waitResponse("OK"))
    {
        Serial.println("[SIM] ATE0 failed");
        return false;
    }
    Serial.println("[SIM] Echo Disabled");

    sendAT("AT+CPIN?");
    if (!waitResponse("READY", 3000))
    {
        Serial.println("[SIM] No SIM or SIM not ready");
        return false;
    }
    Serial.println("[SIM] SIM card found");

    sendAT("AT+CFUN=1");
    if (!waitResponse("OK"))
    {
        Serial.println("[SIM] Set Func failed");
        return false;
    }

    unsigned long start = millis();
    bool registered = false;
    while (millis() - start < 15000)
    {
        sendAT("AT+CREG?");
        String resp = readResponse(2000); // now returns as soon as OK/ERROR line is seen
        if (resp.indexOf(",1") != -1 || resp.indexOf(",5") != -1)
        {
            registered = true;
            break;
        }
        delay(2000);
    }
    if (!registered)
    {
        Serial.println("[SIM] Network registration failed");
        return false;
    }
    Serial.println("[SIM] Network Registered");

    String apnCmd = String("AT+CGDCONT=1,\"IP\",\"") + apn + "\"";
    sendAT(apnCmd.c_str());
    if (!waitResponse("OK"))
    {
        Serial.println("[SIM] APN setup failed");
        return false;
    }
    Serial.println("[SIM] APN Set");

    delay(2000);

    Serial.println("[SIM] Activating network...");
    start = millis();
    bool activated = false;

    sendAT("AT+CNACT?");
    String status = readResponse(2000);
    if (status.indexOf("+CNACT: 0,1") != -1)
    {
        activated = true;
    }

    else
    {
        sendAT("AT+CNACT=0,1");
        String resp = readResponse(8000); // ceiling only - returns early once +APP PDP: line arrives

        if (resp.indexOf("+APP PDP: 0,ACTIVE") != -1)
        {
            activated = true;
        }
    }

    if (!activated)
    {
        Serial.println("[SIM] Wireless connection failed");
        return false;
    }
    Serial.println("[SIM] Wireless Connection Enabled");

    sendAT("AT+SHDISC");
    waitResponse("OK", 3000);

    String urlCmd = String("AT+SHCONF=\"URL\",\"http://") + _serverHostname + "\"";
    sendAT(urlCmd.c_str());
    if (!waitResponse("OK"))
    {
        Serial.println("[SIM] SHCONF URL failed");
        return false;
    }

    sendAT("AT+SHCONF=\"BODYLEN\",1024");
    if (!waitResponse("OK"))
    {
        Serial.println("[SIM] SHCONF BODYLEN failed");
        return false;
    }

    sendAT("AT+SHCONF=\"HEADERLEN\",350");
    if (!waitResponse("OK"))
    {
        Serial.println("[SIM] SHCONF HEADERLEN failed");
        return false;
    }

    if (!httpConnect())
        return false;

    Serial.println("[SIM] Ready");
    return true;
}

bool Modem::checkHttpConn()
{
    sendAT("AT+SHSTATE?");
    String response = readUntilStr("OK", "OK", 2000);
    if (response.indexOf("1") >= 0)
        return true;

    return false;
}

bool Modem::httpConnect()
{
    if (!checkHttpConn())
    {
        sendAT("AT+SHCONN");
        if (!waitResponse("OK", 10000))
        {
            Serial.println("[SIM] HTTP connect failed");
            return false;
        }
        Serial.println("[SIM] HTTP Connected");
        return true;
    }
    return true;
}

void Modem::powerOn()
{
    digitalWrite(_pwrkey, LOW);
    delay(1000);
    digitalWrite(_pwrkey, HIGH);
    delay(2000);
    digitalWrite(_pwrkey, LOW);
    delay(3000);
}

void Modem::sendAT(const char *cmd)
{
    _serial.print(cmd);
    _serial.print("\r");
    Serial.print("[TO SIM]: ");
    Serial.println(cmd);
}

bool Modem::waitResponse(const char *expected, unsigned long timeout)
{
    unsigned long start = millis();
    String buf = "";

    Serial.print("[FROM SIM]: ");

    while (millis() - start < timeout)
    {
        while (_serial.available())
        {
            buf += (char)_serial.read();
            if (buf.indexOf(expected) != -1)
            {
                Serial.println(buf);
                return true;
            }
        }
    }
    Serial.println(buf);
    return false;
}

String Modem::readUntilStr(const char *expected1, const char *expected2, unsigned long timeout)
{
    unsigned long start = millis();
    String buf = "";

    Serial.print("[FROM SIM]: ");

    while (millis() - start < timeout)
    {
        while (_serial.available())
        {
            buf += (char)_serial.read();
            delay(10);
        }
        if (buf.indexOf(expected1) != -1 || buf.indexOf(expected2) != -1)
        {
            break;
        }
    }
    Serial.println(buf);
    return buf;
}

String Modem::readResponse(unsigned long timeout)
{
    // Old version always waited the FULL timeout no matter what -
    // this version returns the instant a real terminal line shows up,
    // and falls back to the timeout only if nothing ever terminates.
    unsigned long start = millis();
    String buf = "";
    String lineBuf = "";

    while (millis() - start < timeout)
    {
        while (_serial.available())
        {
            char c = (char)_serial.read();
            buf += c;
            lineBuf += c;

            if (c == '\n')
            {
                String trimmed = lineBuf;
                trimmed.trim();

                // exact-line check, not substring search - avoids
                // matching "OK" that happens to appear inside data
                if (trimmed == "OK" || trimmed == "ERROR" ||
                    trimmed.startsWith("+CME ERROR") ||
                    trimmed.startsWith("+CMS ERROR") ||
                    trimmed.startsWith("+APP PDP:")) // async PDP notification, e.g. CNACT=0,1
                {
                    Serial.print("[FROM SIM]: ");
                    Serial.println(buf);
                    return buf;
                }
                lineBuf = "";
            }
        }
    }
    Serial.print("[FROM SIM]: ");
    Serial.println(buf);
    return buf;
}

String Modem::waitAndReadResponse(unsigned long timeout, unsigned int interval)
{
    unsigned long start = millis();
    String buf = "";

    Serial.print("[FROM SIM]: ");

    while (!_serial.available())
    {
        if (millis() - start > timeout)
        {
            return "TIMEOUT";
        }
    }

    while (_serial.available())
    {
        buf += (char)_serial.read();
    }

    if (interval != 0)
        delay(interval);

    while (_serial.available())
    {
        buf += (char)_serial.read();
    }

    Serial.println(buf);
    return (buf);
}

String Modem::readShreadData(int expectedLen, unsigned long timeout)
{
    // Don't search for "OK" at all - SIM7080G sometimes sends an
    // early OK right after the read command is accepted, BEFORE the
    // actual +SHREAD data block. Searching for "OK" as a substring
    // matches that early one and returns before the real payload
    // arrives. Instead: find the "+SHREAD:" header line, then read
    // exactly expectedLen bytes after it and stop - no string search
    // needed, no waiting for a trailing OK either.
    unsigned long start = millis();
    String buf = "";
    bool headerFound = false;
    int dataStart = -1;

    while (millis() - start < timeout)
    {
        while (_serial.available())
        {
            buf += (char)_serial.read();

            if (!headerFound)
            {
                int headerPos = buf.indexOf("+SHREAD:");
                if (headerPos != -1)
                {
                    int lineEnd = buf.indexOf('\n', headerPos);
                    if (lineEnd != -1)
                    {
                        headerFound = true;
                        dataStart = lineEnd + 1;
                    }
                }
            }
            else
            {
                int dataBytesRead = buf.length() - dataStart;
                if (dataBytesRead >= expectedLen)
                {
                    // got exactly what we expect, return immediately -
                    // don't wait around for a trailing OK to show up
                    Serial.print("[FROM SIM]: ");
                    Serial.println(buf);
                    return buf.substring(dataStart, dataStart + expectedLen);
                }
            }
        }
    }

    Serial.print("[FROM SIM]: ");
    Serial.println(buf);
    // timeout fallback - return whatever data we did get, if any
    if (headerFound && dataStart != -1)
        return buf.substring(dataStart);
    return "";
}

bool Modem::sendHTTPGet(const char *path, String &response)
{
    if (!httpConnect())
        return false;

    String cmd = String("AT+SHREQ=\"") + path + "\",1";
    sendAT(cmd.c_str());

    String shreq = readUntilStr("+SHREQ: \"GET\",200,", "+SHSTATE: 0", 6000);

    if (shreq.indexOf("+SHSTATE: 0") >= 0)
    {
        if (!httpConnect())
            return false;

        sendAT(cmd.c_str());

        shreq = readUntilStr("+SHREQ: \"GET\",200,", "+SHSTATE", 6000);
    }

    int lastComma = shreq.lastIndexOf(',');
    if (lastComma == -1)
        return false;

    int len = shreq.substring(lastComma + 1).toInt();
    if (len <= 0)
        return false;

    String readCmd = String("AT+SHREAD=0,") + len;
    sendAT(readCmd.c_str());

    response = readShreadData(len, 4000);
    return response.length() > 0;
}

bool Modem::sendHTTPPost(const char *path, const char *body, String &response)
{
    sendAT("AT+SHCHEAD");
    waitResponse("OK");

    sendAT("AT+SHAHEAD=\"Content-Type\",\"text/plain\"");
    waitResponse("OK");

    String bodyCmd = String("AT+SHBOD=") + strlen(body) + ",5000";
    sendAT(bodyCmd.c_str());
    if (!waitResponse(">", 3000))
        return false;

    _serial.print(body);
    _serial.print("\r\n");
    waitResponse("OK", 5000);

    String cmd = String("AT+SHREQ=\"") + path + "\",3";
    sendAT(cmd.c_str());

    String shreq = readUntilStr("+SHREQ: \"POST\",", "+SHSTATE", 8000);
    int lastComma = shreq.lastIndexOf(',');
    if (lastComma == -1)
        return false;

    int len = shreq.substring(lastComma + 1).toInt();
    if (len <= 0)
        return true;

    String readCmd = String("AT+SHREAD=0,") + len;
    sendAT(readCmd.c_str());

    response = readShreadData(len, 4000);
    return response.length() > 0;
}
