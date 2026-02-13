#include <Arduino.h>

#include "MyCAN.h"
#include "Battery.h"
#include "MyBLEServer.h"
#include "BLEBattery.h"
#include "Power.h"
#include "driver/twai.h"

void setup()
{
    Serial.begin(115200);

    MyCAN_Initialize();
    Battery_Initialize();
    MyBLEServer_Initialize("KevinControl");
    BLEBattery_Initialize();
    // Power_Initialize();

    setCpuFrequencyMhz(80);
    Serial.printf("INFO, %s, %i, System initialization successfull.\n", __FILE__, __LINE__);
}

void loop()
{
    static unsigned long powerTimestamp = millis();
    static unsigned long canTimestamp = millis();
    static unsigned long batteryTimestamp = millis();

    if (10 <= (millis() - powerTimestamp))
    {
        // Power_Update();
    }

    // if (true == MyBLEServer_Connected())
    // {
        if (10 <= (millis() - canTimestamp))
        {
            canTimestamp = millis();
            MyCAN_Update();
        }

        if (1000 <= (millis() - batteryTimestamp))
        {
            batteryTimestamp = millis();
            BLEBattery_Update();
        }
    // }
}
