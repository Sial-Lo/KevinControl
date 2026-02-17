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
    Battery_Data_T *pBatteryData = NULL;
} Handle;

static void handleDcOutputStateChange(MyBLE_ReceiveData_T *receiveData);
static void handleAcOutputStateChange(MyBLE_ReceiveData_T *receiveData);

void Broker_Initialize(void)
{
    Handle.pBatteryData = Battery_GetDataPtr();
    pinMode(2, OUTPUT); //!< Set pin 2 as output for the LED indicating the battery connection status.
    pinMode(23, OUTPUT); //!< Set pin 23 as output for the dc charging.
}

void Broker_Update(void)
{
    if (STATE_BATTERY_DISCONNECTED == Handle.state)
    {
        if (Battery_IsConnected())
        {
            if (2000 < (millis() - Battery_IsConnectedTimestamp()))
            {
                Handle.state = STATE_BATTERY_CONNECTED;
                Serial.printf("INFO, %s, %i, Connection established.\n", __FILE__, __LINE__);

                MyBLE_Start();
                analogWrite(2, 5);

                MyBLE_ReceiveData_T receiveData;
                receiveData.dcOutputStateRequested = (Handle.pBatteryData->dcOutputOperatingState == 5u) ? 1 : 0;
                receiveData.acOutputStateRequested = (Handle.pBatteryData->inverterOperatingState == 5u) ? 1 : 0;
                receiveData.dcInputStateRequested = 0;
                MyBLE_SetReceive(&receiveData);
            }
        }
    }
    else if (STATE_BATTERY_CONNECTED == Handle.state)
    {
        if (!Battery_IsConnected())
        {
            Handle.state = STATE_BATTERY_DISCONNECTED;
            MyBLE_Stop();
            analogWrite(2, 0);
            Serial.printf("INFO, %s, %i, Battery disconnected.\n", __FILE__, __LINE__);
        }
        else
        {
            MyBLE_TransmitData_T transmitData;
            transmitData.soc = Handle.pBatteryData->soc;
            transmitData.dcOutputState = (Handle.pBatteryData->dcOutputOperatingState == 5u) ? 1 : 0;
            transmitData.acOutputState = (Handle.pBatteryData->inverterOperatingState == 5u) ? 1 : 0;
            transmitData.dcInputState = (Handle.pBatteryData->dcInputOperatingState == 5u) ? 1 : 0;
            transmitData.acInputState = (Handle.pBatteryData->chargerOperatingState == 5u) ? 1 : 0;
            transmitData.dcInputPower = Handle.pBatteryData->dcInputPower;
            transmitData.dcOutputPower = Handle.pBatteryData->dcOutputPower;
            transmitData.acInputPower = Handle.pBatteryData->acInputPower;
            transmitData.acOutputPower = Handle.pBatteryData->acOutputPower;
            transmitData.cellTemperatureAVG = Handle.pBatteryData->cellTemperatureAVG;
            MyBLE_Transmit(&transmitData);


            MyBLE_ReceiveData_T receiveData = MyBLE_Receive();
            handleDcOutputStateChange(&receiveData);
            handleAcOutputStateChange(&receiveData);

            // Update DC Input State.
            if (receiveData.dcInputStateRequested == 1)
            {
                digitalWrite(23, LOW);
            }
            else
            {
                digitalWrite(23, HIGH);
            }
        }


    }
}

static void handleDcOutputStateChange(MyBLE_ReceiveData_T *receiveData)
{
    static uint8_t previousDcOutputStateRequested = receiveData->dcOutputStateRequested;
    static bool changeRequested = false;
    bool currentDcOutputState = (Handle.pBatteryData->dcOutputOperatingState == 5u) ? 1 : 0;

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
    bool currentAcOutputState = (Handle.pBatteryData->inverterOperatingState == 5u) ? 1 : 0;

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