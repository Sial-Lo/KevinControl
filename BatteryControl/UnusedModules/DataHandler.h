#pragma once

#include <Arduino.h>

typedef struct DataHandler_Battery_T
{
    uint8_t soc;       /**< State of Charge in percentage (0-100%) */
    uint8_t voltage_12V;  /**< 12V line status (true = ON, false = OFF) */
    uint8_t voltage_230V; /**< 230V line status (true = ON, false = OFF) */
} DataHandler_Battery_T;

typedef struct DataHandler_T
{
    DataHandler_Battery_T Battery;
} DataHandler_T;

extern DataHandler_T *DataHandler_get(void);
