#include "https_request.h"

#include <string.h>

#include <esp_log.h>

#include <esp_tls.h>

static const char *TAG = "HTTPS-Request";

static const char* base_url = "https://%s/%s";

static const char* base_get_req =
    "GET %s HTTP/1.0\r\n"
    "Host: %s\r\n"
    "User-Agent: esp-idf/1.0 esp32 smart-mailbox/1.0.0\r\n"
    "\r\n";

static const char* base_post_req =
    "POST %s HTTP/1.0\r\n"
    "Host: %s\r\n"
    "User-Agent: esp-idf/1.0 esp32 smart-mailbox/1.0.0\r\n"
    "Accept: */*\r\n"
    "Cache-Control: no-cache\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %d\r\n"
    "\r\n"
    "%s"
    "\r\n";

bool https_request(const char* full_url, const char* req, const int req_len, char** res, const uint8_t* cacert_start, const uint8_t* cacert_end) {
    esp_tls_cfg_t cfg = {};
    cfg.cacert_buf = (const unsigned char*)cacert_start;
    cfg.cacert_bytes = cacert_end - cacert_start;

    struct esp_tls* tls = esp_tls_conn_http_new(full_url, &cfg);

    if (tls != NULL) {
        ESP_LOGI(TAG, "Connection established...");

    } else {
        ESP_LOGE(TAG, "Connection failed...");

        esp_tls_conn_delete(tls);
        return false;
    }

    size_t written_bytes = 0;
    do {
        int tls_ret = esp_tls_conn_write(tls, req + written_bytes, req_len - written_bytes);

        if (tls_ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", tls_ret);
            written_bytes += tls_ret;

        } else if (tls_ret != ESP_TLS_ERR_SSL_WANT_READ && tls_ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", tls_ret, esp_err_to_name(tls_ret));
            
            esp_tls_conn_delete(tls);
            return false;
        }
    } while (written_bytes < req_len);

    ESP_LOGI(TAG, "Waiting for HTTPS response...");

    if (res == NULL) {    
        char buf;
        (void)esp_tls_conn_read(tls, &buf, 1);

    } else {
        int res_len = 0;
        int res_buf = 129;
        *res = (char*)malloc(sizeof(char) * res_buf);
        if (*res == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for response buffer");

            esp_tls_conn_delete(tls);
            return false;
        }
        (*res)[0] = '\0';

        bool success = true;
        while (1) {
            char buf[129];
            bzero(buf, sizeof(buf));
            int ret = esp_tls_conn_read(tls, (char *)buf, sizeof(buf) - 1);

            ESP_LOGI(TAG, "esp_tls_conn_read  returned %d", ret);

            if (ret == ESP_TLS_ERR_SSL_WANT_WRITE || ret == ESP_TLS_ERR_SSL_WANT_READ) {
                ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
                continue;
            }

            if (ret < 0) {
                ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
                success = false;
                break;
            }

            if (ret == 0) {
                ESP_LOGI(TAG, "connection closed");
                break;
            }

            ESP_LOGI(TAG, "%d bytes read", ret);
            
            if (res_len + ret > res_buf - 1) {
                res_buf = res_len + ret + 1;
                char* new_res = (char*)realloc(*res, sizeof(char) * res_buf);
                if (new_res == NULL) {
                    ESP_LOGE(TAG, "Failed to reallocate memory for response buffer");
                    success = false;
                    break;
                }
                *res = new_res;
            }

            memcpy(*res + res_len, buf, ret);
            res_len += ret;
            (*res)[res_len] = '\0';
        }
        
        if (success) {
            ESP_LOGI(TAG, "Response: %s", *res);

        } else {
            ESP_LOGE(TAG, "Failed to read response");
            free(*res);
            *res = NULL;
            esp_tls_conn_delete(tls);
            return false;
        }
    }

    esp_tls_conn_delete(tls);
    return true;
}

#define NEW_STRING(var, format, ...) \
    int var##_len = snprintf(NULL, 0, format, __VA_ARGS__); \
    ESP_LOGI(TAG, "Allocating %d bytes for %s", var##_len + 1, #var); \
    char* var = (char*)malloc(sizeof(char) * (var##_len + 1)); \
    snprintf(var, var##_len + 1, format, __VA_ARGS__)

bool https_get_request(const char* server, const char* url, char** res, const uint8_t* cacert_start, const uint8_t* cacert_end) {
    NEW_STRING(full_url, base_url, server, url);
    NEW_STRING(req, base_get_req, full_url, server);

    int ret = https_request(full_url, req, req_len, res, cacert_start, cacert_end);

    free(full_url);
    free(req);

    return ret;
}

bool https_post_request(const char* server, const char* url, const char* body, char** res, const uint8_t* cacert_start, const uint8_t* cacert_end) {
    NEW_STRING(full_url, base_url, server, url);
    NEW_STRING(req, base_post_req, full_url, server, strlen(body), body);

    ESP_LOGI("HTTPS-Request", "POST %s", full_url);

    int ret = https_request(full_url, req, req_len, res, cacert_start, cacert_end);

    free(full_url);
    free(req);

    return ret;
}
