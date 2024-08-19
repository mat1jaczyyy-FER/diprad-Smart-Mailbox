idf_component_register(SRCS "infrared.cpp" "main.cpp" "sleep.cpp" "wifi.cpp" "https_request.cpp" "api.cpp" "hc_sr04.cpp" "status_led.cpp" "battery.cpp"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "server_root_cert.pem")
