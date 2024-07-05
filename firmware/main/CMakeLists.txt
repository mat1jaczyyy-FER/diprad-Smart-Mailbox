idf_component_register(SRCS "main.cpp" "sleep.cpp" "wifi.cpp" "https_request.cpp" "hc_sr04.cpp" "infrared.cpp" "debouncer.cpp" "status_led.cpp"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "server_root_cert.pem")
