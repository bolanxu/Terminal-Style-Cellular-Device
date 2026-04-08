#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include "settings.h"

class Memory
{
  private:
    File file;

  public:
    Memory() {}

    int openFile(const String& filename, const String& access);
    void closeFile();

    int addToFile(const String& filename, const String& data);
    String readNextLine();
};

#endif