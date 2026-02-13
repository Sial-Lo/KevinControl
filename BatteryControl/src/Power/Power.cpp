#include "Power.h"
#include "MyBLEServer.h"
#include "esp_pm.h"
#include "esp32/clk.h"
#include "driver/twai.h"

static void enterDeepSleep(void);

uint64_t gTimestamp;

void Power_Initialize(void)
{
    gTimestamp = millis();
    Serial.printf("INFO, %s, %i, Startup Timestamp: %i.\n", __FILE__, __LINE__, gTimestamp);
}

void Power_Update(void)
{
    if (true == MyBLEServer_Connected())
    {
        gTimestamp = millis();
    }
    else
    {
        if (1000 <= (millis() - gTimestamp))
        {
            enterDeepSleep();
            gTimestamp = millis();
        }
    }
}

static void enterDeepSleep(void)
{
    // Serial.printf("INFO, %s, %i, Enter Deep Sleep.\n", __FILE__, __LINE__);
    // esp_sleep_enable_timer_wakeup(3 * 1000 * 1000);

    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF);

    // esp_deep_sleep_start();

    twai_stop();
    Serial.printf("INFO, %s, %i, Started Sleep at %i.\n", __FILE__, __LINE__, millis());
    esp_sleep_enable_timer_wakeup(1000 * 1000);
    esp_light_sleep_start();
    Serial.printf("INFO, %s, %i, Stop Sleep at %i.\n", __FILE__, __LINE__, millis());

    twai_clear_receive_queue();
    twai_clear_transmit_queue();
    twai_start();
}