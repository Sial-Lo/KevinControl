#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

typedef struct
{
    size_t length;
    uint8_t *data;
} MyBLE_Data_T;

typedef struct
{
    uint16_t soc;
    uint8_t dcOutputState;
    uint8_t acOutputState;
    uint8_t dcInputState;
    uint8_t acInputState;
    int16_t dcInputPower;
    int16_t dcOutputPower;
    int16_t acInputPower;
    int16_t acOutputPower;
    int16_t cellTemperatureAVG;
} MyBLE_TransmitData_T;

typedef struct
{
    uint8_t dcOutputStateRequested;
    uint8_t acOutputStateRequested;
    uint8_t dcInputStateRequested;
} MyBLE_ReceiveData_T;


extern void MyBLE_Initialize(const char* name);

extern bool MyBLE_DeviceConnected();

extern void MyBLE_Start(void);

extern void MyBLE_Stop(void);

extern void MyBLE_Transmit(uint8_t *transmitData, size_t dataSize);

extern void MyBLE_SetReceive(MyBLE_ReceiveData_T *receiveData);

extern MyBLE_ReceiveData_T MyBLE_Receive(void);




