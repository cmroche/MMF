#ifndef UNIT_TEST

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "appconfig.h"
#include "timetriggerevent.h"
#include "mqtttriggerevent.h"
#include "feeder.h"

WiFiUDP udpClient;

// -5H for Eastern Time, we could adjust the offset automatically in the future
// NTPClient ntpClient(udpClient, "www.pool.ntp.org/zone/north-america", 0, 120000);
NTPClient ntpClient(udpClient, "time.nist.gov", -4*3600, 300000);

Feeder feeder;
TimeTriggerEvent timeTrigger([](){ feeder.Feed(); });
MqttTriggerEvent mqttTrigger([](){ feeder.Feed(); });

void setup() 
{
  Serial.begin(115200);
  delay(2000);

  Serial.print("Setting up WiFi");
  WiFi.setAutoConnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

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
    
    yield();
  }

  setSyncProvider([]() -> time_t { return ntpClient.getEpochTime(); });
  setSyncInterval((time_t)60UL);
}

void loop() 
{
  ntpClient.update(); 
  mqttTrigger.Update();
  timeTrigger.Update();
  //feeder.Update();

  // This requires a hardware setup, we'll look at it soon ...
  // ESP.deepSleep(10e6);
}

#endif