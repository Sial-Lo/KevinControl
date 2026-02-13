#pragma once

#include <Arduino.h>
#include <BLEDevice.h>


extern void MyBLEServer_Initialize(const char* name);

extern BLEServer* MyBLEServer_GetServer();

extern bool MyBLEServer_Connected();

extern void MyBLEServer_StartService(BLEService *pService);




