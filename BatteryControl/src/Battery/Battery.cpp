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
    [CAN_FRAME_BatteryStatus] = {.id = 0x18FF0003},
    [CAN_FRAME_DcInputAndOutput] = {.id = 0x18FF0103},
    [CAN_FRAME_OperatingState] = {.id = 0x18FF0303},
    [CAN_FRAME_FunctionOperatingState] = {.id = 0x18FF0403},
    [CAN_FRAME_FailureCodeBuffer] = {.id = 0x18FF0503},
    [CAN_FRAME_BatteryTemperature] = {.id = 0x18FF0603},
    [CAN_FRAME_AcInputAndOutput] = {.id = 0x18FF0903},
    [CAN_FRAME_CellVoltage] = {.id = 0x18FF1003},
    [CAN_FRAME_CellSOC] = {.id = 0x18FF1803},
};

static struct
{
    uint32_t notificationValue = 0u;
} Handle;

void Battery_Initialize(void)
{
    // Initialize CAN
    for (uint8_t i = 0u; i < CAN_FRAME_UNDEFINED; i++)
    {
        MyCAN_RegisterMessage(gCAN[i].id, gCAN[i].data);
    }
}

void Battery_SetOutputDC(Battery_OutputState_E state)
{
    uint8_t data[8];
    data[0] = 0x00;
    data[1] = 0x40;
    data[2] = 0x00;
    data[3] = 0x9F;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = (uint8_t)state;

    MyCAN_SendMessage(0x19EF0301u, sizeof(data), data);
}

void Battery_SetOutputAC(Battery_OutputState_E state)
{
    uint8_t data[8];
    data[0] = 0x00;
    data[1] = 0x40;
    data[2] = 0x00;
    data[3] = 0x9E;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = (uint8_t)state;

    MyCAN_SendMessage(0x19EF0301u, sizeof(data), data);
}

uint8_t Battery_GetSoc(void)
{
    uint16_t socRawValue = (gCAN[CAN_FRAME_BatteryStatus].data[0] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[1];
    return (uint8_t)map(socRawValue, 0, 0xFFFF, 0, 100);
}

int16_t Battery_GetCurrent(void)
{
    return (int16_t)((gCAN[CAN_FRAME_BatteryStatus].data[2] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[3]);
}

int16_t Battery_GetRemainingTime(void)
{
    return (int16_t)((gCAN[CAN_FRAME_BatteryStatus].data[4] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[5]);
}

int16_t Battery_GetBatteryAh(void)
{
    return (int16_t)((gCAN[CAN_FRAME_BatteryStatus].data[6] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[7]);
}

int16_t Battery_GetDcPower(void)
{
    float input_voltage = (float)((int16_t)(((gCAN[CAN_FRAME_DcInputAndOutput].data[0] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[1])) * 0.01);
    float input_current = (float)((int16_t)(((gCAN[CAN_FRAME_DcInputAndOutput].data[2] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[3])) * 0.01);
    float output_voltage = (float)((int16_t)(((gCAN[CAN_FRAME_DcInputAndOutput].data[4] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[5])) * 0.01);
    float output_current = (float)((int16_t)(((gCAN[CAN_FRAME_DcInputAndOutput].data[6] << 8) | gCAN[CAN_FRAME_DcInputAndOutput].data[7])) * 0.01);

    float input_power = input_voltage * input_current;
    float output_power = output_voltage * output_current;

    int16_t dc_power = (int16_t)(input_power - output_power);

    return dc_power;
}

int16_t Battery_GetAcPower(void)
{
    float input_voltage = (float)((int16_t)(((gCAN[CAN_FRAME_AcInputAndOutput].data[0] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[1])) * 0.01);
    float input_current = (float)((int16_t)(((gCAN[CAN_FRAME_AcInputAndOutput].data[2] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[3])) * 0.01);
    float output_voltage = (float)((int16_t)(((gCAN[CAN_FRAME_AcInputAndOutput].data[4] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[5])) * 0.01);
    float output_current = (float)((int16_t)(((gCAN[CAN_FRAME_AcInputAndOutput].data[6] << 8) | gCAN[CAN_FRAME_AcInputAndOutput].data[7])) * 0.01);

    float input_power = input_voltage * input_current;
    float output_power = output_voltage * output_current;

    int16_t dc_power = (int16_t)(input_power - output_power);

    return dc_power;
}

int16_t Battery_GetPower(void)
{
    return Battery_GetDcPower() + Battery_GetAcPower();
}