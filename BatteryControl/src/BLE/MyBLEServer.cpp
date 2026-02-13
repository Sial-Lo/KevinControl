#include "MyBLEServer.h"
#include <BLEDevice.h>
#include <BLE2902.h>

static struct
{
    BLEServer *pServer = NULL;
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

void MyBLEServer_Initialize(const char *name)
{
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    BLEDevice::init(name);
    Handle.pServer = BLEDevice::createServer();
    Handle.pServer->setCallbacks(new MyServerCallbacks);
    Serial.printf("INFO, %s, %i, Initialization successful.\n", __FILE__, __LINE__);
}

BLEServer *MyBLEServer_GetServer()
{
    return Handle.pServer;
}

bool MyBLEServer_Connected()
{
    return Handle.connected;
}

void MyBLEServer_StartService(BLEService *pService)
{
    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->setMinInterval(0xA0);
    pAdvertising->setMaxInterval(0x1E0);
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.printf("INFO, %s, %i, Advertising started.\n", __FILE__, __LINE__);
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