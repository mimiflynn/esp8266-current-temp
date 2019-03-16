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
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ApiWeatherGovClient.h"
bool usePM = false; // Set to true if you want to use AM/PM time disaply
bool isPM = false;  // JJG added ///////////

const String API_URL = "api.weather.gov";

ApiWeatherGovClient::ApiWeatherGovClient(boolean _isMetric)
{
  isMetric = _isMetric;
}

// Added by fowlerk, 12/22/16, as an option to change metric setting other than at instantiation
void ApiWeatherGovClient::initMetric(boolean _isMetric)
{
  isMetric = _isMetric;
}
// end add fowlerk, 12/22/16

void ApiWeatherGovClient::updateConditions(String stationId)
{
  isForecast = false;
  doUpdate("/stations/" + stationId + "/observations/latest");
}

void ApiWeatherGovClient::doUpdate(String url)
{
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(API_URL, httpPort))
  {
    Serial.println("connection failed");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host:" + API_URL + "\r\n" +
               "Connection: close\r\n\r\n");
  int retryCounter = 0;
  while (!client.available())
  {
    delay(1000);
    retryCounter++;
    if (retryCounter > 10)
    {
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

void ApiWeatherGovClient::whitespace(char c)
{
  Serial.println("whitespace");
}

void ApiWeatherGovClient::startDocument()
{
  Serial.println("start document");
}

void ApiWeatherGovClient::key(String key)
{
  currentKey = String(key);
}

void ApiWeatherGovClient::value(String value)
{
}

void ApiWeatherGovClient::endArray()
{
}

void ApiWeatherGovClient::startObject()
{
  currentParent = currentKey;
}

void ApiWeatherGovClient::endObject()
{
  currentParent = "";
}

void ApiWeatherGovClient::endDocument()
{
}

void ApiWeatherGovClient::startArray()
{
}

String ApiWeatherGovClient::getCurrentTemp()
{
  return currentTemp;
}

String ApiWeatherGovClient::getMeteoconIcon(String iconText)
{
  if (iconText == "chanceflurries")
    return "F";
  if (iconText == "chancerain")
    return "Q";
  if (iconText == "chancesleet")
    return "W";
  if (iconText == "chancesnow")
    return "V";
  if (iconText == "chancetstorms")
    return "S";
  if (iconText == "clear")
    return "B";
  if (iconText == "cloudy")
    return "Y";
  if (iconText == "flurries")
    return "F";
  if (iconText == "fog")
    return "M";
  if (iconText == "hazy")
    return "E";
  if (iconText == "mostlycloudy")
    return "Y";
  if (iconText == "mostlysunny")
    return "H";
  if (iconText == "partlycloudy")
    return "H";
  if (iconText == "partlysunny")
    return "J";
  if (iconText == "sleet")
    return "W";
  if (iconText == "rain")
    return "R";
  if (iconText == "snow")
    return "W";
  if (iconText == "sunny")
    return "B";
  if (iconText == "tstorms")
    return "0";

  if (iconText == "nt_chanceflurries")
    return "F";
  if (iconText == "nt_chancerain")
    return "7";
  if (iconText == "nt_chancesleet")
    return "#";
  if (iconText == "nt_chancesnow")
    return "#";
  if (iconText == "nt_chancetstorms")
    return "&";
  if (iconText == "nt_clear")
    return "2";
  if (iconText == "nt_cloudy")
    return "Y";
  if (iconText == "nt_flurries")
    return "9";
  if (iconText == "nt_fog")
    return "M";
  if (iconText == "nt_hazy")
    return "E";
  if (iconText == "nt_mostlycloudy")
    return "5";
  if (iconText == "nt_mostlysunny")
    return "3";
  if (iconText == "nt_partlycloudy")
    return "4";
  if (iconText == "nt_partlysunny")
    return "4";
  if (iconText == "nt_sleet")
    return "9";
  if (iconText == "nt_rain")
    return "7";
  if (iconText == "nt_snow")
    return "#";
  if (iconText == "nt_sunny")
    return "4";
  if (iconText == "nt_tstorms")
    return "&";

  return ")";
}
