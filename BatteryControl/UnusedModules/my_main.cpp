// /*
//     Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
//     Ported to Arduino ESP32 by Evandro Copercini
//     updates by chegewara
// */

// #include <Arduino.h>
// #include <string.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>

// // See the following for generating UUIDs:
// // https://www.uuidgenerator.net/

// #define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
// #define CHARACTERISTIC_UUID1 "f0c7a700-149d-42a2-bf4f-9ed260b0490e"

// BLECharacteristic *pCharacteristic = NULL;
// BLECharacteristic *pCharacteristic1 = NULL;

// void setup()
// {
//     Serial.begin(115200);
//     Serial.println("Starting BLE work!");

//     pinMode(2, OUTPUT);

//     BLEDevice::init("Long name works now");
//     BLEServer *pServer = BLEDevice::createServer();
//     BLEService *pService = pServer->createService(SERVICE_UUID);
//     pCharacteristic =
//         pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
//     pCharacteristic1 =
//         pService->createCharacteristic(CHARACTERISTIC_UUID1, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

//     pCharacteristic->setValue("1");
//     pCharacteristic1->setValue("0");
//     pService->start();
//     // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
//     BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
//     pAdvertising->addServiceUUID(SERVICE_UUID);
//     pAdvertising->setScanResponse(true);
//     pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
//     pAdvertising->setMaxPreferred(0x12);
//     BLEDevice::startAdvertising();
//     Serial.println("Characteristic defined! Now you can read it in your phone!");
// }

// void loop()
// {
//     if (Serial.available())
//     {
//         String str = Serial.readStringUntil('\n');
//         str.trim();
//         pCharacteristic->setValue(str.c_str());
//         Serial.print("Set new value: ");
//         Serial.println(str);
//     }
//     // static unsigned long timestamp1000 = millis();

//     // // Serial.println(pCharacteristic->getValue().c_str());
//     // // Serial.println(pCharacteristic1->getValue().c_str());
//     // if (pCharacteristic->getValue() == "1")
//     // {
//     //     digitalWrite(2, 1);
//     // }
//     // else
//     // {
//     //     digitalWrite(2, 0);
//     // }
//     // // put your main code here, to run repeatedly:
//     // // delay(2000);

//     // if (1000 <= (millis() - timestamp1000))
//     // {
//     //     timestamp1000 = millis();
//     //     std::string value = std::to_string(atoi(pCharacteristic1->getValue().c_str()) + 1);
//     //     pCharacteristic1->setValue(value);
//     // }
// }
