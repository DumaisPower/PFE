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
#include "motor.h"


DRV8834 stepper(MOTOR_STEPS, DIR, STEP, SLEEP/*, M0, M1*/);

void Task_Moteur(void * parameter);
TaskHandle_t TaskMot;

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;


void Motor_Init()
{
    xTaskCreatePinnedToCore(
                    Task_Moteur,                 /* Task function. */
                    "TaskMot",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */
    
    delay(500); 

}


void Task_Moteur(void * parameter)
{
  //setup for motor
  //terminal.println("Task Motor Start");
 
  unsigned long currentMillis;
  unsigned long nextMillis = 0;

  stepper.begin(RPM, MICROSTEPS);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
  stepper.enable();

  stepper.startRotate(360);

  /* The parameter value is expected to be 1 as 1 is passed in the
  pvParameters value in the call to xTaskCreate() below.*/ 
 //configASSERT( ( ( uint32_t ) parameter ) == 1 );

  xSemaphoreTake(BarrierMotor, portMAX_DELAY);
  xSemaphoreGive(BarrierComz);
  //run task motor
  while(1)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreMotor, portMAX_DELAY);

    //stepperMotor();

    currentMillis = millis();
    if (currentMillis  > nextMillis + 7000)
    {
      console_Debug("test1");
      //stepper.enable();
      stepper.rotate(360);
      //stepper.disable();
      nextMillis = currentMillis;
    }
    stepper.rotate(360);
    //si manuelle attend un mouvement des GPIO/Blynk

      //compte le nombre de step a tournee 

      //met a jour blynk

      //retourne a attendre une nouvelle commande

    //si automatique attend un mouvement des Sensor/IA/GPIO/Blynk

      //compte le nombre de step a tournee 

      //met a jour blynk

      //retourne a attendre une nouvelle commande
  }
  vTaskDelete( NULL );
}


