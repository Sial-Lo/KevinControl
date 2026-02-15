#pragma once

#include <Arduino.h>
#include "MyBLEServer.h"


extern void BLEBattery_Initialize(void);
extern BLECharacteristic *BLEBattery_GetCharacteristic(void);