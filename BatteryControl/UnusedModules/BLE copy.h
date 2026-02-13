// #pragma once

// #include <Arduino.h>

// typedef enum
// {
//     BLE_MESSAGE_BATTERY_SOC = 0u,
//     BLE_MESSAGE_BATTERY_12V_ENABLE,
//     BLE_MESSAGE_BATTERY_230V_ENABLE,

//     BLE_MESSAGE_BATTERY_UNDEFINED,
// } BLE_Message_Battery_E;

// class MyBleServer
// {
// public:
//     BLEService* createService(const char* uuid);	
//     BLEService* createService(BLEUUID uuid, uint32_t numHandles=15, uint8_t inst_id=0);
    
// private:
//     MyBleServer();
//     BLEServer *pServer;

// }; // MyBleServer

// class MyBleService
// {
// public:
// private:
//     MyBleService();

// }; // MyBleService

// extern void BLE_initialize(void);

// extern void BLE_update(void);

// extern uint32_t BLE_getMessageValue(BLE_Message_Battery_E message);