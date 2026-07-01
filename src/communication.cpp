#include "communication.h"

Communication::Communication()
{
}

int Communication::postToWeb(const String &path, const String &message)
{
    String url = "http://" + _serverHostname + path;

    if (WiFi.status() == WL_CONNECTED)
    {
        _http.begin(_client, url);
        _http.addHeader("Content-Type", "text/plain");
        int httpCode = _http.POST(message);
        WiFiClient *stream = _http.getStreamPtr();
        if (stream)
            while (stream->available())
                stream->read();
        _http.end();
        return (httpCode > 0) ? 1 : -1;
    }

    if (_modem != nullptr)
    {
        String response;
        return _modem->sendHTTPPost(path.c_str(), message.c_str(), response) ? 1 : -1;
    }

    return -1;
}

String Communication::pollFromWeb(const String &path)
{
    String url = "http://" + _serverHostname + path;

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(_client, url);
        int httpCode = http.GET();
        if (httpCode <= 0)
        {
            http.end();
            return "ERROR";
        }
        String msg = http.getString();
        http.end();
        return msg;
    }

    if (_modem != nullptr)
    {
        String response;
        return _modem->sendHTTPGet(path.c_str(), response) ? response : "ERROR";
    }

    return "ERROR";
}
