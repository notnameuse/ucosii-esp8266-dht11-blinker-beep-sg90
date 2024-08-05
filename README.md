Using the STM32F103C8T6 development board and SG90 servo, ESP8266 module, TFLCD display, 2 buttons, buzzer, DHT11 temperature and humidity sensor and light sensor, a kitchen early warning monitoring system was developed. The main functions of the system are as follows:
1. Automatic light switch function: When the light intensity is weak, the kitchen light will be automatically turned on, and the current light intensity will be displayed on the LCD screen at this time. When the light intensity is strong, the kitchen lights will be automatically turned off. In addition, when the lights need to be turned off at night, the kitchen lights can be turned off manually and cannot be turned on automatically without affecting the continued execution of other functions.
2. Light collection function: Use a photosensitive sensor to collect light intensity and display it on the LCD screen.
3. DHT11 temperature and humidity collection and early warning function: first collect the current temperature and humidity information and display it on the LCD screen, set the temperature and humidity threshold, if the temperature threshold is exceeded, the buzzer will alarm. When the humidity threshold is exceeded, the servo rotates. And below the temperature threshold, the servo reverses. In addition, when the buzzer alarms, you need to manually turn off the buzzer. At this time, the buzzer will no longer trigger automatically and will not affect the continued execution of other functions.
4. APP monitors temperature and humidity and performs early warning functions: Upload the temperature and humidity information collected by DHT11 to the ESP8266 module through the serial port, and then upload it to the mobile APP for graphical real-time display through WIFI. And when the temperature in the kitchen is detected and the buzzer alarms, the mobile APP sends an alarm message to the user's mobile phone through WeChat.
