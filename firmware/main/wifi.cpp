#include "config.h"
#include "wifi.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <driver/gpio.h>

#include <esp_wifi.h>
#include <esp_dpp.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <qrcode.h>

#include "status_led.h"

static const char* TAG = "WiFi";

#define DPP_LISTEN_CHANNEL_LIST "6"

wifi_config_t wifi_config;
const char* qrcode_data;
EventGroupHandle_t event_group;
int retries = 0;

#define CONNECTED_BIT       BIT0
#define RETRY_TIMEOUT_BIT   BIT1
#define DPP_TX_FAILURE_BIT  BIT2

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        if (strlen((const char*)wifi_config.sta.ssid)) {
            ESP_LOGI(TAG, "Connecting to AP: %s", wifi_config.sta.ssid);
            esp_wifi_connect();

        } else {
            ESP_ERROR_CHECK(esp_supp_dpp_start_listen());
            status_led.dpp_authentication();
            ESP_LOGI(TAG, "Started listening for DPP Authentication");
            
            if (qrcode_data) {
                esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
                ESP_LOGI(TAG, "Scan below QR code:");
                esp_qrcode_generate(&cfg, qrcode_data);
            } else {
                ESP_LOGE(TAG, "DPP QRCode is not yet available?");
            }
        }

    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retries >= 0) {
            if (retries < CONFIG_WIFI_MAXRETRY) {
                retries++;
                ESP_LOGW(TAG, "Retrying connection to AP: %s", wifi_config.sta.ssid);
                status_led.wifi_retrying();
                esp_wifi_connect();

            } else {
                retries = 0;
                ESP_LOGE(TAG, "Failed to connect to AP: %s", wifi_config.sta.ssid);
                xEventGroupSetBits(event_group, RETRY_TIMEOUT_BIT);
            }

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

void dpp_enrollee_event_cb(esp_supp_dpp_event_t event, void* data) {
    switch (event) {
        case ESP_SUPP_DPP_URI_READY:
            ESP_LOGI(TAG, "DPP QRCode Ready");
            qrcode_data = (const char*)data;
            break;

        case ESP_SUPP_DPP_CFG_RECVD:
            memcpy(&wifi_config, data, sizeof(wifi_config));
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            esp_supp_dpp_stop_listen();
            ESP_LOGI(TAG, "DPP Authentication successful, connecting to AP: %s", wifi_config.sta.ssid);
            retries = 0;
            esp_wifi_connect();
            break;

        case ESP_SUPP_DPP_FAIL:
            ESP_LOGI(TAG, "DPP Authentication failed (Reason: %s), retry...", esp_err_to_name((int)data));
            if ((int)data == ESP_ERR_DPP_TX_FAILURE) {
                xEventGroupSetBits(event_group, DPP_TX_FAILURE_BIT);
            } else {
                ESP_ERROR_CHECK(esp_supp_dpp_start_listen());
            }
            break;
    }
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

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_supp_dpp_init(dpp_enrollee_event_cb));
    ESP_ERROR_CHECK(esp_supp_dpp_bootstrap_gen(
        DPP_LISTEN_CHANNEL_LIST,
        DPP_BOOTSTRAP_QR_CODE,
        "8a1d6a839ad69cf1ce04f30b7981c3b7cea87d918ae65c8fba1a624fb6d025c7",
        "smart-mailbox"
    ));

    gpio_reset_pin(GPIO_NUM_9);
    gpio_set_direction(GPIO_NUM_9, GPIO_MODE_INPUT);
}

void wifi_start(bool force_dpp) {
    ESP_LOGI(TAG, "WiFi starting");

    event_group = xEventGroupCreate();

    if (!gpio_get_level(GPIO_NUM_9)) {
        ESP_ERROR_CHECK(esp_wifi_restore());
        ESP_LOGW(TAG, "WiFi configuration manually reset");
    }

    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &wifi_config));
    if (force_dpp) {
        wifi_config.sta.ssid[0] = '\0';
    }

    retries = 0;
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(
        event_group,
        CONNECTED_BIT | RETRY_TIMEOUT_BIT | DPP_TX_FAILURE_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY
    );

    retries = -1; // connected

    vEventGroupDelete(event_group);

    if (bits & CONNECTED_BIT) {
        //ESP_LOGI(TAG, "Connected to AP: %s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
        ESP_LOGI(TAG, "Connected to AP: %s", wifi_config.sta.ssid);

    } else if (bits & RETRY_TIMEOUT_BIT) {
        ESP_LOGE(TAG, "Could not connect to AP, restarting in DPP Authentication mode...");
        wifi_stop();
        wifi_start(true);
        return;

    } else if (bits & DPP_TX_FAILURE_BIT) {
        ESP_LOGW(TAG, "DPP Authentication is broken due to TX failure, restarting...");
        wifi_stop();
        wifi_start(true);
        return;

    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    status_led.active();
}

void wifi_start() {
    wifi_start(false);
}

void wifi_stop() {
    retries = -2; // we're disconnecting
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_LOGI(TAG, "WiFi stopped");
}
