#include "MyBLE.h"
#include <BLEDevice.h>
#include <BLE2902.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
static BLEUUID BLESERVICE_UUID(SERVICE_UUID);
#define CHARACTERISTIC_TRANSMIT_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_RECEIVE_UUID "d7be7b90-2423-4d6e-926d-239bc96bb2fd"

static struct
{
    BLEServer *pServer = NULL;
    BLEService *pService = NULL;
    BLEAdvertising *pAdvertising = NULL;
    BLECharacteristic *pCharacteristicTransmit = NULL;
    BLECharacteristic *pCharacteristicReceive = NULL;
    bool connected = false;
} Handle;

static void OnConnect(BLEServer *pServer);
static void OnDisconnect(BLEServer *pServer);

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        OnConnect(pServer);
    };

    void onDisconnect(BLEServer *pServer)
    {
        OnDisconnect(pServer);
    }
};

void MyBLE_Initialize(const char *name)
{
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    BLEDevice::init(name);
    Handle.pServer = BLEDevice::createServer();
    Handle.pServer->setCallbacks(new MyServerCallbacks);

    Handle.pService = Handle.pServer->createService(BLESERVICE_UUID, 10, 0);

    Handle.pAdvertising = BLEDevice::getAdvertising();
    Handle.pAdvertising->setMinInterval(0xA0);
    Handle.pAdvertising->setMaxInterval(0x1E0);
    Handle.pAdvertising->addServiceUUID(Handle.pService->getUUID());
    Handle.pAdvertising->setScanResponse(true);
    Handle.pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    Handle.pAdvertising->setMinPreferred(0x12);

    Handle.pCharacteristicTransmit = Handle.pService->createCharacteristic(CHARACTERISTIC_TRANSMIT_UUID, (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ));
    Handle.pCharacteristicReceive = Handle.pService->createCharacteristic(CHARACTERISTIC_RECEIVE_UUID, (BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE));

    Serial.printf("INFO, %s, %i, Initialization successful.\n", __FILE__, __LINE__);
}

bool MyBLE_DeviceConnected()
{
    return Handle.connected;
}

void MyBLE_Start(void)
{
    // Start the service
    Handle.pService->start();

    // Start advertising
    BLEDevice::startAdvertising();

    Serial.printf("INFO, %s, %i, BLE started.\n", __FILE__, __LINE__);
}

void MyBLE_Stop(void)
{
    // Stop advertising
    BLEDevice::stopAdvertising();

    // Stop the service
    Handle.pService->stop();

    Serial.printf("INFO, %s, %i, BLE stopped.\n", __FILE__, __LINE__);
}

void MyBLE_Transmit(MyBLE_TransmitData_T *transmitData)
{
    Handle.pCharacteristicTransmit->setValue((uint8_t *)transmitData, sizeof(MyBLE_TransmitData_T));
    Handle.pCharacteristicTransmit->notify();
}

void MyBLE_InitializeReceive(MyBLE_ReceiveData_T *receiveData)
{
    Handle.pCharacteristicReceive->setValue((uint8_t *)receiveData, sizeof(MyBLE_ReceiveData_T));
}

MyBLE_ReceiveData_T MyBLE_Receive(void)
{
    MyBLE_ReceiveData_T retVal = {0};
    uint8_t *data = Handle.pCharacteristicReceive->getData();
    if (data != NULL)
    {
        memcpy(&retVal, data, sizeof(retVal));
    }
    return retVal;
}

static void OnConnect(BLEServer *pServer)
{
    Handle.connected = true;
    pServer->getAdvertising()->stop();
    Serial.printf("INFO, %s, %i, Connected.\n", __FILE__, __LINE__);
}

static void OnDisconnect(BLEServer *pServer)
{
    Handle.connected = false;
    pServer->getAdvertising()->start();
    Serial.printf("INFO, %s, %i, Disconnected.\n", __FILE__, __LINE__);
}