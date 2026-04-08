#ifndef GLOBALS_H
#define GLOBALS_H

#include <TFT_eSPI.h>

class Terminal;
class Communication;
class SMS;
class Memory;
class Chat;

extern TFT_eSPI tft;

extern Terminal terminal;
extern Terminal termInfo;
extern Terminal termMessages;
extern Terminal termSendBar;

extern Communication comm;
extern SMS sms;
extern Memory data;

extern Chat* currentChat;

extern int mainState;
/*
 *  0: Off, sleep mode
 *  1: Terminal mode
 *  2: SMS Chat
 */

#endif