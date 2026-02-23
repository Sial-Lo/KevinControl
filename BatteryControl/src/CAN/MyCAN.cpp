#include "MyCAN.h"
#include "driver/twai.h"

#define RX_PIN 21
#define TX_PIN 22

#define TRANSMIT_RATE_MS 1000
#define POLLING_RATE_MS 1000

#define MAX_MESSAGES 32

typedef struct MyCAN_Message_T
{
    uint32_t identifier;
    uint32_t mask;
    MyCan_Callback callback;
} MyCAN_Message_T;

static void handle_rx_messages(twai_message_t &message);

static struct
{
    bool initialized = false;
    MyCAN_Message_T messages[MAX_MESSAGES] = {0, 0, NULL};
    bool enableRequests = false;
} Handle;

bool MyCAN_Initialize(void)
{
    // Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TX_PIN, (gpio_num_t)RX_PIN, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS(); // Look in the api-reference for other speed sets.
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Install TWAI driver
    if (ESP_OK != twai_driver_install(&g_config, &t_config, &f_config))
    {
        Serial.printf("ERROR, %s, %i, Failed to install driver.\n", __FILE__, __LINE__);
        return 0;
    }

    // Start TWAI driver
    if (ESP_OK != twai_start())
    {
        Serial.printf("ERROR, %s, %i, Failed to start driver.\n", __FILE__, __LINE__);
        return 0;
    }

    // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
    uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
    if (ESP_OK != twai_reconfigure_alerts(alerts_to_enable, NULL))
    {
        Serial.printf("ERROR, %s, %i, Failed to reconfigure alerts.\n", __FILE__, __LINE__);
        return 0;
    }

    Serial.printf("INFO, %s, %i, Twai initialized.\n", __FILE__, __LINE__);

    // TWAI driver is now successfully installed and started
    Handle.initialized = true;
    return 1;
}

void MyCAN_Update(void)
{
    twai_status_info_t twaistatus;
    twai_get_status_info(&twaistatus);

    if (twaistatus.state != TWAI_STATE_RUNNING)
    {
    }
    else
    {
        // Check if alert happened
        uint32_t alerts_triggered;
        twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));

        // Handle alerts
        if (alerts_triggered & TWAI_ALERT_ERR_PASS)
        {
            Serial.printf("ALERT, %s, %i, TWAI controller has become error passive.\n", __FILE__, __LINE__);
        }
        if (alerts_triggered & TWAI_ALERT_BUS_ERROR)
        {
            Serial.printf("ALERT, %s, %i, A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.\n", __FILE__, __LINE__);
            Serial.printf("Bus error count: %lu\n", twaistatus.bus_error_count);
        }
        if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL)
        {
            Serial.printf("ALERT, %s, %i, The RX queue is full causing a received frame to be lost.\n", __FILE__, __LINE__);
            Serial.printf("RX buffered: %lu\t", twaistatus.msgs_to_rx);
            Serial.printf("RX missed: %lu\t", twaistatus.rx_missed_count);
            Serial.printf("RX overrun %lu\n", twaistatus.rx_overrun_count);
        }

        // Receive messages.
        if (alerts_triggered & TWAI_ALERT_RX_DATA)
        {
            twai_message_t message;
            while (ESP_OK == twai_receive(&message, 0))
            {
                handle_rx_messages(message);
            }
        }
    }
}

bool MyCAN_RegisterMessage(uint32_t messageId, uint32_t mask, MyCan_Callback callback)
{
    static uint8_t messageCounter = 0u;
    bool retVal = true;

    if (NULL == callback)
    {
        Serial.printf("ERROR, %s, %i, The parameter callback is a NULL pointer.\n", __FILE__, __LINE__);
        retVal = false;
    }
    else if (messageCounter >= MAX_MESSAGES)
    {
        Serial.printf("ERROR, %s, %i, The module can't hold more messages. Change the value of MAX_MESSAGES.\n", __FILE__, __LINE__);
        retVal = false;
    }
    else
    {
        Handle.messages[messageCounter].identifier = messageId;
        Handle.messages[messageCounter].mask = mask;
        Handle.messages[messageCounter].callback = callback;
        messageCounter++;
    }

    return retVal;
}

void MyCAN_SendMessage(uint32_t identifier, uint8_t length, uint8_t data[8])
{
    if (data == NULL)
    {
        Serial.printf("ERROR, %s, %i, data[] is a NULL pointer.\n", __FILE__, __LINE__);
    }
    else if (length > TWAI_FRAME_MAX_DLC)
    {
        Serial.printf("ERROR, %s, %i, a CAN frame can only hold 8 bytes. Requested %i\n", __FILE__, __LINE__, length);
    }
    else
    {
        twai_message_t message;
        message.extd = 1;
        message.rtr = 0;
        message.ss = 0;
        message.self = 0;
        message.dlc_non_comp = 0;

        message.identifier = identifier;
        message.data_length_code = length;
        for (uint8_t i = 0u; i < length; i++)
        {
            message.data[i] = data[i];
        }

        if (ESP_OK != twai_transmit(&message, pdMS_TO_TICKS(1000)))
        {
            Serial.printf("ERROR, %s, %i, Failed to queue message for transmission.\n", __FILE__, __LINE__);
        }
    }
}

void MyCAN_Request(uint32_t canId, uint8_t id)
{
    twai_message_t message;
    message.extd = 1;
    message.rtr = 0;
    message.ss = 0;
    message.self = 0;
    message.dlc_non_comp = 0;

    message.identifier = canId;
    message.data_length_code = 8;

    message.data[0] = 0x00;
    message.data[1] = 0x40;
    message.data[2] = 0x00;
    message.data[3] = id;
    message.data[4] = 0x00;
    message.data[5] = 0x00;
    message.data[6] = 0x00;
    message.data[7] = 0x00;

    if (ESP_OK != twai_transmit(&message, pdMS_TO_TICKS(1000)))
    {
        Serial.printf("ERROR, %s, %i, Failed to queue message for transmission.\n", __FILE__, __LINE__);
    }
}

void MyCAN_ClearAlerts(void)
{
    if (ESP_OK != twai_initiate_recovery())
    {
        Serial.printf("ERROR, %s, %i, Failed to initiate recovery.\n", __FILE__, __LINE__);
    }

    if (ESP_OK != twai_start())
    {
        Serial.printf("ERROR, %s, %i, Failed to start driver.\n", __FILE__, __LINE__);
    }
}

static void handle_rx_messages(twai_message_t &message)
{
    if (message.extd)
    {
        if (!(message.rtr))
        {
            for (uint8_t messageCount = 0; messageCount < MAX_MESSAGES; messageCount++)
            {
                if (NULL != Handle.messages[messageCount].callback)
                {
                    if ((message.identifier & Handle.messages[messageCount].mask) == (Handle.messages[messageCount].identifier & Handle.messages[messageCount].mask))
                    {
                        Handle.messages[messageCount].callback(message.identifier, message.data);
                    }
                }
            }
        }
    }
    else
    {
        Serial.printf("ERROR, %s, %i, Message is in Standard Format.\n", __FILE__, __LINE__);
    }
}