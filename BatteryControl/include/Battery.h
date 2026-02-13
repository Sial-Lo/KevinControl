#pragma once

#include <Arduino.h>

typedef enum
{
    BATTERY_OUTPUT_STATE_OFF = 0u,
    BATTERY_OUTPUT_STATE_ON = 1u,
} Battery_OutputState_E;

extern void Battery_Initialize(void);
extern void Battery_SetOutputDC(Battery_OutputState_E state);
extern void Battery_SetOutputAC(Battery_OutputState_E state);
extern uint8_t Battery_GetSoc(void);
extern int16_t Battery_GetCurrent(void);
extern int16_t Battery_GetRemainingTime(void);
extern int16_t Battery_GetBatteryAh(void);
extern int16_t Battery_GetDcPower(void);
extern int16_t Battery_GetAcPower(void);
extern int16_t Battery_GetPower(void);
