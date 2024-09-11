#ifndef _CONFIG_h
#define _CONFIG_h

#define CONFIG_MAILBOX_PUBLIC_CODE          "CXIGJ5A"
#define CONFIG_MAILBOX_PRIVATE_CODE         "WTCO7D1"

#define CONFIG_SLEEP_MAXLEVEL               1

#define CONFIG_TEST_SLEEP_INTERVAL          3 // s
//#define CONFIG_TEST_DISABLE_NOTIFY

//#define CONFIG_USE_ULTRASONIC
#define CONFIG_ULTRASONIC_GPIO_EN           GPIO_NUM_10
#define CONFIG_ULTRASONIC_GPIO_TRIG         GPIO_NUM_1
#define CONFIG_ULTRASONIC_GPIO_ECHO         GPIO_NUM_0

#define CONFIG_USE_INFRARED
#define CONFIG_INFRARED_GPIO_TX_EN          GPIO_NUM_10
#define CONFIG_INFRARED_GPIO_RX_EN          GPIO_NUM_1
#define CONFIG_INFRARED_ADC_RX              ADC1_CHANNEL_0

#define CONFIG_MEASURE_DISCARD              1
#define CONFIG_MEASURE_ITERATIONS           41
#define CONFIG_MEASURE_SUCCESS              20
#define CONFIG_MEASURE_DELAY                50 // ms

#define CONFIG_USE_STATUS_LED
#define CONFIG_STATUS_LED_RMT               RMT_CHANNEL_0
#define CONFIG_STATUS_LED_GPIO              GPIO_NUM_8

//#define CONFIG_USE_BATTERY_CHECK
#define CONFIG_BATTERY_CHECK_ADC            ADC1_CHANNEL_2
#define CONFIG_BATTERY_CHECK_THRESH         800 // mV

#define CONFIG_WIFI_MAXRETRY                3

#define CONFIG_API_SERVER                   "smart-mailbox.mat1jaczyyy.com"
#define CONFIG_API_BASE_URL                 "api/mailbox/"
#define CONFIG_API_FAIL_INTERVAL            10 // s

#endif
