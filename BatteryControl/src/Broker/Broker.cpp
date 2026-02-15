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

void Broker_Initialize(void)
{
    Handle.pBatteryData = Battery_GetDataPtr();
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

                MyBLE_ReceiveData_T receiveData;
                receiveData.dcOutputStateRequested = (Handle.pBatteryData->dcOutputOperatingState == 5u) ? 1 : 0;
                receiveData.acOutputStateRequested = (Handle.pBatteryData->inverterOperatingState == 5u) ? 1 : 0;
                MyBLE_InitializeReceive(&receiveData);
            }
        }
    }
    else if (STATE_BATTERY_CONNECTED == Handle.state)
    {
        if (!Battery_IsConnected())
        {
            Handle.state = STATE_BATTERY_DISCONNECTED;
            MyBLE_Stop();
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
            if (receiveData.dcOutputStateRequested != ((Handle.pBatteryData->dcOutputOperatingState == 5u) ? 1 : 0))
            {
                Battery_SetDcOutput((receiveData.dcOutputStateRequested == 1) ? BATTERY_OUTPUT_STATE_ON : BATTERY_OUTPUT_STATE_OFF);
            }

            if (receiveData.acOutputStateRequested != ((Handle.pBatteryData->inverterOperatingState == 5u) ? 1 : 0))
            {
                Battery_SetAcOutput((receiveData.acOutputStateRequested == 1) ? BATTERY_OUTPUT_STATE_ON : BATTERY_OUTPUT_STATE_OFF);
            }
        }


    }
}
