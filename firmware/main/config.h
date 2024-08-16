#ifndef _CONFIG_h
#define _CONFIG_h

#define CONFIG_MAILBOX_PUBLIC_CODE          "CXIGJ5A"
#define CONFIG_MAILBOX_PRIVATE_CODE         "WTCO7D1"

#define CONFIG_SLEEP_INTERVAL               0
#define CONFIG_SLEEP_MAXLEVEL               0

//#define CONFIG_USE_ULTRASONIC
#define CONFIG_GPIO_ULTRASONIC_TRIG         GPIO_NUM_3
#define CONFIG_GPIO_ULTRASONIC_ECHO         GPIO_NUM_2

//#define CONFIG_MAIL_EMPTY_FROM              54
//#define CONFIG_MAIL_EMPTY_TO                58

#define CONFIG_USE_INFRARED
#define CONFIG_GPIO_INFRARED_EN_RX          GPIO_NUM_1
#define CONFIG_GPIO_INFRARED_EN_TX          GPIO_NUM_10
#define CONFIG_ADC_INFRARED_RX              ADC1_CHANNEL_0

#define CONFIG_MAIL_EMPTY_FROM              0
#define CONFIG_MAIL_EMPTY_TO                100

#define CONFIG_MAIL_SENSOR_POLL_DELAY       30
#define CONFIG_MAIL_DEBOUNCE_ITERATIONS     20

#define CONFIG_USE_STATUS_LED
#define CONFIG_RMT_STATUS_LED               RMT_CHANNEL_0
#define CONFIG_GPIO_STATUS_LED              GPIO_NUM_8

#define CONFIG_WIFI_MAXRETRY                3

#define CONFIG_API_SERVER                   "smart-mailbox.mat1jaczyyy.com"
#define CONFIG_API_BASE_URL                 "api/mailbox/"

#endif
