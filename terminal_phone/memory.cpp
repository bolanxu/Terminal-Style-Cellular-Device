#include "memory.h"

int Memory::openFile(const String& filename, const String& access)
{
  String path = filename;
  if (!path.startsWith("/")) path = "/" + path;

  file = LittleFS.open(path, access.c_str());

  if (!file)
  {
    closeFile();
    return -1;
  }
  return 1;
}

void Memory::closeFile()
{
  if (file) file.close();
}

int Memory::addToFile(const String& filename, const String& data)
{
  if (openFile(filename, "a") == -1)
  {
    return -2;
  }

  if (!file.print(data))
  {
    closeFile();
    return -1;
  }

  closeFile();
  return 1;
}

String Memory::readNextLine()
{
  if (file && file.available())
  {
    String currLine = file.readStringUntil('\n');
    currLine.trim();
    return currLine;
  }

  return "";
}