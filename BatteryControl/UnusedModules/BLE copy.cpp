#include "BLE.h"
#include "DataHandler.h"
#include <BLEDevice.h>
#include <BLE2902.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

static BLEUUID BLESERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_BATTERY_NOTIFY "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_BATTERY_SOC "d7be7b90-2423-4d6e-926d-239bc96bb2fd"
#define CHARACTERISTIC_UUID_BATTERY_12V "aa6316cb-878e-4572-a94e-fec129349f85"
#define CHARACTERISTIC_UUID_BATTERY_230V "5be76c28-edb4-4112-8c0a-9c4a75215842"

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        Serial.println("Client connected");
        pServer->getAdvertising()->stop();
    };

    void onDisconnect(BLEServer *pServer)
    {
        Serial.println("Client disconnected");
        pServer->getAdvertising()->start();
    }
};

static uint8_t updateBatterySoc(void);
static uint8_t updateBatteryState12v(void);
static uint8_t updateBatteryState230v(void);

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristicBatteryNotify = NULL;
BLECharacteristic *pCharacteristicBatterySoc = NULL;
BLECharacteristic *pCharacteristicBattery12V = NULL;
BLECharacteristic *pCharacteristicBattery230V = NULL;
BLEDescriptor *pDescr;

void BLE_initialize(void)
{
    // Create the BLE Device
    BLEDevice::init("Kevin Control");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pBatteryService = pServer->createService(BLESERVICE_UUID, 10, 0); // the 30 in this line defines the number of chars to be used, increase as needed

    // Create a BLE Characteristic
    pCharacteristicBatteryNotify = pBatteryService->createCharacteristic(CHARACTERISTIC_UUID_BATTERY_NOTIFY, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
    pCharacteristicBatterySoc = pBatteryService->createCharacteristic(CHARACTERISTIC_UUID_BATTERY_SOC, BLECharacteristic::PROPERTY_READ);
    pCharacteristicBattery12V = pBatteryService->createCharacteristic(CHARACTERISTIC_UUID_BATTERY_12V, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristicBattery230V = pBatteryService->createCharacteristic(CHARACTERISTIC_UUID_BATTERY_230V, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    // // Create a BLE Descriptor
    // std::__cxx11::string descriptorValue_1 = "Notfication characteristic of the battery service.";
    // pDescr = new BLEDescriptor((uint16_t)sizeof(descriptorValue_1));
    // pDescr->setValue(descriptorValue_1);
    // pCharacteristicBatteryNotify->addDescriptor(pDescr);

    // pDescr = new BLEDescriptor((uint16_t)0x2901);
    // pDescr->setValue("Soc of the battery.");
    // pCharacteristicBatterySoc->addDescriptor(pDescr);

    uint8_t trueValue = 1;
    uint8_t falseValue = 0;
    pCharacteristicBatteryNotify->setValue(&falseValue, sizeof(falseValue));
    pCharacteristicBatterySoc->setValue(&falseValue, sizeof(falseValue));
    pCharacteristicBattery12V->setValue(&falseValue, sizeof(falseValue));
    pCharacteristicBattery230V->setValue(&falseValue, sizeof(falseValue));

    // Start the service
    pBatteryService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Initialization complete. Advertising started...");

    pinMode(2, OUTPUT);
}

extern void BLE_update(void)
{
    uint8_t notificationValue = 0u;

    notificationValue |= updateBatterySoc();
    notificationValue |= updateBatteryState12v();

    if (notificationValue > 0)
    {
        pCharacteristicBatteryNotify->setValue(&notificationValue, sizeof(notificationValue));
        pCharacteristicBatteryNotify->notify();
    }

    /* Mock SOC */
    if (Serial.available())
    {
        String str = Serial.readStringUntil('\n');
        str.trim();
        uint8_t soc = str.toInt();
        pCharacteristicBatterySoc->setValue(&soc, sizeof(soc));

        uint8_t charValue = 1;
        pCharacteristicBatteryNotify->setValue(&charValue, sizeof(charValue));
        pCharacteristicBatteryNotify->notify();

        Serial.print("Set new value: ");
        Serial.println(soc);
    }

    /* Mock 12V state */
    if (*pCharacteristicBattery12V->getData() == 0)
    {
        digitalWrite(2, 0);
    }
    else
    {
        digitalWrite(2, 1);
    }
}

static uint8_t updateBatterySoc(void)
{
    static uint8_t oldSoc = DataHandler_get()->Battery.soc;
    uint8_t retVal = 0;

    if (DataHandler_get()->Battery.soc != oldSoc)
    {
        oldSoc = DataHandler_get()->Battery.soc;
        pCharacteristicBatterySoc->setValue(&oldSoc, sizeof(oldSoc));

        retVal = 0b00000001;
    }

    return retVal;
}

static uint8_t updateBatteryState12v(void)
{
    static uint8_t oldState = DataHandler_get()->Battery.voltage_12V;
    uint8_t retVal = 0;

    if (DataHandler_get()->Battery.voltage_12V != oldState)
    {
        oldState = DataHandler_get()->Battery.voltage_12V;
        pCharacteristicBattery12V->setValue(&oldState, sizeof(oldState));

        retVal = 0b00000010;
    }
    else if (DataHandler_get()->Battery.voltage_12V != *pCharacteristicBattery12V->getData())
    {
        DataHandler_get()->Battery.voltage_12V = *pCharacteristicBattery12V->getData();
        Serial.printf("BLE Updated 12V State to: %i\n", DataHandler_get()->Battery.voltage_12V);
    }

    return retVal;
}

static uint8_t updateBatteryState230v(void)
{
    static uint8_t oldState = DataHandler_get()->Battery.voltage_230V;
    uint8_t retVal = 0;

    if (DataHandler_get()->Battery.voltage_230V != oldState)
    {
        oldState = DataHandler_get()->Battery.voltage_230V;
        pCharacteristicBattery230V->setValue(&oldState, sizeof(oldState));

        retVal = 0b00000100;
    }
    else if (DataHandler_get()->Battery.voltage_230V != *pCharacteristicBattery230V->getData())
    {
        DataHandler_get()->Battery.voltage_230V = *pCharacteristicBattery230V->getData();
    }

    return retVal;
}