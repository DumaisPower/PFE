/*⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆ 
⋆  Project: PFE 
⋆  Function: 
⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆ 
⋆  $Author: DumaisPower  
⋆  $Name: Dany Dumais   
⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆ 
⋆ 
⋆  Copyright 2019 by  
⋆ 
⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆⋆*/

/*********************************ESP32 LIB***************************************/
#include "BluetoothSerial.h" 
#include <esp_bt.h>
#include "soc/rtc.h"
#include "esp32/pm.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
/*****************************Include File**************************/
#include "capteur.h"
#include "comz.h"
#include "motor.h"
#include "ia.h"
/*************************sub program declaration******************************/
void Config_Init();

/**********************Semaphore for task manager********************************/
SemaphoreHandle_t SemaphoreMotor = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t SemaphoreSensor = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t	SemaphoreComz = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t	SemaphoreIA = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t BarrierComz = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t BarrierMotor = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t BarrierSensor = xSemaphoreCreateCounting( 1, 0 );


wifi_init_config_t config ;
BluetoothSerial ESP_BT;

void setup()
{
  //set initial serial communication
  Comz_Init();

  Config_Init();

  Motor_Init();

  Sensor_Init();

  IA_Init();

  Blynk_Print_Terminal("All Initialisation done and succesfull");

}

void Config_Init()
{
  ESP_BT.begin("ESP32_BT");
  Serial.begin(9600);
  delay(300); 

  console_Debug("ESP setup");

  //enable wakup sourcce
  console_Err(esp_sleep_enable_timer_wakeup(8000000), "esp_sleep_enable_timer_wakeup : ");
  console_Err(esp_sleep_enable_ulp_wakeup(), "esp_sleep_enable_ulp_wakeup : ");
  //consoleErr(gpio_wakeup_enable(), "gpio_wakeup_enable : ");
  //consoleErr(esp_sleep_enable_gpio_wakeup(), "esp_sleep_enable_gpio_wakeup : ");

  //close what we don't use
  delay(300); 
  console_Err(esp_bluedroid_disable(), "esp_bluedroid_disable : ");
  delay(300); 
  console_Err(esp_bt_controller_disable(), "esp_bt_controller_disable : ");
  delay(300); 
  Blynk_Flush_Terminal();
  delay(300); 

  //setup for comz
  console_Debug("Task Comz Start");
  //changing cpu speed
  //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  //initial setup of the blind

}
 

void loop() 
{
  vTaskSuspend(NULL);
}