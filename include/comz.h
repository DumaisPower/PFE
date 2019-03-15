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

#define BLYNK_PRINT Serial

void Comz_Init();

void console_Err(esp_err_t err, String StringToPrint);

void Blynk_Run();

void Blynk_Clear_Terminal();

void Blynk_Flush_Terminal();

void Blynk_Print_Terminal(String StringToPrint);

void console_Debug(String StringToPrint);

void Go_To_Sleep();

#endif // COMZ_H_
