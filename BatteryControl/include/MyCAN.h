#pragma once

#include <Arduino.h>

typedef void (*MyCan_Callback)(uint32_t, uint8_t[8]);

extern bool MyCAN_Initialize(void);

extern void MyCAN_Update(void);

extern bool MyCAN_RegisterMessage(uint32_t messageId, uint32_t mask, MyCan_Callback callback);

extern void MyCAN_SendMessage(uint32_t identifier, uint8_t length, uint8_t data[8]);