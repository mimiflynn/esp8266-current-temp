#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "../lib/ApiWeatherGovClient.h"

// WIFI
const char *WIFI_SSID = "";
const char *WIFI_PWD = "";

// Wunderground Settings
const boolean IS_METRIC = false;
const String WEATHER_STATION = "KNYC";

// Update every 10 minutes
const int UPDATE_INTERVAL_SECS = 10 * 60;

// Flag for timer function
bool readyForWeatherUpdate = false;

void setReadyForWeatherUpdate();
void updateWeather();

// init timer
Ticker ticker;

// set up the Display.
Adafruit_7segment matrix = Adafruit_7segment();

// init weather api library
WeatherClient weather(IS_METRIC);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up");

  matrix.begin(0x70);
  // print a hex number
  matrix.print(0xBEEF, HEX);
  matrix.writeDisplay();
  delay(500);

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  Serial.println("Connecting to WiFi");

  // Show status of WiFi connection
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;
  }

  Serial.println("Connected");

  // Start timer
  ticker.attach(UPDATE_INTERVAL_SECS, setReadyForWeatherUpdate);

  updateWeather();
}

void loop()
{
  if (readyForWeatherUpdate)
  {
    updateWeather();
  }
}

void setReadyForWeatherUpdate()
{
  Serial.println("Setting readyForUpdate to true");
  readyForWeatherUpdate = true;
}

void updateWeather()
{
  Serial.println("Updating weather");
  weather.initMetric(IS_METRIC);
  weather.updateConditions(WEATHER_STATION);
  Serial.println("Weather Updated");
  String temp = weather.getCurrentTemp();
  Serial.println("Current Temp:" + temp);
  matrix.print(temp.toInt());
  matrix.writeDisplay();
  Serial.println("Setting readyForUpdate to false");
  readyForWeatherUpdate = false;
}
