#include <Arduino.h>

#include "MyCAN.h"
#include "Battery.h"
#include "MyBLE.h"
#include "Broker.h"
#include "driver/twai.h"

void setup()
{
    Serial.begin(115200);

    MyCAN_Initialize();
    Battery_Initialize();
    MyBLE_Initialize("KevinControl");
    Broker_Initialize();

    // setCpuFrequencyMhz(80);
    Serial.printf("INFO, %s, %i, System initialization successfull.\n", __FILE__, __LINE__);
}

void loop()
{
    static unsigned long canTimestamp = millis();
    static unsigned long batteryTimestamp = millis();
    static unsigned long brokerTimestamp = millis();

    if (2 <= (millis() - canTimestamp))
    {
        canTimestamp = millis();
        MyCAN_Update();
    }

    if (20 <= (millis() - batteryTimestamp))
    {
        batteryTimestamp = millis();
        Battery_Update();
    }

    if (1000 <= (millis() - brokerTimestamp))
    {
        brokerTimestamp = millis();
        Broker_Update();
    }
}
