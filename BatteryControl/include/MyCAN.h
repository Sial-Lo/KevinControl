#pragma once

#include <Arduino.h>


extern bool MyCAN_Initialize(void);

extern void MyCAN_Update(void);

extern bool MyCAN_RegisterMessage(uint32_t messageId, uint8_t messageData[8]);

extern void MyCAN_SendMessage(uint32_t identifier, uint8_t length, uint8_t data[8]);