#include "config.h"
#include "api.h"

#include <string.h>

#include <esp_log.h>
#include <mbedtls/base64.h>

#include "https_request.h"
#include "sleep.h"
#include "status_led.h"

static const char* TAG = "API";

/* 
   openssl s_client -showcerts -connect server:443 </dev/null
   The CA root cert is the last cert given in the chain of certs.
*/
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

uint8_t* decode_base64_http_response(char* res) {
    if (!res) return NULL;

    if (!(strstr(res, "HTTP/1.0 200 OK") || strstr(res, "HTTP/1.1 200 OK"))) {
        ESP_LOGE(TAG, "decode_base64_http_response: HTTP status code is not 200 OK");
        return NULL;
    }
    
    if (!strstr(res, "Content-Type: application/octet-stream")) {
        ESP_LOGE(TAG, "decode_base64_http_response: Invalid or missing Content-Type");
        return NULL;
    }

    if (!strstr(res, "Content-Transfer-Encoding: base64")) {
        ESP_LOGE(TAG, "decode_base64_http_response: Invalid or missing Content-Transfer-Encoding");
        return NULL;
    }

    const char* base64 = strstr(res, "\r\n\r\n");
    if (!base64) {
        ESP_LOGE(TAG, "decode_base64_http_response: Failed to find the encoded data");
        return NULL;
    }
    base64 += 4;

    size_t decoded_len = 0;
    size_t encoded_len = strlen(base64);
    size_t max_decoded_len = (encoded_len * 3) / 4;

    uint8_t* decoded_data = (uint8_t*)malloc(sizeof(uint8_t) * max_decoded_len);
    if (decoded_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for decoded data");
        return NULL;
    }

    int ret = mbedtls_base64_decode(
        decoded_data, max_decoded_len, &decoded_len,
        (const uint8_t*)base64, encoded_len
    );
    if (ret != 0) {
        ESP_LOGE(TAG, "Base64 decode failed with error code: %d", ret);
        free(decoded_data);
        return NULL;
    }

    return decoded_data;
}

uint32_t* api_config() {
    char* res = NULL;

    while (1) {
        (void)https_post_request(
            CONFIG_API_SERVER,
            CONFIG_API_BASE_URL "config",
            "{\"private_code\":\"" CONFIG_MAILBOX_PRIVATE_CODE "\"}",
            &res,
            server_root_cert_pem_start,
            server_root_cert_pem_end
        );

        uint8_t* config = decode_base64_http_response(res);
        free(res);

        if (config != NULL) {
            status_led.active();
            ESP_LOGI(TAG, "Config received from server");
            return (uint32_t*)config;
        }

        status_led.api_retrying();
        ESP_LOGW(TAG, "Failed to get config from server, retrying in %d seconds", CONFIG_API_FAIL_INTERVAL);
        light_sleep(CONFIG_API_FAIL_INTERVAL * 1000);
    }
}

bool api_notify() {
    return https_post_request(
        CONFIG_API_SERVER,
        CONFIG_API_BASE_URL "notify",
        "{\"private_code\":\"" CONFIG_MAILBOX_PRIVATE_CODE "\"}",
        NULL,
        server_root_cert_pem_start,
        server_root_cert_pem_end
    );
}

bool api_battery(bool battery_low) {
    char json_payload[40];
    snprintf(
        json_payload,
        sizeof(json_payload),
        "{\"private_code\":\"" CONFIG_MAILBOX_PRIVATE_CODE "\", \"low\":%s}",
        battery_low? "true" : "false"
    );

    return https_post_request(
        CONFIG_API_SERVER,
        CONFIG_API_BASE_URL "battery",
        json_payload,
        NULL,
        server_root_cert_pem_start,
        server_root_cert_pem_end
    );
}
