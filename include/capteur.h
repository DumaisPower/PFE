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

#define ANALOGSUN 37

void Task_Sensor(void * parameter);

void Sensor_Init();
void Update_Inside_Temp_IR();
void Update_Object_Temp_IR();
void Update_Sun();
int Get_Temp_Desirer();
#endif // SENSOR_H_
