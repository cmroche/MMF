#ifndef UNIT_TEST

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#include <time.h>

#include "appconfig.h"
#include "timetriggerevent.h"
#include "feeder.h"
#include "webform.hpp"

template <class T>
int EEPROM_put(int ee, const T &value)
{
  const byte *p = (const byte *)(const void *)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}

template <class T>
int EEPROM_get(int ee, T &value)
{
  byte *p = (byte *)(void *)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}

WiFiUDP udpClient;
AsyncWebServer server(80);

#define SAFETY_DANCE 0xDEADBEEF
struct config_t
{
  unsigned long feedSteps = 50000;
  unsigned long safety = SAFETY_DANCE;
} _stored;

Feeder feeder;
TimeTriggerEvent timeTrigger([]()
                             { feeder.Feed(); });

void init_eeprom()
{
  EEPROM.begin(sizeof(_stored));

  config_t t;
  EEPROM_get(0, t);

  if (t.safety != SAFETY_DANCE)
  {
    Serial.println("Reinitializing EEPROM");
    EEPROM_put(0, _stored);
  }
  else
  {
    Serial.println("EEPROM loaded");
    _stored = t;
  }

  Serial.print("Feed steps: ");
  Serial.println(_stored.feedSteps, DEC);
  Serial.print("Safety dance: ");
  Serial.println(_stored.safety, HEX);
  feeder.SetFeedSteps(_stored.feedSteps);
}

void init_ntp()
{
  Serial.print("\nSetting up NTP");
  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", "EST5EDT,M3.2.0,M11.1.0", 1);

  time_t now = 0;
  const int EPOCH_1_1_2019 = 1546300800;
  while (now < EPOCH_1_1_2019)
  {
    now = time(nullptr);
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nSystem time is set: ");
  Serial.println(String(ctime(&now)));

  setSyncProvider([]()
                  {
                    time_t tnow = time(nullptr);
                    Serial.print("\nSynchronizing system time: ");
                    Serial.println(String(ctime(&tnow)));

                    tm *tm = localtime(&tnow);
                    tmElements_t tmElems;
                    tmElems.Year = tm->tm_year - 70;
                    tmElems.Month = tm->tm_mon + 1;
                    tmElems.Day = tm->tm_mday;
                    tmElems.Hour = tm->tm_hour;
                    tmElems.Minute = tm->tm_min;
                    tmElems.Second = tm->tm_sec;
                    time_t lnow = makeTime(tmElems);
                    return lnow;
                  });
}

// Replaces placeholder with stored values
String processor(const String &var)
{
  //Serial.println(var);
  if (var == "systemTime")
  {
    time_t tnow = time(nullptr);
    return String(ctime(&tnow));
  }
  else if (var == "feedSteps")
  {
    return String(_stored.feedSteps);
  }
  return String();
}

bool feed = false;

void setup()
{
  SPI.begin();
  Serial.begin(115200);
  delay(2000);

  init_eeprom();

  Serial.print("Setting up WiFi");
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  String clientId = String("MMF-") + String(ESP.getChipId(), HEX);
  Serial.print("Starting MDNS, client name ");
  Serial.println(clientId);
  if (!MDNS.begin(clientId))
  {
    Serial.println("Unable to setup MDNS responder");
  }
  MDNS.addService(0, "http", "tcp", 80);

  Serial.println("Starting HTTP OTA service");

  server.on("/", [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam(PARAM_FEED_STEPS))
              {
                String inputMessage = request->getParam(PARAM_FEED_STEPS)->value();
                _stored.feedSteps = atol(inputMessage.c_str());
                feeder.SetFeedSteps(_stored.feedSteps);
                EEPROM_put(0, _stored);
              }
              request->send(200, "text/text", "OK");
            });
  server.on("/feed", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              feed = true;
              request->send(200, "text/text", "OK");
            });

  AsyncElegantOTA.begin(&server);
  server.begin();

  init_ntp();

  // Sanity check
  if (timeStatus() == timeNotSet)
  {
    Serial.println("System time is not set, the timers will be disabled!");
  }

  // Init the feeder drivers
  Serial.print("\nInitializing drivers");
  feeder.InitDriver();
}

void loop()
{
  MDNS.update();
  timeTrigger.Update();

  if (feed == true)
  {
    feeder.Feed();
    feed = false;
  }
}

#endif