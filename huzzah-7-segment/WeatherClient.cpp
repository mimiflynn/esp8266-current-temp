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
const char fingerprint[] PROGMEM = "1C E6 10 E0 6D 39 26 74 EE 44 3A 46 9B 44 99 77 AC A3 D4 72";

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

// start parser event methods
void WeatherClient::startDocument()
{
  Serial.println("start document");
}

void WeatherClient::key(String key)
{
  currentKey = String(key);
  Serial.println("currentKey " + currentKey);
}

void WeatherClient::value(String value)
{
  Serial.println("currentKey " + currentKey);
  Serial.println("value " + value);

  if (currentParent == "properties")
  {
    if (currentParent == "temperature")
    {
      if (currentKey == "value")
      {
        currentTemp = value;
      }
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

String WeatherClient::getCurrentTemp()
{
  return currentTemp;
}
