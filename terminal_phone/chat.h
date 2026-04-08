#ifndef CHAT_H
#define CHAT_H

#include <Arduino.h>
#include "settings.h"

class Terminal;

class Chat
{
  private:
    Terminal* _info;
    Terminal* _messages;
    Terminal* _sendBar;

    String _inputText;
    String _currentContact;

  public:
    Chat(const String& contact, Terminal* info, Terminal* messages, Terminal* sendBar);
    ~Chat();

    void displayHistory(const String& contact);
    void updateKey(char c);
    void updateMsgs();
};

#endif