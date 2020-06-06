#pragma once

#include <ESP8266WiFi.h>
#include <MQTT.h>

class MqttTriggerEvent
{
public:
    typedef void (*handler_t)();

    MqttTriggerEvent(handler_t cb);
    ~MqttTriggerEvent();

    MqttTriggerEvent(const MqttTriggerEvent&) = delete;
    MqttTriggerEvent& operator=(const MqttTriggerEvent&) = delete;
 
    void Update();

private:
    handler_t _cb = nullptr;
    unsigned long _lastCbTime = 0;

    WiFiClient _netClient;
    MQTTClient _mqttClient;

    bool Connect();
    void Disconnect();

    void PublishTopics();
    void OnMqttMessage(String& topic, String& payload);
};