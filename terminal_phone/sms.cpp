#include "sms.h"
#include "globals.h"

String SMS::getContactNum(const String& name)
{
  for (int i = 0; i < _numOfContacts; i++)
  {
    if (name == _contactTable[i].name)
      return _contactTable[i].phoneNum;
  }
  return "not found";
}

int SMS::sendSMS(const String& contact, const String& msg)
{
  String phoneNum = getContactNum(contact);

  if (phoneNum == "not found")
    return -2;

  addMessageOut(phoneNum, msg);
  return comm.postToWeb("/" + phoneNum + "/post_from_arduino", msg);
}

String SMS::readSMS()
{
  String msg = comm.pollFromWeb();

  if (msg != "ERROR" && msg != "NONE")
  {
    for (;;)
    {
      String contact_name = msg.substring(1, msg.indexOf(']'));
      String currentMsg = msg.substring(msg.indexOf(' ') + 1, msg.indexOf('[', 1) - 1);

      addMessageIn(getContactNum(contact_name), contact_name, currentMsg);

      if (msg.indexOf('\n') == -1) break;
      msg = msg.substring(msg.indexOf('\n') + 1);
    }
  }

  return msg;
}

int SMS::addMessageOut(const String& phoneNum, String msg)
{
  msg.trim();
  return data.addToFile(phoneNum + ".txt", "ME:" + msg + "\n");
}

int SMS::addMessageIn(const String& phoneNum, const String& contact, String msg)
{
  msg.trim();
  return data.addToFile(phoneNum + ".txt", contact + ":" + msg + "\n");
}