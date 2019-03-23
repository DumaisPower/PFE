#ifndef COMZ_H_   /* Include guard */
#define COMZ_H_

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "WString.h"
#include "esp32/pm.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include <ArduinoJson.h> 

#define BLYNK_PRINT Serial

#define STATE_MANUEL 0
#define STATE_AUTO 1

#define TEMP_INT 1
#define TEMP_EXT V2
#define NIV_STORE_MAN V3
#define AUTO_MAN V4
#define NIV_BAT 5
#define WEBHOOK V6

void Comz_Init();

void console_Err(esp_err_t err, String StringToPrint);

void Blynk_Run();

void Blynk_Clear_Terminal();

void Blynk_Flush_Terminal();

void Blynk_Print_Terminal(String StringToPrint);

void console_Debug(String StringToPrint);

void console_Debug_Int(int IntToPrint);

void console_Debug_Double(double DoubleToPrint);

void Go_To_Sleep();

void Blynk_Virtual_Write(int pin, double Value);

#endif // COMZ_H_
