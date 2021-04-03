#pragma once

#include <ESP8266WiFi.h>
#include <MQTT.h>

class MqttTriggerEvent
{
private:
    String _clientId;
    String _baseTopic;

public:
    typedef void (*handler_t)();
    typedef void (*qty_handler_t)(unsigned long);

    MqttTriggerEvent(handler_t cb, qty_handler_t qty_cb);
    ~MqttTriggerEvent();

    MqttTriggerEvent(const MqttTriggerEvent &) = delete;
    MqttTriggerEvent &operator=(const MqttTriggerEvent &) = delete;

    void SetClientId(String &clientId);
    void SetAmount(unsigned long);
    void Update();

private:
    qty_handler_t _qty_cb = nullptr;
    handler_t _cb = nullptr;
    unsigned long _lastCbTime = 0;

    unsigned long _lastAmount = 0;
    bool _amountDirty = false;

    WiFiClient _netClient;
    MQTTClient _mqttClient;

    bool Connect();
    void Disconnect();

    void PublishTopics();
    void OnMqttMessage(String &topic, String &payload);
};