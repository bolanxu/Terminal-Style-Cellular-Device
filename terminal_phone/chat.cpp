#include "chat.h"
#include "terminal.h"
#include "globals.h"

Chat::Chat(const String& contact, Terminal* info, Terminal* messages, Terminal* sendBar)
{
  _info = info;
  _messages = messages;
  _sendBar = sendBar;

  _currentContact = contact;

  _info->clear();
  _messages->clear();
  _sendBar->clear();

  _info->println(_currentContact, TFT_GREEN);
  _sendBar->print(PROMPT, TFT_YELLOW);

  displayHistory(_currentContact);
}

Chat::~Chat()
{
}

void Chat::displayHistory(const String& contact)
{
  String phoneNum = sms.getContactNum(contact);
  if (phoneNum == "not found") return;

  if (data.openFile(phoneNum + ".txt", "r") == -1) return;

  while (1)
  {
    String currLine = data.readNextLine();
    if (currLine.length() == 0) break;

    int sep = currLine.indexOf(':');
    if (sep == -1)
    {
      _messages->println(currLine);
      continue;
    }

    _messages->print(currLine.substring(0, sep + 1), TFT_YELLOW);
    _messages->print(" ");
    _messages->println(currLine.substring(sep + 1));
  }

  data.closeFile();
}

void Chat::updateKey(char c)
{
  if (c == '\n' || c == '\r')
  {
    if (_inputText.length() > 0)
      sms.sendSMS(_currentContact, _inputText);

    _inputText = "";
    _sendBar->clear();
    _sendBar->print(PROMPT, TFT_YELLOW);
  }
  else if (c == '\b' || c == 127)
  {
    if (_inputText.length() > 0)
    {
      _inputText.remove(_inputText.length() - 1);
      _sendBar->print('\b');
    }
  }
  else
  {
    _inputText += c;
    _sendBar->print(c);
  }
}

void Chat::updateMsgs()
{
  String msg = sms.readSMS();

  if (msg != "ERROR" && msg != "NONE")
  {
    for (;;)
    {
      String contact_name = msg.substring(1, msg.indexOf(']'));
      String currentMsg = msg.substring(msg.indexOf(' ') + 1, msg.indexOf('[', 1) - 1);

      _messages->print(contact_name + ":", TFT_YELLOW);
      _messages->print(" ");
      _messages->println(currentMsg);

      if (msg.indexOf('\n') == -1) break;
      msg = msg.substring(msg.indexOf('\n') + 1);
    }
  }
}