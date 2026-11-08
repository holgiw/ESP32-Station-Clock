#pragma once
#include "globals.h"

    // mqtt_status.cpp - MQTT-Meldung des Status (z. B. fuer openHAB)
    // mqtt_status.cpp - MQTT status report (e.g. for openHAB)

    void loadMqttSettings();
    void mqttTick();
    String mqttStatusText();
    void setupMqttRoutes();

    extern bool mqttEnabled;
    extern String mqttHost;
    extern String mqttLastResult;
    extern unsigned long mqttLastAttempt;
    extern bool mqttPending;
