#include "config.h"
#include "wifi.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "status_led.h"

static const char* TAG = "WiFi";

wifi_config_t wifi_config;
EventGroupHandle_t event_group;
int retries = 0;

#define CONNECTED_BIT       BIT0

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Connecting to AP: %s", wifi_config.sta.ssid);
        esp_wifi_connect();

    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retries >= 0) {
            ESP_LOGW(TAG, "Retrying connection to AP: %s", wifi_config.sta.ssid);
            status_led.wifi_retrying();
            esp_wifi_connect();

        } else if (retries == -1) {
            ESP_LOGE(TAG, "Disconnected from AP: %s", wifi_config.sta.ssid);
            wifi_stop();
            wifi_start();
        } 
    }
}

void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    xEventGroupSetBits(event_group, CONNECTED_BIT);
}

void wifi_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL
    ));
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL
    ));

    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, CONFIG_WIFI_SSID, sizeof(CONFIG_WIFI_SSID));
    memcpy(wifi_config.sta.password, CONFIG_WIFI_PASSWORD, sizeof(CONFIG_WIFI_PASSWORD));
    wifi_config.sta.threshold.authmode = CONFIG_WIFI_SCAN_AUTH_MODE_THRESHOLD;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
}

void wifi_start() {
    ESP_LOGI(TAG, "WiFi starting");

    event_group = xEventGroupCreate();

    retries = 0;
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(
        event_group,
        CONNECTED_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY
    );

    retries = -1; // connected

    vEventGroupDelete(event_group);

    if (bits & CONNECTED_BIT) {
        //ESP_LOGI(TAG, "Connected to AP: %s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        ESP_LOGI(TAG, "Connected to AP: %s", wifi_config.sta.ssid);

    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    status_led.active();
}

void wifi_stop() {
    retries = -2; // we're disconnecting
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_LOGI(TAG, "WiFi stopped");
}
