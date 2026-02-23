#include "Battery.h"
#include "MyCAN.h"
#include "driver/twai.h"

typedef struct
{
    uint8_t identifier;
    uint32_t value;
} Requests_T;

static void canRequestCallback(uint32_t identifier, uint8_t data[8]);
static void canStatusCallback(uint32_t identifier, uint8_t data[8]);

static struct
{
    uint8_t batteryId = 0xFFu;
    bool connected = false;
    uint64_t firstMessageTimestamp = 0u;
    uint64_t lastMessageTimestamp = 0u;
    bool wakeupActive = false;
} Handle;

static Requests_T gRequests[] = {
    [REQUEST_BATTERY_VOLTAGE] = {.identifier = 100, .value = 0},
    [REQUEST_AC_INPUT_POWER] = {.identifier = 104, .value = 0},
    [REQUEST_AC_OUTPUT_POWER] = {.identifier = 107, .value = 0},
    [REQUEST_DC_INPUT_POWER] = {.identifier = 110, .value = 0},
    [REQUEST_DC_OUTPUT_POWER] = {.identifier = 113, .value = 0},
    [REQUEST_SOC] = {.identifier = 119, .value = 0},
    [REQUEST_CELL_TEMPERATURE] = {.identifier = 121, .value = 0},
    [REQUEST_BATTERY_POWER] = {.identifier = 127, .value = 0},
    [REQUEST_AC_INPUT_OPERATING_STATE] = {.identifier = 200, .value = 0},
    [REQUEST_AC_OUTPUT_OPERATING_STATE] = {.identifier = 201, .value = 0},
    [REQUEST_DC_INPUT_OPERATING_STATE] = {.identifier = 202, .value = 0},
    [REQUEST_DC_OUTPUT_OPERATING_STATE] = {.identifier = 203, .value = 0},
};

void Battery_Initialize(void)
{
    MyCAN_RegisterMessage(0x18FF0000u, 0xFFFF0000u, canStatusCallback); // Status Frame
    MyCAN_RegisterMessage(0x19EF0103u, 0xFFFF0000u, canRequestCallback);

    pinMode(23, OUTPUT);    //!< Set pin 23 as output for the dc charging.
    digitalWrite(23, HIGH); //!< Set pin 23 to HIGH to disable charging by default.
}

void Battery_Update(void)
{
    if (Handle.connected)
    {
        if (true == Handle.wakeupActive)
        {
            // Handle.wakeupActive = false;
            // digitalWrite(23, HIGH); //!< Set pin 23 to HIGH to disable wakeup signal.
        }

        if (1000 < (millis() - Handle.lastMessageTimestamp))
        {
            Handle.connected = false;
            Handle.firstMessageTimestamp = 0u;
            Serial.printf("INFO, %s, %i, Battery disconnected.\n", __FILE__, __LINE__);
        }

        static uint8_t requestIndex = 0u;
        MyCAN_Request(0x19EF0001u | (Handle.batteryId << 8), gRequests[requestIndex].identifier);
        requestIndex++;
        if (requestIndex >= sizeof(gRequests) / sizeof(gRequests[0]))
        {
            requestIndex = 0u;
        }
    }
}

bool Battery_IsConnected(void)
{
    return Handle.connected;
}

uint64_t Battery_IsConnectedTimestamp(void)
{
    return Handle.firstMessageTimestamp;
}

uint32_t Battery_GetData(Requests_E request)
{
    return gRequests[request].value;
}

void Battery_setDcInput(Battery_OutputState_E state)
{
    if (state == BATTERY_OUTPUT_STATE_ON)
    {
        digitalWrite(23, LOW);
    }
    else
    {
        digitalWrite(23, HIGH);
    }
}

void Battery_SetDcOutput(Battery_OutputState_E state)
{
    if (0xFF == Handle.batteryId)
    {
        Serial.printf("ERROR, %s, %i, The device id is not set. Can't send messages.\n", __FILE__, __LINE__);
    }
    else
    {
        Serial.printf("INFO, %s, %i, Setting DC output state to %i.\n", __FILE__, __LINE__, state);

        uint32_t id = 0x19EF00FFu | (Handle.batteryId << 8);
        uint8_t data[8];
        data[0] = 0x00;
        data[1] = 0x41;
        data[2] = 0x00;
        data[3] = 0x9F;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = (uint8_t)state;

        MyCAN_SendMessage(id, sizeof(data), data);
    }
}

void Battery_SetAcOutput(Battery_OutputState_E state)
{
    if (0xFF == Handle.batteryId)
    {
        Serial.printf("ERROR, %s, %i, The device id is not set. Can't send messages.\n", __FILE__, __LINE__);
    }
    else
    {
        Serial.printf("INFO, %s, %i, Setting DC output state to %i.\n", __FILE__, __LINE__, state);

        uint32_t id = 0x19EF00FFu | (Handle.batteryId << 8);
        uint8_t data[8];
        data[0] = 0x00;
        data[1] = 0x41;
        data[2] = 0x00;
        data[3] = 0x9E;
        data[4] = 0x00;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = (uint8_t)state;

        MyCAN_SendMessage(id, sizeof(data), data);
    }
}

void Battery_Wakeup(void)
{
    // digitalWrite(23, LOW);
    // Handle.wakeupActive = true;
    // Serial.printf("INFO, %s, %i, Generate Wakeup.\n", __FILE__, __LINE__);
}

static void canStatusCallback(uint32_t identifier, uint8_t data[8])
{
    Handle.lastMessageTimestamp = millis();

    if (Handle.connected == false)
    {
        Handle.connected = true;
        Handle.firstMessageTimestamp = millis();
        Serial.printf("INFO, %s, %i, Battery connected.\n", __FILE__, __LINE__);
    }

    // Update the device ID since the battery sometimes changes it. The device ID is the second last byte of the identifier.
    Handle.batteryId = (uint8_t)(identifier & 0xFFu);
}

static void canRequestCallback(uint32_t identifier, uint8_t data[8])
{
    for (uint8_t i = 0u; i < (sizeof(gRequests) / sizeof(gRequests[0])); i++)
    {
        if (data[3] == gRequests[i].identifier)
        {
            gRequests[i].value = (uint32_t)(data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
            break;
        }
    }
}