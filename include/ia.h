#ifndef IA_H_   /* Include guard */
#define IA_H_

/***************************Library***************/
#include "freertos/FreeRTOS.h"
#include "comz.h"
#include "Arduino.h"

#define NIGHT 0
#define DAY 1
#define DAYLIGHTMIN 2000

void Task_IA(void * parameter);
void IA_Init();
void IA_Setup();
void Change_Mode();
void Automatique_Position();
void Update_Local_Variable();
void Update_Day_Night();
void Bat_To_Pourcentage();
void Read_Niv_Bat();
void Set_Bat_Niv_Color();

#endif // IA_H_