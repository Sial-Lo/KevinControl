#pragma once

#include <Arduino.h>

typedef enum
{
    BATTERY_OUTPUT_STATE_OFF = 0u,
    BATTERY_OUTPUT_STATE_ON = 1u,
} Battery_OutputState_E;

typedef enum
{
    REQUEST_BATTERY_VOLTAGE,
    REQUEST_AC_INPUT_POWER,
    REQUEST_AC_OUTPUT_POWER,
    REQUEST_DC_INPUT_POWER,
    REQUEST_DC_OUTPUT_POWER,
    REQUEST_SOC,
    REQUEST_CELL_TEMPERATURE,
    REQUEST_BATTERY_POWER,
    REQUEST_AC_INPUT_OPERATING_STATE,
    REQUEST_AC_OUTPUT_OPERATING_STATE,
    REQUEST_DC_INPUT_OPERATING_STATE,
    REQUEST_DC_OUTPUT_OPERATING_STATE,
    REQUEST_UNDEFINED,
} Requests_E;

/**
 * @brief Initializes the battery module. This should be called once at the start of the program.
 */
extern void Battery_Initialize(void);

/**
 * @brief Updates the battery data. This should be called periodically to update the battery data and send notifications if necessary.
 */
extern void Battery_Update(void);

/**
 * @brief Checks if the battery is currently connected.
 */
extern bool Battery_IsConnected(void);

/**
 * @brief Gets the timestamp of when the battery was connected. This returns 0 if the battery is not currently connected.
 */
extern uint64_t Battery_IsConnectedTimestamp(void);

/**
 * @brief Gets a pointer to the battery data structure. This can be used to read and write the current battery data.
 */
extern uint32_t Battery_GetData(Requests_E request);

extern void Battery_setDcInput(Battery_OutputState_E state);

extern void Battery_SetDcOutput(Battery_OutputState_E state);

extern void Battery_SetAcOutput(Battery_OutputState_E state);

extern void Battery_Wakeup(void);