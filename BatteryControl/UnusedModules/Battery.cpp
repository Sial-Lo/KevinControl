#include "Battery.h"
#include "MyBLEServer.h"
#include "MyCAN.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
static BLEUUID BLESERVICE_UUID(SERVICE_UUID);

typedef enum
{
    BLE_CHARACTERISTIC_100MS = 0u,
    BLE_CHARACTERISTIC_1000MS,
    BLE_CHARACTERISTIC_WRITE,

    BLE_CHARACTERISTIC_UNDEFINED
} BLE_Characteristic_E;

typedef struct
{
    const char *uuid;
    uint32_t properties;
    uint32_t notificationMask;
} BLE_Config_T;

typedef struct
{
    BLE_Config_T config;
    BLECharacteristic *pCharacteristic;
} BLE_T;

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
} CAN_Config_T;

typedef struct
{
    CAN_Config_T config;
    uint8_t data[8];
} CAN_T;

static void update100ms(void);
static void update1000ms(void);
static void updateWrite(void);

static void getSoc(uint8_t sendData[]);
static void getBatteryCurrent(uint8_t sendData[]);

static BLE_T gBLE[BLE_CHARACTERISTIC_UNDEFINED] = {
    [BLE_CHARACTERISTIC_100MS] = {.config = {.uuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8", .properties = (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ)}},
    [BLE_CHARACTERISTIC_1000MS] = {.config = {.uuid = "d7be7b90-2423-4d6e-926d-239bc96bb2fd", .properties = (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ)}},
    [BLE_CHARACTERISTIC_WRITE] = {.config = {.uuid = "aa6316cb-878e-4572-a94e-fec129349f85", .properties = (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE)}},
};

static CAN_T gCAN[CAN_FRAME_UNDEFINED] = {
    [CAN_FRAME_BatteryStatus] = {.config = {.id = 0x18FF0003}},
    [CAN_FRAME_DcInputAndOutput] = {.config = {.id = 0x18FF0103}},
};

static struct
{
    uint32_t notificationValue = 0u;
} Handle;

void Battery_Initialize(void)
{
    // Initialize BLE
    BLEService *pBleService = MyBLEServer_GetServer()->createService(BLESERVICE_UUID, 10, 0);
    for (uint8_t i = 0u; i < BLE_CHARACTERISTIC_UNDEFINED; i++)
    {

        gBLE[i].pCharacteristic = pBleService->createCharacteristic(gBLE[i].config.uuid, gBLE[i].config.properties);
        Serial.printf("INFO, %s, %i, Characteristic %s started. %#010x\n", __FILE__, __LINE__, gBLE[i].config.uuid, gBLE[i].pCharacteristic);
    }

    // Initialize CAN
    for (uint8_t i = 0u; i < BLE_CHARACTERISTIC_UNDEFINED; i++)
    {
        MyCAN_RegisterMessage(gCAN[i].config.id, gCAN[i].data);
        Serial.printf("INFO, %s, %i, Can message %#010x registered.\n", __FILE__, __LINE__, gCAN[i].config.id);
    }

    // Initialize write values

    MyBLEServer_StartService(pBleService);
    Serial.printf("INFO, %s, %i, Service Advertising started %s.\n", __FILE__, __LINE__, SERVICE_UUID);

    uint8_t data[] = {0,0};
    gBLE[BLE_CHARACTERISTIC_WRITE].pCharacteristic->setValue(data, sizeof(data));
}

void Battery_Update(void)
{
    update100ms();
    // update1000ms();
    updateWrite();
}

static void update100ms(void)
{
    CAN_T *pCAN = &gCAN[CAN_FRAME_BatteryStatus];
    BLE_T *pBLE = &gBLE[BLE_CHARACTERISTIC_100MS];

    uint8_t sendValue[7];

    getSoc(&sendValue[0]);
    getBatteryCurrent(&sendValue[1]);

    // Remaining Time
    sendValue[3] = pCAN->data[4];
    sendValue[4] = pCAN->data[5];

    // Battery Ah
    sendValue[5] = pCAN->data[6];
    sendValue[6] = pCAN->data[7];

    pBLE->pCharacteristic->setValue(sendValue, sizeof(sendValue));
    pBLE->pCharacteristic->notify();
}

static void update1000ms(void)
{
}

static void updateWrite(void)
{
    BLE_T *pBLE = &gBLE[BLE_CHARACTERISTIC_WRITE];
    static uint8_t lastValue12v = 0u;
    static uint8_t lastValue230v = 0u;

    uint8_t value12v = pBLE->pCharacteristic->getData()[0];
    uint8_t value230v = pBLE->pCharacteristic->getData()[1];

    if (value12v != lastValue12v)
    {
        uint8_t data[8];
        data[0] = 0x00;
        data[1] = 0x40;
        data[2] = 0x00;
        data[3] = 0x9F;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = value12v;
        
        MyCAN_SendMessage(0x19EF0301u, sizeof(data), data);
        lastValue12v = value12v;
    }

    if (value230v != lastValue230v)
    {
        uint8_t data[8];
        data[0] = 0x00;
        data[1] = 0x40;
        data[2] = 0x00;
        data[3] = 0x9E;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = value230v;

        MyCAN_SendMessage(0x19EF0301u, sizeof(data), data);
        lastValue230v = value230v;
    }

    pBLE->pCharacteristic->notify();
}

static void getSoc(uint8_t sendData[])
{
    uint16_t socRawValue = (gCAN[CAN_FRAME_BatteryStatus].data[0] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[1];
    sendData[0] = (uint8_t)map(socRawValue, 0, 0xFFFF, 0, 100);
}

static void getBatteryCurrent(uint8_t sendData[])
{
    int16_t batteryCurrentRaw = (gCAN[CAN_FRAME_BatteryStatus].data[2] << 8) | gCAN[CAN_FRAME_BatteryStatus].data[3];
    int16_t batteryCurrent = batteryCurrentRaw * 1000;
    sendData[0] = (uint8_t)batteryCurrent >> 8;
    sendData[1] = (uint8_t)batteryCurrent;
}