#pragma once
#include <ArduinoOTA.h>

inline constexpr const char* MODULE_VERSION_OTA = "4.0";

void initOTA(const char* hostname);
void handleOTA();