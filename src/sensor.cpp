#include "sensor.h"

void Task_Sensor(void * parameter);
TaskHandle_t TaskSen;

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreSensor;

void Sensor_Init()
{

          xTaskCreatePinnedToCore(
                    Task_Sensor,                  /* Task function. */
                    "TaskSen",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */

  delay(500); 

}

void Task_Sensor(void * parameter)
{

  //terminal.println("Task Sensor Start");
 
  //setup for sensor
  unsigned long currentMillis ;
  unsigned long nextMillis = 0;  
  //run task sensor

  /* The parameter value is expected to be 1 as 1 is passed in the
  pvParameters value in the call to xTaskCreate() below.*/ 
 //configASSERT( ( ( uint32_t ) parameter ) == 1 );

  xSemaphoreGive(BarrierMotor);
  
  while(1)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreSensor, portMAX_DELAY);

    currentMillis = millis();
    if (currentMillis  > nextMillis + 7000)
    {
      console_Debug("test2");
      nextMillis = currentMillis;
    }

   
    //A chaque X wake up 

    //update inside temp

    //update outside temp

    //update sun on the window
  }
  vTaskDelete( NULL );
}
