#include "Battery.h"
#include "MyCAN.h"

typedef enum
{
    CAN_FRAME_BatteryStatus = 0u,
    CAN_FRAME_DcInputAndOutput,
    CAN_FRAME_OperatingState,
    CAN_FRAME_FunctionOperatingState,
    CAN_FRAME_FailureCodeBuffer,
    CAN_FRAME_BatteryTemperature,
    CAN_FRAME_AcInputAndOutput,
    CAN_FRAME_CellVoltage,
    CAN_FRAME_CellSOC,

    CAN_FRAME_UNDEFINED
} CAN_Frame_E;

typedef struct
{
    uint32_t id;
    uint8_t data[8];
} CAN_T;

static CAN_T gCAN[CAN_FRAME_UNDEFINED] = {
    [CAN_FRAME_BatteryStatus] = {.id = 0x18FF0000},
    [CAN_FRAME_DcInputAndOutput] = {.id = 0x18FF0100},
    [CAN_FRAME_OperatingState] = {.id = 0x18FF0300},
    [CAN_FRAME_FunctionOperatingState] = {.id = 0x18FF0400},
    [CAN_FRAME_FailureCodeBuffer] = {.id = 0x18FF0500},
    [CAN_FRAME_BatteryTemperature] = {.id = 0x18FF0600},
    [CAN_FRAME_AcInputAndOutput] = {.id = 0x18FF0900},
    [CAN_FRAME_CellVoltage] = {.id = 0x18FF1000},
    [CAN_FRAME_CellSOC] = {.id = 0x18FF1800},
};

static int16_t getDcInputPower(void);
static int16_t getDcOutputPower(void);
static int16_t getDcPower(void);
static int16_t getAcInputPower(void);
static int16_t getAcOutputPower(void);
static int16_t getAcPower(void);
static int16_t getBatteryPower(void);
static void canCallback(uint32_t identifier, uint8_t data[8]);

static struct
{
    uint8_t batteryId = 0xFFu;
    Battery_Data_T data;
    bool connected = false;
    uint64_t connectionTimestamp = 0u;
    uint64_t lastMessageTimestamp = 0u;
} Handle;

void Battery_Initialize(void)
{
    MyCAN_RegisterMessage(0x18FF0000u, 0xFFFF0000u, canCallback);
}

void Battery_Update(void)
{
    if (Handle.connected)
    {
        if (2000 < (millis() - Handle.lastMessageTimestamp))
        {
            Handle.connected = false;
            Handle.connectionTimestamp = 0u;
            Serial.printf("INFO, %s, %i, Battery disconnected.\n", __FILE__, __LINE__);
        }
        else
        {
            
            Handle.data.soc = (uint16_t)((gCAN[CAN_FRAME_BatteryStatus].data[0] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[1]);
            Handle.data.batteryCurrent = (int16_t)((gCAN[CAN_FRAME_BatteryStatus].data[2] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[3]);
            Handle.data.reaminingTime = (int16_t)((gCAN[CAN_FRAME_BatteryStatus].data[4] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[5]);
            Handle.data.batteryAh = (int16_t)((gCAN[CAN_FRAME_BatteryStatus].data[6] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[7]);

            Handle.data.dcInputVoltage = (uint16_t)((gCAN[CAN_FRAME_DcInputAndOutput].data[0] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[1]);
            Handle.data.dcInputCurrent = (int16_t)((gCAN[CAN_FRAME_DcInputAndOutput].data[2] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[3]);
            Handle.data.dcOutputVoltage = (uint16_t)((gCAN[CAN_FRAME_DcInputAndOutput].data[4] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[5]);
            Handle.data.dcOutputCurrent = (int16_t)((gCAN[CAN_FRAME_DcInputAndOutput].data[6] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[7]);

            Handle.data.operatingState = (Battery_OperatingState_E)gCAN[CAN_FRAME_OperatingState].data[0];
            Handle.data.failureLevel = (Battery_FailureLevel_E)gCAN[CAN_FRAME_OperatingState].data[1];
            Handle.data.lpsTestMode = (Battery_LpsTestMode_E)gCAN[CAN_FRAME_OperatingState].data[2];
            Handle.data.lpsWakeupFlag = (Battery_LpsWakeupFlag_BF)gCAN[CAN_FRAME_OperatingState].data[3];
            Handle.data.lpsInputState = (Battery_LpsInputState_BF)gCAN[CAN_FRAME_OperatingState].data[4];
            Handle.data.lpsOutputState = (Battery_LpsOutputState_BF)gCAN[CAN_FRAME_OperatingState].data[5];
            Handle.data.cellCount = gCAN[CAN_FRAME_OperatingState].data[6];
            Handle.data.temperatureSensorCount = gCAN[CAN_FRAME_OperatingState].data[7];

            Handle.data.inverterOperatingState = (Battery_FunctionOperatingState_E)gCAN[CAN_FRAME_FunctionOperatingState].data[0];
            Handle.data.inverterFailureLevel = (Battery_FailureLevel_E)gCAN[CAN_FRAME_FunctionOperatingState].data[1];
            Handle.data.chargerOperatingState = (Battery_FunctionOperatingState_E)gCAN[CAN_FRAME_FunctionOperatingState].data[2];
            Handle.data.chargerFailureLevel = (Battery_FailureLevel_E)gCAN[CAN_FRAME_FunctionOperatingState].data[3];
            Handle.data.dcInputOperatingState = (Battery_FunctionOperatingState_E)gCAN[CAN_FRAME_FunctionOperatingState].data[4];
            Handle.data.dcInputFailureLevel = (Battery_FailureLevel_E)gCAN[CAN_FRAME_FunctionOperatingState].data[5];
            Handle.data.dcOutputOperatingState = (Battery_FunctionOperatingState_E)gCAN[CAN_FRAME_FunctionOperatingState].data[6];
            Handle.data.dcOutputFailureLevel = (Battery_FailureLevel_E)gCAN[CAN_FRAME_FunctionOperatingState].data[7];

            Handle.data.internalTemperature1 = (int16_t)((gCAN[CAN_FRAME_BatteryTemperature].data[0] << 8) | gCAN[CAN_FRAME_BatteryTemperature].data[1]);
            Handle.data.internalTemperature2 = (int16_t)((gCAN[CAN_FRAME_BatteryTemperature].data[2] << 8) | gCAN[CAN_FRAME_BatteryTemperature].data[3]);
            Handle.data.internalTemperature3 = (int16_t)((gCAN[CAN_FRAME_BatteryTemperature].data[4] << 8) | gCAN[CAN_FRAME_BatteryTemperature].data[5]);
            Handle.data.cellTemperatureAVG = (int16_t)((gCAN[CAN_FRAME_BatteryTemperature].data[6] << 8) | gCAN[CAN_FRAME_BatteryTemperature].data[7]);

            Handle.data.acInputVoltage = (uint16_t)((gCAN[CAN_FRAME_AcInputAndOutput].data[0] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[1]);
            Handle.data.acInputCurrent = (int16_t)((gCAN[CAN_FRAME_AcInputAndOutput].data[2] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[3]);
            Handle.data.acOutputVoltage = (uint16_t)((gCAN[CAN_FRAME_AcInputAndOutput].data[4] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[5]);
            Handle.data.acOutputCurrent = (int16_t)((gCAN[CAN_FRAME_AcInputAndOutput].data[6] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[7]);

            Handle.data.voltageCell1 = (uint16_t)((gCAN[CAN_FRAME_CellVoltage].data[0] << 8) | gCAN[CAN_FRAME_CellVoltage].data[1]);
            Handle.data.voltageCell2 = (uint16_t)((gCAN[CAN_FRAME_CellVoltage].data[2] << 8) | gCAN[CAN_FRAME_CellVoltage].data[3]);
            Handle.data.voltageCell3 = (uint16_t)((gCAN[CAN_FRAME_CellVoltage].data[4] << 8) | gCAN[CAN_FRAME_CellVoltage].data[5]);
            Handle.data.voltageCell4 = (uint16_t)((gCAN[CAN_FRAME_CellVoltage].data[6] << 8) | gCAN[CAN_FRAME_CellVoltage].data[7]);

            Handle.data.socCell1 = (uint16_t)((gCAN[CAN_FRAME_CellSOC].data[0] << 8) | gCAN[CAN_FRAME_CellSOC].data[1]);
            Handle.data.socCell2 = (uint16_t)((gCAN[CAN_FRAME_CellSOC].data[2] << 8) | gCAN[CAN_FRAME_CellSOC].data[3]);
            Handle.data.socCell3 = (uint16_t)((gCAN[CAN_FRAME_CellSOC].data[4] << 8) | gCAN[CAN_FRAME_CellSOC].data[5]);
            Handle.data.socCell4 = (uint16_t)((gCAN[CAN_FRAME_CellSOC].data[6] << 8) | gCAN[CAN_FRAME_CellSOC].data[7]);

            Handle.data.dcInputPower = getDcInputPower();
            Handle.data.dcOutputPower = getDcOutputPower();
            Handle.data.dcPower = getDcPower();
            Handle.data.acInputPower = getAcInputPower();
            Handle.data.acOutputPower = getAcOutputPower();
            Handle.data.acPower = getAcPower();
            Handle.data.batteryPower = getBatteryPower();
        }
    }
}

bool Battery_IsConnected(void)
{
    return Handle.connected;
}

uint64_t Battery_IsConnectedTimestamp(void)
{
    return Handle.connectionTimestamp;
}

Battery_Data_T *Battery_GetDataPtr(void)
{
    return &Handle.data;
}

void Battery_SetDcOutput(Battery_OutputState_E state)
{
    if (0xFF == Handle.batteryId)
    {
        Serial.printf("ERROR, %s, %i, The device id is not set. Can't send messages.\n", __FILE__, __LINE__);
    }
    else
    {
        Serial.printf("INFO, %s, %i, Setting DC output state to %i.\n", __FILE__, __LINE__, state);

        uint32_t id = 0x19EF00FFu | (Handle.batteryId << 8);
        uint8_t data[8];
        data[0] = 0x00;
        data[1] = 0x40;
        data[2] = 0x00;
        data[3] = 0x9F;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = (uint8_t)state;

        MyCAN_SendMessage(id, sizeof(data), data);
    }
}

void Battery_SetAcOutput(Battery_OutputState_E state)
{
    if (0xFF == Handle.batteryId)
    {
        Serial.printf("ERROR, %s, %i, The device id is not set. Can't send messages.\n", __FILE__, __LINE__);
    }
    else
    {
        Serial.printf("INFO, %s, %i, Setting DC output state to %i.\n", __FILE__, __LINE__, state);

        uint32_t id = 0x19EF00FFu | (Handle.batteryId << 8);
        uint8_t data[8];
        data[0] = 0x00;
        data[1] = 0x40;
        data[2] = 0x00;
        data[3] = 0x9E;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = (uint8_t)state;

        MyCAN_SendMessage(id, sizeof(data), data);
    }
}

static int16_t getDcInputPower(void)
{
    float voltage = (float)Handle.data.dcInputVoltage * 0.01f;
    float current = (float)Handle.data.dcInputCurrent * 0.01f;

    int16_t power = (int16_t)(voltage * current);

    return power;
}

static int16_t getDcOutputPower(void)
{
    float voltage = (float)Handle.data.dcOutputVoltage * 0.01f;
    float current = (float)Handle.data.dcOutputCurrent * 0.01f;

    int16_t power = (int16_t)(voltage * current);

    return power;
}

static int16_t getDcPower(void)
{
    return (getDcInputPower() - getDcOutputPower());
}

static int16_t getAcInputPower(void)
{
    float voltage = (float)Handle.data.acInputVoltage * 0.01f;
    float current = (float)Handle.data.acInputCurrent * 0.01f;

    int16_t power = (int16_t)(voltage * current);

    return power;
}

static int16_t getAcOutputPower(void)
{
    float voltage = (float)Handle.data.acOutputVoltage * 0.01f;
    float current = (float)Handle.data.acOutputCurrent * 0.01f;

    int16_t power = (int16_t)(voltage * current);

    return power;
}

static int16_t getAcPower(void)
{
    return (getAcInputPower() - getAcOutputPower());
}

static int16_t getBatteryPower(void)
{
    return getDcPower() + getAcPower();
}

static void canCallback(uint32_t identifier, uint8_t data[8])
{
    Handle.lastMessageTimestamp = millis();

    if (Handle.connected == false)
    {
        Handle.connected = true;
        Handle.connectionTimestamp = millis();
        Serial.printf("INFO, %s, %i, Battery connected.\n", __FILE__, __LINE__);
    }

    // Update the device ID since the battery sometimes changes it. The device ID is the second last byte of the identifier.
    Handle.batteryId = (uint8_t)(identifier & 0xFFu);

    for (uint8_t i = 0u; i < CAN_FRAME_UNDEFINED; i++)
    {
        if ((identifier & 0xFFFFFF00u) == (gCAN[i].id & 0xFFFFFF00u))
        {
            for (uint8_t j = 0u; j < 8u; j++)
            {
                gCAN[i].data[j] = data[j];
            }
        }
    }
}