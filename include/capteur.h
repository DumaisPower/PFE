#ifndef SENSOR_H_ /* Include guard */
#define SENSOR_H_

/***************************Library***************/
#include "comz.h"
#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "Adafruit_MLX90614.h"

#define SDA1 23
#define SCL1 18

#define AnalogTMP 37
#define AnalogSUN 35

void Sensor_Init();
void  Update_Inside_Temp_Analog();
void  Update_Inside_Temp_IR();
void Update_Object_Temp_IR();
void Update_Sun();
void Sensor_Setup();
#endif // SENSOR_H_
