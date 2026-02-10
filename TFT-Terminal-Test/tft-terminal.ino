#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// -------- TFT PINS --------
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

#define ST7735_VSCRSADD 0x37

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// -------- SCREEN --------
#define SCREEN_W 160
#define SCREEN_H 128
#define CHAR_W    6
#define CHAR_H    8
#define MAX_LINES (SCREEN_H / CHAR_H)

// -------- TERMINAL BUFFER --------
String lines[MAX_LINES];
int lineCount = 0;

// -------- INPUT --------
String inputLine = "";
const char *PROMPT = "> ";

// -------- DRAW TERMINAL --------
void redrawTerminal() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  for (int i = 0; i < lineCount; i++) {
    tft.setCursor(0, i * CHAR_H);
    tft.print(lines[i]);
  }
}

// -------- ADD LINE --------
void addLine(String text) {
  if (lineCount < MAX_LINES) {
    lines[lineCount++] = text;
  } else {
    for (int i = 1; i < MAX_LINES; i++) {
      lines[i - 1] = lines[i];
    }
    lines[MAX_LINES - 1] = text;
  }
  redrawTerminal();
}

// -------- COMMAND HANDLER --------
void handleCommand(String cmd) {
  cmd.trim();

  if (cmd == "help") {
    addLine("Commands:");
    addLine(" help  clear  echo");
  }
  else if (cmd == "clear") {
    lineCount = 0;
    redrawTerminal();
    addLine(PROMPT);
    return;
  }
  else if (cmd.startsWith("echo ")) {
    addLine(cmd.substring(5));
  }
  else if (cmd.length() > 0) {
    addLine("Unknown command");
  }

  addLine(PROMPT);
}

// -------- SETUP --------
void setup() {
  Serial.begin(9600);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.setTextSize(1);
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);

  addLine("Terminal Phone Booting...");
  addLine("Done...");
  addLine("---------------------");
  addLine(PROMPT);
}

// -------- LOOP --------
void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    // ENTER
    if (c == '\n' || c == '\r') {

      lines[lineCount - 1] = String(PROMPT) + inputLine;
      redrawTerminal();

      handleCommand(inputLine);

      inputLine = "";
      return;
    }

    // BACKSPACE
    if (c == 8 || c == 127) {
      if (inputLine.length() > 0) {
        inputLine.remove(inputLine.length() - 1);
        redrawTerminal();
        tft.setCursor(strlen(PROMPT) * CHAR_W, (lineCount - 1) * CHAR_H);
        tft.print(PROMPT);
        tft.print(inputLine);
      }
      return;
    }

    // NORMAL CHAR
    if (c >= 32 && c <= 126) {
      inputLine += c;
      redrawTerminal();
      tft.setCursor(strlen(PROMPT) * CHAR_W, (lineCount - 1) * CHAR_H);
      tft.print(PROMPT);
      tft.print(inputLine);
    }
  }
}
