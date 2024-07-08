#ifndef _CONFIG_h
#define _CONFIG_h

#define CONFIG_SLEEP_INTERVAL               0
#define CONFIG_SLEEP_MAXLEVEL               0

//#define CONFIG_USE_ULTRASONIC
#define CONFIG_GPIO_ULTRASONIC_TRIG         GPIO_NUM_3
#define CONFIG_GPIO_ULTRASONIC_ECHO         GPIO_NUM_2

//#define CONFIG_MAIL_EMPTY_FROM              54
//#define CONFIG_MAIL_EMPTY_TO                58

#define CONFIG_USE_INFRARED
#define CONFIG_GPIO_INFRARED_EN             GPIO_NUM_3
#define CONFIG_ADC_INFRARED_RECV            ADC1_CHANNEL_2

#define CONFIG_MAIL_EMPTY_FROM              0
#define CONFIG_MAIL_EMPTY_TO                100

#define CONFIG_MAIL_SENSOR_POLL_DELAY       30
#define CONFIG_MAIL_DEBOUNCE_ITERATIONS     20

#define CONFIG_USE_STATUS_LED
#define CONFIG_GPIO_STATUS_LED_R            GPIO_NUM_0
#define CONFIG_GPIO_STATUS_LED_G            GPIO_NUM_1
#define CONFIG_GPIO_STATUS_LED_B            GPIO_NUM_10

#define CONFIG_WIFI_MAXRETRY                3

#define CONFIG_NOTIFICATION_SERVER          "api.telegram.org"
#define CONFIG_NOTIFICATION_URL             "bot6613411665:AAGZ7HPSl2-hDrlzkZS0U_EXmaUIO8DVLd4/sendMessage?chat_id=267163277&text=You%27ve%20got%20mail!"

#endif
