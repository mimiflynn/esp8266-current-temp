#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "WeatherClient.h"

// WIFI
const char *WIFI_SSID = "";
const char *WIFI_PWD = "";

// API Settings
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
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// init weather api library
WeatherClient weather;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up");

  alpha4.begin(0x70);
  // print a hex number
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDisplay();
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

  updateWeather();

  // Start timer
  ticker.attach(UPDATE_INTERVAL_SECS, setReadyForWeatherUpdate);
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
  weather.updateConditions(WEATHER_STATION);
  Serial.println("Weather Updated");
  int temp = weather.getCurrentTemp();
  char displayBuffer[4];
  Serial.println("Current Temp:" + temp);
  itoa(temp, displayBuffer, 10);
  // alpha4.writeDigitAscii(0, displayBuffer[0]);
  // alpha4.writeDigitAscii(1, displayBuffer[1]);
  alpha4.writeDigitAscii(2, displayBuffer[0]);
  alpha4.writeDigitAscii(3, displayBuffer[1]);
  alpha4.writeDisplay();
  Serial.println("Setting readyForUpdate to false");
  readyForWeatherUpdate = false;
}
