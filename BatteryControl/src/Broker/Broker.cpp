#include "Broker.h"
#include "Battery.h"
#include "MyBLE.h"

typedef enum
{
    STATE_BATTERY_DISCONNECTED = 0u,
    STATE_BATTERY_CONNECTED,
} DataBroker_State_E;

static struct Handle
{
    DataBroker_State_E state = STATE_BATTERY_DISCONNECTED;
    bool batteryConnected = false;
} Handle;

static void handleDcOutputStateChange(MyBLE_ReceiveData_T *receiveData);
static void handleAcOutputStateChange(MyBLE_ReceiveData_T *receiveData);

void Broker_Initialize(void)
{
}

void Broker_Update(void)
{
    if (STATE_BATTERY_DISCONNECTED == Handle.state)
    {
        if (true == Battery_IsConnected())
        {
            if (1000 < (millis() - Battery_IsConnectedTimestamp()))
            {
                Handle.state = STATE_BATTERY_CONNECTED;

                MyBLE_ReceiveData_T receiveData;
                receiveData.dcOutputStateRequested = Battery_GetData(REQUEST_DC_OUTPUT_OPERATING_STATE);
                receiveData.acOutputStateRequested = Battery_GetData(REQUEST_AC_OUTPUT_OPERATING_STATE);
                receiveData.dcInputStateRequested = 0;
                MyBLE_SetReceive(&receiveData);
            }
        }
    }
    else if (STATE_BATTERY_CONNECTED == Handle.state)
    {
        if (false == Battery_IsConnected())
        {
            Handle.state = STATE_BATTERY_DISCONNECTED;
            Serial.printf("INFO, %s, %i, Battery disconnected.\n", __FILE__, __LINE__);
        }
    }

    if (true == MyBLE_DeviceConnected())
    {
        if (STATE_BATTERY_DISCONNECTED == Handle.state)
        {
            Battery_Wakeup();
        }

        uint32_t transmitData[(uint8_t)REQUEST_UNDEFINED] = {0};
        for (uint8_t i = 0; i < (uint8_t)REQUEST_UNDEFINED; i++)
        {
            transmitData[i] = Battery_GetData((Requests_E)i);
        }
        MyBLE_Transmit((uint8_t *)transmitData, sizeof(transmitData));

        MyBLE_ReceiveData_T receiveData = MyBLE_Receive();
        handleDcOutputStateChange(&receiveData);
        handleAcOutputStateChange(&receiveData);

        // Update DC Input State.
        Battery_setDcInput(receiveData.dcInputStateRequested == 1 ? BATTERY_OUTPUT_STATE_ON : BATTERY_OUTPUT_STATE_OFF);
    }
}

static void handleDcOutputStateChange(MyBLE_ReceiveData_T *receiveData)
{
    static uint8_t previousDcOutputStateRequested = receiveData->dcOutputStateRequested;
    static bool changeRequested = false;
    bool currentDcOutputState = (bool)Battery_GetData(REQUEST_DC_OUTPUT_OPERATING_STATE);

    if (receiveData->dcOutputStateRequested != previousDcOutputStateRequested)
    {
        changeRequested = true;
    }

    if (changeRequested)
    {
        if (receiveData->dcOutputStateRequested != currentDcOutputState)
        {
            Battery_SetDcOutput((receiveData->dcOutputStateRequested == 1) ? BATTERY_OUTPUT_STATE_ON : BATTERY_OUTPUT_STATE_OFF);
        }
        else
        {
            changeRequested = false;
            previousDcOutputStateRequested = receiveData->dcOutputStateRequested;
        }
    }

    if (receiveData->dcOutputStateRequested != currentDcOutputState)
    {
        if (false == changeRequested)
        {
            receiveData->dcOutputStateRequested = currentDcOutputState;
            previousDcOutputStateRequested = currentDcOutputState;
            MyBLE_SetReceive(receiveData);
        }
    }
}

static void handleAcOutputStateChange(MyBLE_ReceiveData_T *receiveData)
{
    static uint8_t previousAcOutputStateRequested = receiveData->acOutputStateRequested;
    static bool changeRequested = false;
    bool currentAcOutputState = (bool)Battery_GetData(REQUEST_AC_OUTPUT_OPERATING_STATE);

    if (receiveData->acOutputStateRequested != previousAcOutputStateRequested)
    {
        changeRequested = true;
    }

    if (changeRequested)
    {
        if (receiveData->acOutputStateRequested != currentAcOutputState)
        {
            Battery_SetAcOutput((receiveData->acOutputStateRequested == 1) ? BATTERY_OUTPUT_STATE_ON : BATTERY_OUTPUT_STATE_OFF);
        }
        else
        {
            changeRequested = false;
            previousAcOutputStateRequested = receiveData->acOutputStateRequested;
        }
    }

    if (receiveData->acOutputStateRequested != currentAcOutputState)
    {
        if (false == changeRequested)
        {
            receiveData->acOutputStateRequested = currentAcOutputState;
            previousAcOutputStateRequested = currentAcOutputState;
            MyBLE_SetReceive(receiveData);
        }
    }
}