#include "co2.h"
#include "esp_log.h"
#include "core2forAWS.h"

static const char* TAG = "CO2";

static SemaphoreHandle_t envSemaphore = NULL;

mhz19_config_t mhz19_cfg[HW_MHZ19_DEVICES_MAX];

void start_mhz19(void) {
    ESP_LOGI(TAG, "Start CO2 Sensor MHZ19C");
#ifdef CONFIG_OAP_MH_ENABLED
	if (mhz19_set_hardware_config(&mhz19_cfg[0], 2) == ESP_OK) {
		mhz19_cfg[0].interval = 5000;
		mhz19_init(&mhz19_cfg[0]);
		mhz19_enable(&mhz19_cfg[0], true);
	}
#endif

}