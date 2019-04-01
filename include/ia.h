#ifndef IA_H_   /* Include guard */
#define IA_H_

/***************************Library***************/
#include "freertos/FreeRTOS.h"
#include "comz.h"
#include "Arduino.h"

void Task_IA(void * parameter);
void IA_Init();
void IA_Setup();
void Change_Mode();
void Automatique_Position();
void Update_Local_Variable();

#endif // IA_H_