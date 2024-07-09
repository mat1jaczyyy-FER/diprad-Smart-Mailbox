#include "config.h"
#include "api.h"

#include "https_request.h"

/* 
   openssl s_client -showcerts -connect server:443 </dev/null
   The CA root cert is the last cert given in the chain of certs.
*/
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

char* api_config() {
    char* res = NULL;

    (void)https_post_request(
        CONFIG_API_SERVER,
        CONFIG_API_BASE_URL "config",
        "{\"code\":\"" CONFIG_MAILBOX_PRIVATE_CODE "\"}",
        &res,
        server_root_cert_pem_start,
        server_root_cert_pem_end
    );

    return res;
}

bool api_notify() {
    return https_post_request(
        CONFIG_API_SERVER,
        CONFIG_API_BASE_URL "config",
        "{\"code\":\"" CONFIG_MAILBOX_PRIVATE_CODE "\"}",
        NULL,
        server_root_cert_pem_start,
        server_root_cert_pem_end
    );
}
