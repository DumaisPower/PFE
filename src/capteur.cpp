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
double insideTempAnalog;
double insideTempIR;
double ObjectTempIR;
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

void Sensor_Setup()
{
  pinMode(AnalogTMP,INPUT);
  pinMode(AnalogSUN,INPUT);

  return;
}

void Task_Sensor(void * parameter)
{
  console_Debug("Task Sensor Start");
  
  Sensor_Setup();

  xSemaphoreGive(BarrierMotor);
  
  //run task sensor
  while(true)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreSensor, portMAX_DELAY);

    console_Debug("test2");

    Get_Inside_Temp_Analog();

    Get_Sun(); //~1000 = noir total , ~2000= normal , ~3000= soleil
      
    //update inside temp

    //update outside temp

    //update sun on the window
  }
  vTaskDelete( NULL );
}

double Get_Inside_Temp_Analog()
{
  analogRead(AnalogTMP);
  insideTempAnalog = ((AnalogTMP*(3300.00/1024.00) - 150) / 100.000);
  console_Debug("ambiant Analog temp");
  console_Debug_Double(insideTempAnalog);
  Blynk_Virtual_Write(TEMP_INT, insideTempAnalog);
  return insideTempAnalog;

}

double  Get_Inside_Temp_IR()
{
  insideTempIR = IR_Sensor.readAmbientTempC();
  console_Debug("ambiant IR temp");
  console_Debug_Double(insideTempIR); 
  return insideTempIR;
}

double Get_Object_Temp_IR()
{
  ObjectTempIR = IR_Sensor.readObjectTempC();
  console_Debug("object IR temp");
  console_Debug_Double(ObjectTempIR);
  return ObjectTempIR;
}

double Get_Sun()
{
  SunLevel = analogRead(AnalogSUN);
  console_Debug("Sun Level");
  console_Debug_Double(SunLevel);

  return SunLevel;
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

  Blynk_Virtual_Write(NIV_BAT, niveauBatteriePourcent);

  return niveauBatteriePourcent;
}


