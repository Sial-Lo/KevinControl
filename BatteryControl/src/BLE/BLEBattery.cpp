#include "BLEBattery.h"
#include "MyBLEServer.h"
#include "Battery.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
static BLEUUID BLESERVICE_UUID(SERVICE_UUID);

typedef enum
{
    BLE_CHARACTERISTIC_BASIC = 0u,
    BLE_CHARACTERISTIC_DETAILED,
    BLE_CHARACTERISTIC_WRITE,

    BLE_CHARACTERISTIC_UNDEFINED
} Characteristic_E;

typedef struct
{
    const char *uuid;
    uint32_t properties;
} Characteristic_Config_T;

static void updateBasic(void);
static void updateDetailed(void);
static void updateWrite(void);

static struct
{
    BLEService *pService = NULL;
    BLECharacteristic *pCharacteristic[BLE_CHARACTERISTIC_UNDEFINED] = {NULL};
} gHandle;

Characteristic_Config_T gCharacteristicConfig[BLE_CHARACTERISTIC_UNDEFINED] = {
    [BLE_CHARACTERISTIC_BASIC] = {"beb5483e-36e1-4688-b7f5-ea07361b26a8", (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ)},
    [BLE_CHARACTERISTIC_DETAILED] = {"d7be7b90-2423-4d6e-926d-239bc96bb2fd", (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ)},
    [BLE_CHARACTERISTIC_WRITE] = {"aa6316cb-878e-4572-a94e-fec129349f85", (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE)},
};

void BLEBattery_Initialize(void)
{
    gHandle.pService = MyBLEServer_GetServer()->createService(BLESERVICE_UUID, 10, 0);
    for (uint8_t i = 0u; i < BLE_CHARACTERISTIC_UNDEFINED; i++)
    {
        gHandle.pCharacteristic[i] = gHandle.pService->createCharacteristic(gCharacteristicConfig[i].uuid, gCharacteristicConfig[i].properties);
        // Serial.printf("INFO, %s, %i, Characteristic %s started. %#010x\n", __FILE__, __LINE__, gCharacteristicConfig[i].uuid, gHandle.pCharacteristic[i]);
    }

    MyBLEServer_StartService(gHandle.pService);
    // Serial.printf("INFO, %s, %i, Service Advertising started.\n", __FILE__, __LINE__);
}

void BLEBattery_Update(void)
{
    updateBasic();
    updateDetailed();
    updateWrite();
}

static void updateBasic(void)
{
    uint8_t sendValue[7];

    sendValue[0] = Battery_GetSoc();

    sendValue[1] = (uint8_t)(Battery_GetPower() >> 8);
    sendValue[2] = (uint8_t)(Battery_GetPower());

    gHandle.pCharacteristic[BLE_CHARACTERISTIC_BASIC]->setValue(sendValue, sizeof(sendValue));
    gHandle.pCharacteristic[BLE_CHARACTERISTIC_BASIC]->notify();
}

static void updateDetailed(void)
{
    // uint8_t sendValue[1];

    // gHandle.pCharacteristic[BLE_CHARACTERISTIC_DETAILED]->setValue(sendValue, sizeof(sendValue));
    // gHandle.pCharacteristic[BLE_CHARACTERISTIC_DETAILED]->notify();
}

static void updateWrite(void)
{
    static Battery_OutputState_E lastStateOutputDC = (gHandle.pCharacteristic[BLE_CHARACTERISTIC_WRITE]->getValue()[0] == 0) ? BATTERY_OUTPUT_STATE_OFF : BATTERY_OUTPUT_STATE_ON;
    Battery_OutputState_E newStateOutputDC = (gHandle.pCharacteristic[BLE_CHARACTERISTIC_WRITE]->getValue()[0] == 0) ? BATTERY_OUTPUT_STATE_OFF : BATTERY_OUTPUT_STATE_ON;
    if (lastStateOutputDC != newStateOutputDC)
    {
        Battery_SetOutputDC(newStateOutputDC);
        lastStateOutputDC = newStateOutputDC;
    }

    static Battery_OutputState_E lastStateOutputAC = (gHandle.pCharacteristic[BLE_CHARACTERISTIC_WRITE]->getValue()[1] == 0) ? BATTERY_OUTPUT_STATE_OFF : BATTERY_OUTPUT_STATE_ON;
    Battery_OutputState_E newStateOutputAC = (gHandle.pCharacteristic[BLE_CHARACTERISTIC_WRITE]->getValue()[1] == 0) ? BATTERY_OUTPUT_STATE_OFF : BATTERY_OUTPUT_STATE_ON;
    if (lastStateOutputAC != newStateOutputAC)
    {
        Battery_SetOutputAC(newStateOutputAC);
        lastStateOutputAC = newStateOutputAC;
    }
}
