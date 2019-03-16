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
SemaphoreHandle_t BarrierComz = xSemaphoreCreateCounting( 1, 0 );
SemaphoreHandle_t BarrierMotor = xSemaphoreCreateCounting( 1, 0 );


wifi_init_config_t config ;
 BluetoothSerial ESP_BT;

void setup()
{
  //set initial serial communication
  Comz_Init();

  Config_Init();

  Motor_Init();

  Sensor_Init();

  Blynk_Print_Terminal("All Initialisation done and succesfull");

}

void Config_Init()
{
  ESP_BT.begin("ESP32_BT");
  Serial.begin(9600);

  delay(300); 

  //clear terminal
  Blynk_Clear_Terminal();

  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  Blynk_Print_Terminal("Blynk : Device started");
  Blynk_Print_Terminal("-------------");
  
  /* If we use other app than blynk it might be useful*/
  // consoleErr(esp_wifi_init(&config), "esp_wifi_init : ");
  // delay(300); 
  // consoleErr(esp_wifi_start(), "esp_wifi_start : ");
  // delay(300); 
  // consoleErr(esp_wifi_set_auto_connect(1), "esp_wifi_set_auto_connect : ");
  
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

  //changing cpu speed
  //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  //initial setup of the blind

}
 

void loop() 
{
  vTaskSuspend(NULL);
}