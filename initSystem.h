#pragma once
#include <Arduino.h>

inline constexpr const char* MODULE_VERSION_INIT_SYSTEM = "4.9";

void initPins();
void initSerial();
void initSensors();
void initInputs();
void initConnection();
void initTimers();
void reportStatus();