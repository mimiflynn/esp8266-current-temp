/**The MIT License (MIT)

Copyright (c) 2019 by Mimi Flynn

https://mimiflynn.com

based on WundergroundClient which is:

Copyright (c) 2015 by Daniel Eichhorn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

See more at http://blog.squix.ch

Secure client example at: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/HTTPSRequest/HTTPSRequest.ino

https://circuits4you.com/2019/01/10/esp8266-nodemcu-https-secured-get-request/

*/
#define DEBUG_SSL
#define DEBUGV

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "WeatherClient.h"

const String API_URL = "api.weather.gov";
const int httpsPort = 443;
const char fingerprint[] PROGMEM = "29 F0 37 BA CE 86 3C 94 3C 3B DF 2C E5 30 3A 24 3E AE F4 91";

WeatherClient::WeatherClient()
{
}

void WeatherClient::updateConditions(String stationId)
{
  doUpdate("/stations/" + stationId + "/observations/latest");
}

void WeatherClient::doUpdate(String url)
{
  Serial.setDebugOutput(true);
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClientSecure client;

  Serial.print("Requesting URL: ");
  Serial.println(url);
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(API_URL, httpsPort))
  {
    Serial.println("connection failed");
    return;
  }

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host:" + API_URL + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" +
               "Connection: close\r\n\r\n");

  int retryCounter = 0;

  while (!client.available())
  {
    delay(1000);
    retryCounter++;
    if (retryCounter > 10)
    {
      Serial.println("Retry counter");
      Serial.println(retryCounter);
      return;
    }
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  client.setNoDelay(false);
  while (client.connected())
  {
    while ((size = client.available()) > 0)
    {
      c = client.read();
      if (c == '{' || c == '[')
      {
        isBody = true;
      }
      if (isBody)
      {
        parser.parse(c);
      }
    }
  }
}

void WeatherClient::whitespace(char c)
{
  Serial.println("whitespace");
}

// start json streaming parser event methods
void WeatherClient::startDocument()
{
  Serial.println("start document");
}

void WeatherClient::key(String key)
{
  currentKey = String(key);
}

void WeatherClient::value(String value)
{
  if (currentParent == "temperature")
  {
    if (currentKey == "value")
    {
      Serial.println("temp " + value);
      cToF(value);
    }
  }
  if (currentParent == "properties")
  {
    if (currentKey == "textDescription")
    {
      Serial.println("conditions " + value);
      currentCond = value;
    }
  }
}

void WeatherClient::endArray() {}

void WeatherClient::endObject()
{
  currentParent = "";
}

void WeatherClient::endDocument() {}

void WeatherClient::startArray() {}

void WeatherClient::startObject()
{
  currentParent = currentKey;
}
// end parser event methods

void WeatherClient::cToF(String c)
{
  Serial.println("celcius " + c);
  currentTemp = c.toInt() * 1.8 + 32;
  Serial.println("currentTemp F " + currentTemp);
}

int WeatherClient::getCurrentTemp()
{
  Serial.println("get current temp" + currentTemp);
  return currentTemp;
}

String WeatherClient::getCurrentConditions()
{
  Serial.println("get current conditions" + currentCond);
  return currentCond;
}
