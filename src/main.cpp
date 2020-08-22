#ifndef UNIT_TEST

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ElegantOTA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "appconfig.h"
#include "timetriggerevent.h"
#include "mqtttriggerevent.h"
#include "feeder.h"

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

void update_feed_amount(unsigned long val);

WiFiUDP udpClient;

// -5H for Eastern Time, we could adjust the offset automatically in the future
// NTPClient ntpClient(udpClient, "www.pool.ntp.org/zone/north-america", 0, 120000);
NTPClient ntpClient(udpClient, "time.nist.gov", -4 * 3600, 120000);

ESP8266WebServer server(80);

#define SAFETY_DANCE 0xDEADBEEF
struct config_t
{
  unsigned long feedSteps = 50000;
  unsigned long safety = SAFETY_DANCE;
} _stored;

Feeder feeder;
TimeTriggerEvent timeTrigger([]() { feeder.Feed(); });
MqttTriggerEvent mqttTrigger([]() { feeder.Feed(); }, update_feed_amount);

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

void update_feed_amount(unsigned long val)
{
  _stored.feedSteps = val;
  EEPROM_put(0, _stored);
  EEPROM.commit();

  Serial.println("EEPROM saved");
  ESP.restart();
}

void init_ntp()
{
  Serial.print("\nSetting up NTP");
  ntpClient.begin();

  auto start = millis();
  while (true)
  {
    if (millis() - start > 65000)
    {
      start = millis();
      Serial.print(".");
      if (ntpClient.update())
      {
        tmElements_t ntpElemTime;
        breakTime(ntpClient.getEpochTime(), ntpElemTime);

        String ntpStr = String(monthShortStr(ntpElemTime.Month)) + " " + ntpElemTime.Day + " " + ntpElemTime.Hour + ":" + ntpElemTime.Minute + ":" + ntpElemTime.Second;
        Serial.printf("\nSynchronized time with NTP %s\n", ntpStr.c_str());
        break;
      }
    }

    delay(1000);
  }

  setSyncProvider([]() -> time_t { return ntpClient.getEpochTime(); });
  setSyncInterval((time_t)120UL);
}

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

  Serial.println("Starting MDNS");
  String clientId = String("MMF-") + String(ESP.getChipId(), HEX);
  if (!MDNS.begin(clientId))
  {
    Serial.println("Unable to setup MDNS responder");
  }

  Serial.println("Starting HTTP OTA service");
  server.on("/", []() {
    server.send(200, "text/plain", "Meow meow meow meow meow meow meow meow meow meow.");
  });

  ElegantOTA.begin(&server);
  server.begin();

  init_ntp();

  // Init the feeder drivers
  Serial.print("Initializing drivers");
  feeder.InitDriver();
}

void loop()
{
  MDNS.update();
  ntpClient.update();
  mqttTrigger.Update();
  timeTrigger.Update();

  server.handleClient();

  // This requires a hardware setup, we'll look at it soon ...
  // ESP.deepSleep(10e6);
}

#endif