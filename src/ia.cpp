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
#include "ia.h"

TaskHandle_t TaskIA;

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	BarrierSensor;
extern SemaphoreHandle_t	SemaphoreComz;
extern SemaphoreHandle_t	SemaphoreIA;
extern SemaphoreHandle_t	SemaphoreSensor;

/*********************************Variable***********************/
double InsideTempAnalogIA ;
double InsideTempIRIA ;
double ObjectTempIRIA ;
double OutsideTempIA ;
double SunLevelIA;
 
void IA_Init()
{
    xTaskCreatePinnedToCore(
                    Task_IA,                 /* Task function. */
                    "TaskIA",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */
    
  delay(500); 
  return;
}

void IA_Setup()
{

    return;
}

void Task_IA(void * parameter)
{
  
  console_Debug("Task IA Start");

  IA_Setup();
  
  xSemaphoreGive(BarrierSensor); //retourne a sensor

  //run task motor
  while(true)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreIA, portMAX_DELAY);

    console_Debug("Doing IA Task");

    //calculation

    Update_Local_Variable();

    Change_Mode();//if too long in manual mode 

    Automatique_Position();
    

    xSemaphoreGive(SemaphoreComz); //retourne a comz

  }
  vTaskDelete( NULL );
}

void Change_Mode()
{


}

void Automatique_Position()
{


}

void Update_Local_Variable()
{
    InsideTempAnalogIA = Get_Inside_Temp_Analog();

    InsideTempIRIA = Get_Inside_Temp_IR();

    ObjectTempIRIA = Get_Object_Temp_IR();

    OutsideTempIA = Get_Outside_Temp();

    SunLevelIA = Get_Sun();

    return;

}



