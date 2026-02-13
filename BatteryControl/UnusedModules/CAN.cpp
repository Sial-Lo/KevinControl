#include "CAN.h"
#include "DataHandler.h"
#include "driver/twai.h"

#define RX_PIN 21
#define TX_PIN 22

#define TRANSMIT_RATE_MS 1000
#define POLLING_RATE_MS 1000

typedef void (*fctptrSetValue)(uint32_t value);

typedef struct CAN_Data_T
{
    uint32_t id;
    fctptrSetValue setFunction;
} CAN_Data_T;

static void update12vState(void);
static void handle_rx_message(twai_message_t &message);
static void set12vState(uint8_t state);

unsigned long previousMillis = 0; // will store last time a message was send
static bool driver_installed = false;

void CAN_initialize(void)
{
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TX_PIN, (gpio_num_t)RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS(); // Look in the api-reference for other speed sets.
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        Serial.println("Driver installed");
    }
    else
    {
        Serial.println("Failed to install driver");
        return;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK)
    {
        Serial.println("Driver started");
    }
    else
    {
        Serial.println("Failed to start driver");
        return;
    }

    // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
    uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
    {
        Serial.println("CAN Alerts reconfigured");
    }
    else
    {
        Serial.println("Failed to reconfigure alerts");
        return;
    }

    // TWAI driver is now successfully installed and started
    driver_installed = true;
}

void CAN_update(void)
{
    if (!driver_installed)
    {
    }
    else
    {
        // Check if alert happened
        uint32_t alerts_triggered;
        twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));
        twai_status_info_t twaistatus;
        twai_get_status_info(&twaistatus);

        // Handle alerts
        if (alerts_triggered & TWAI_ALERT_ERR_PASS)
        {
            Serial.println("Alert: TWAI controller has become error passive.");
        }
        if (alerts_triggered & TWAI_ALERT_BUS_ERROR)
        {
            Serial.println("Alert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
            Serial.printf("Bus error count: %lu\n", twaistatus.bus_error_count);
        }
        if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL)
        {
            Serial.println("Alert: The RX queue is full causing a received frame to be lost.");
            Serial.printf("RX buffered: %lu\t", twaistatus.msgs_to_rx);
            Serial.printf("RX missed: %lu\t", twaistatus.rx_missed_count);
            Serial.printf("RX overrun %lu\n", twaistatus.rx_overrun_count);
        }

        // Check if message is received
        if (alerts_triggered & TWAI_ALERT_RX_DATA)
        {
            // One or more messages received. Handle all.
            twai_message_t message;
            while (twai_receive(&message, 0) == ESP_OK)
            {
                handle_rx_message(message);
            }
        }
    }

    update12vState();
}

static void update12vState(void)
{
    static uint8_t oldState = DataHandler_get()->Battery.voltage_12V;

    if (oldState != DataHandler_get()->Battery.voltage_12V)
    {
        oldState = DataHandler_get()->Battery.voltage_12V;
        set12vState(oldState);
    }
}

static void handle_rx_message(twai_message_t &message)
{
    // Process received message
    if (message.extd)
    {
        if (!(message.rtr))
        {
            if (message.identifier == 0x18FF0003)
            {
                uint16_t soc = (message.data[0] << 8) | message.data[1];
                soc = map(soc, 0, 0xFFFF, 0, 100);
                DataHandler_get()->Battery.soc = (uint8_t)soc;
            }
        }
    }
    else
    {
        Serial.println("Message is in Standard Format");
    }
    // Serial.printf("ID: %lx\nByte:", message.identifier);
    // if (!(message.rtr))
    // {
    //     for (int i = 0; i < message.data_length_code; i++)
    //     {
    //         Serial.printf(" %d = %02x,", i, message.data[i]);
    //     }
    //     Serial.println("");
    // }
}

static void set12vState(uint8_t state)
{
    // Configure message to transmit
    twai_message_t message;
    message.identifier = 0x19EF0301;
    message.data_length_code = 8;
    message.data[0] = 0x00;
    message.data[1] = 0x40;
    message.data[2] = 0x00;
    message.data[3] = 0x7F;
    message.data[4] = 0x00;
    message.data[5] = 0x00;
    message.data[6] = 0x00;
    message.data[7] = state;

    // Queue message for transmission
    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK)
    {
        printf("CAN: Set 12 V state to %i\n", state);
    }
    else
    {
        printf("Failed to queue message for transmission\n");
    }
}