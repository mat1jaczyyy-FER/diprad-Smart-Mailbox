#include "https_request.h"

#include <string.h>

#include <esp_log.h>

#include <esp_tls.h>

//#define DEBUG_HTTPS_RESPONSE

static const char *TAG = "HTTPS-Request";

static const char* base_url = "https://%s/%s";

static const char* base_req =
    "GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: esp-idf/1.0 esp32 smart-mailbox/0.3.0\r\n"
    "\r\n";

int https_get_request(const char* server, const char* url, const uint8_t* cacert_start, const uint8_t* cacert_end) {
    esp_tls_cfg_t cfg = {};
    cfg.cacert_buf = (const unsigned char*)cacert_start;
    cfg.cacert_bytes = cacert_end - cacert_start;

    char full_url[strlen(base_url) + strlen(server) + strlen(url) + 1];
    snprintf(full_url, sizeof(full_url), base_url, server, url);

    struct esp_tls* tls = esp_tls_conn_http_new(full_url, &cfg);

    if (tls != NULL) {
        ESP_LOGI(TAG, "Connection established...");

    } else {
        ESP_LOGE(TAG, "Connection failed...");

        esp_tls_conn_delete(tls);
        return 0;
    }

    char req[strlen(base_req) + strlen(full_url) + strlen(server) + 1];
    snprintf(req, sizeof(req), base_req, full_url, server);

    size_t written_bytes = 0;
    do {
        int tls_ret = esp_tls_conn_write(tls, req + written_bytes, strlen(req) - written_bytes);

        if (tls_ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", tls_ret);
            written_bytes += tls_ret;

        } else if (tls_ret != ESP_TLS_ERR_SSL_WANT_READ  && tls_ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", tls_ret, esp_err_to_name(tls_ret));
            
            esp_tls_conn_delete(tls);
            return 0;
        }
    } while (written_bytes < strlen(req));

    ESP_LOGI(TAG, "Waiting for HTTPS response...");

    #ifdef DEBUG_HTTPS_RESPONSE

    do {
        char buf[512];
        bzero(buf, sizeof(buf));
        int ret = esp_tls_conn_read(tls, (char *)buf, sizeof(buf) - 1);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            continue;
        }

        if (ret < 0) {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        }

        if (ret == 0) {
            ESP_LOGI(TAG, "connection closed");
            break;
        }

        ESP_LOGD(TAG, "%d bytes read", ret);
        
        for (int i = 0; i < ret; i++) {
            putchar(buf[i]);
        }
        putchar('\n'); // JSON output doesn't have a newline at end
    } while (1);
    
    #else

    char buf;
    (void)esp_tls_conn_read(tls, &buf, 1);

    #endif

    esp_tls_conn_delete(tls);
    return 1;
}
