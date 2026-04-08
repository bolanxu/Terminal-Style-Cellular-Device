#ifndef SMS_H
#define SMS_H

#include <Arduino.h>
#include "settings.h"
#include "communication.h"

struct Contact
{
  String name;
  String phoneNum;
};

class SMS
{
  private:
    const Contact _contactTable[2] =
    {
      {"testuser", "1111111111"},
      {"dad",      "2222222222"},
    };
    int _numOfContacts = sizeof(_contactTable) / sizeof(Contact);

  public:
    SMS() {}

    String getContactNum(const String& name);

    int sendSMS(const String& contact, const String& msg);
    String readSMS();

    int addMessageOut(const String& phoneNum, String msg);
    int addMessageIn(const String& phoneNum, const String& contact, String msg);
};

#endif