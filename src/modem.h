#ifndef MODEM_H
#define MODEM_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "settings.h"

class Modem
{
private:
    SoftwareSerial _serial;
    int _pwrkey;
    String _serverHostname = SERVER_HOSTNAME;

    bool waitResponse(const char *expected, unsigned long timeout = 3000);
    String readUntilStr(const char *expected1, const char *expected2, unsigned long timeout);
    String waitAndReadResponse(unsigned long timeout, unsigned int interval = 0);

    String readShreadData(int expectedLen, unsigned long timeout);

    bool httpConnect();
    bool checkHttpConn();

public:
    Modem(int rx, int tx, int pwrkey);

    bool begin(const char *apn = "hologram");
    
    void powerOn();

    void sendAT(const char *cmd);

    String readResponse(unsigned long timeout = 3000);

    bool sendHTTPGet(const char *path, String &response);
    bool sendHTTPPost(const char *path, const char *body, String &response);
};

#endif
