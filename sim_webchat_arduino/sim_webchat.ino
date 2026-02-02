#include <SoftwareSerial.h>

// ================== CONFIG ==================
SoftwareSerial modem(7, 8);   // RX, TX

const String SERVER = "username.pythonanywhere.com"; // CHANGE: Change 'username' to your pythonanywhere username
const String APN    = "hologram";                         // CHANGE: Change this to your LTE-M SIM carrier
// ============================================

bool sendCheck(String cmd, String expected, int timeout = 2000);
bool waitFor(String expected, int timeout);
String parsePayload(String raw);

void setup() {
  Serial.begin(9600);
  modem.begin(9600);

  Serial.println("n--- SIM7080G HTTP Chat Init ---");

  pinMode(4, OUTPUT);

  digitalWrite(4, LOW);
  delay(100);
  digitalWrite(4, HIGH);
  delay(2000);
  digitalWrite(4, LOW);
  delay(4000);

  if (!sendCheck("AT", "OK", 3000)) {
    Serial.println("Modem not responding");
    while (1);
  }
  else
  {
    Serial.println("Modem initialized");
  }

  sendCheck("ATE0", "OK");            // Echo off
  sendCheck("AT+CFUN=1", "OK");

  // PDP context
  sendCheck("AT+CGDCONT=1,\"IP\",\"" + APN + "\"", "OK");

  delay(1000);

  //sendCheck("AT+CNACT=0,0", "OK", 5000);

  Serial.println("Activating network...");
  while (!sendCheck("AT+CNACT=0,1", "OK", 5000)) {
    delay(2000);
  }
  delay(2000);

  // ---------- HTTP CONFIG (NO TLS) ----------
  //sendCheck("AT+SHCONF=\"CID\",1", "OK");
  sendCheck("AT+SHCONF=\"URL\",\"" + SERVER + "\"", "OK");
  sendCheck("AT+SHCONF=\"BODYLEN\",1024", "OK");
  sendCheck("AT+SHCONF=\"HEADERLEN\",350", "OK");

  Serial.println("Connecting HTTP...");
  if (sendCheck("AT+SHCONN", "OK", 10000)) {
    Serial.println("HTTP connected.");
  } else {
    Serial.println("HTTP connect failed.");
  }

  Serial.println("Ready.");
}

void loop() {
  // Send message from Serial Monitor
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length()) postToWeb(msg);
  }

  // Poll server every 10 seconds
  static unsigned long lastPoll = 0;
  if (millis() - lastPoll > 5000) {
    pollFromWeb();
    lastPoll = millis();
  }
}

// ================= POST =================
void postToWeb(String text) {
  text.trim();
  Serial.println("Sending: " + text);

  sendCheck("AT+SHCHEAD", "OK");
  sendCheck("AT+SHAHEAD=\"Content-Type\",\"text/plain\"", "OK");

  modem.print("AT+SHBOD=" + String(text.length()) + ",5000\r\n");
  if (!waitFor(">", 3000)) {
    Serial.println("ERROR: No > prompt");
    return;
  }

  modem.print(text+"\r\n");
  waitFor("OK", 5000);

  if (sendCheck("AT+SHREQ=\"/post_from_arduino\",3",
                "+SHREQ: \"POST\",200", 8000)) {
    Serial.println("POST OK");
  } else {
    Serial.println("POST FAILED");
  }
}

// ================= GET =================
void pollFromWeb() {
  sendCheck("AT+SHCHEAD", "OK");

  modem.println("AT+SHREQ=\"/get_for_arduino\",1\r");

  String shreq;
  unsigned long start = millis();
  while (millis() - start < 1000) {
    while (modem.available()) {
      shreq += (char)modem.read();
    }
  }
  
  int lastComma = shreq.lastIndexOf(',');
  if (lastComma == -1) return;

  int len = shreq.substring(lastComma+1).toInt();
  if (len <= 0) return;

  modem.println("AT+SHREAD=0,"+String(len)+"\r");

  //Serial.println("LENGTH: "+String(len));

  String response;
  start = millis();
  while (millis() - start < 1000) {
    while (modem.available()) {
      char c = (char)modem.read();
      response += c;
      //Serial.print(c);
    }
  }

  //Serial.println("RECIEVED: "+response);

  if (response.length() && response.indexOf("NO_MSG") == -1) {
    Serial.print("\n[WEB]: ");
    String msg = response.substring(response.length()-len-2, response.length()-2);
    Serial.println(msg);
  }
}

// =============== HELPERS =================
bool sendCheck(String cmd, String expected, int timeout = 2000) {
  //Serial.println("Sent: "+cmd);
  modem.println(cmd+"\r");
  return (waitFor(expected, timeout));
}

bool waitFor(String expected, int timeout) {
  unsigned long start = millis();
  String buffer;

  while (millis() - start < timeout) {
    while (modem.available()) {
      char c = modem.read();
      buffer += c;
      if (buffer.indexOf(expected) != -1)
      {
        //Serial.println("Success!");
        return true;
      }
    }
  }
  //Serial.println("Error: "+buffer);
  return false;
}

String parsePayload(String raw) {
  int idx = raw.indexOf("\r\n");
  if (idx == -1) return "";
  String s = raw.substring(idx);
  s.trim();
  return s;
}
