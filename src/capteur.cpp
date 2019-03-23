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
#include "capteur.h"

void Task_Sensor(void * parameter);
TaskHandle_t TaskSen;


Adafruit_MLX90614 IR_Sensor = Adafruit_MLX90614();
TwoWire I2Cone = TwoWire(0);

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreSensor;

/******************Variable**********************/
double insideTemp;
double SunLevel;
extern double niveauBatterie;
extern float niveauBatteriePourcent;

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

  Wire.begin(SDA1,SCL1);
  //I2Cone.begin(SDA1,SCL1,400000); // SDA pin 21, SCL pin 22 TTGO TQ


}

void Task_Sensor(void * parameter)
{

  //terminal.println("Task Sensor Start");
 
  pinMode(AnalogTMP,INPUT);
  pinMode(AnalogSUN,INPUT);
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
      insideTemp = Get_Inside_Temp();
      insideTemp = ((insideTemp*(3300.00/1024.00) - 150) / 100.000);
      console_Debug("ambiant Analog temp");
      console_Debug_Double(insideTemp);
      Blynk_Virtual_Write(1, insideTemp);

      SunLevel = Get_Sun(); //~1000 = noir total , ~2000= normal , ~3000= soleil
      console_Debug("Sun Level");
      console_Debug_Double(SunLevel);

      console_Debug("ambiant IR temp");
      console_Debug_Double(IR_Sensor.readAmbientTempC()); 
      console_Debug("object IR temp");
      console_Debug_Double(IR_Sensor.readObjectTempC());
    }

 

   
    //A chaque X wake up 

    //update inside temp

    //update outside temp

    //update sun on the window
  }
  vTaskDelete( NULL );
}

double Get_Inside_Temp()
{
    return analogRead(AnalogTMP);

}

double Get_Sun()
{
    return analogRead(AnalogSUN);

}

int Get_Niv_Bat_Poucent()
{
  
    if(niveauBatterie >= 3.9)
    {
      niveauBatteriePourcent = 41.6667*niveauBatterie-75;
    }
    else if((niveauBatterie < 3.9) && (niveauBatterie > 3.6))
    {
        niveauBatteriePourcent = 250*niveauBatterie-887.5;
    }
    else            //niveauBatterie <= 3.6
    {
        niveauBatteriePourcent = 16.129*niveauBatterie-45.5645;
    }
    return niveauBatteriePourcent;
}


