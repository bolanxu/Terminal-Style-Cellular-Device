#ifndef SETTINGS_H
#define SETTINGS_H

/*  Network Settings  */

#ifndef STASSID
#define STASSID "benji"
#define STAPSK  "benji2021"
#endif

#define QUOTE_HOST       "djxmmx.net"
#define SERVER_HOSTNAME  "bolanxu.pythonanywhere.com"

/*  TFT Settings  */

#define SCREEN_W  160
#define SCREEN_H  128
#define CHAR_W    6
#define CHAR_H    8
#define MAX_LINES (SCREEN_H / CHAR_H)
#define MAX_COLS  (SCREEN_W / CHAR_W)

/*  Terminal Settings  */
#define PROMPT "> "

#endif