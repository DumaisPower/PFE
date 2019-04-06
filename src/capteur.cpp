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

TaskHandle_t TaskSen;

Adafruit_MLX90614 IR_Sensor = Adafruit_MLX90614();
TwoWire I2Cone = TwoWire(0);

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreSensor;
extern SemaphoreHandle_t	BarrierSensor;
extern SemaphoreHandle_t	SemaphoreComz;

/******************Variable**********************/
double insideTempAnalogTmp;
double insideTempIRTmp;
double ObjectTempIRTmp;
double SunLevelTmp;


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

  return;
}

void Sensor_Setup()
{
  pinMode(ANALOGTMP,INPUT);
  pinMode(ANALOGSUN,INPUT);

  return;
}

void Task_Sensor(void * parameter)
{
  console_Debug("Task Sensor Start");
  
  Sensor_Setup();

  xSemaphoreTake(BarrierSensor,portMAX_DELAY);
  xSemaphoreGive(BarrierMotor);//retourne a motor
  
  //run task sensor
  while(true)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreSensor, portMAX_DELAY);

    console_Debug("Doing Sensor Task");

    //update the sensor value
    Update_Inside_Temp_Analog();

    Update_Sun(); //~1000 = noir total , ~2000= normal , ~3000= soleil
      
    Update_Inside_Temp_IR();

    Update_Object_Temp_IR();

    //set the sensor value 
    Set_Inside_Temp_Analog(insideTempAnalogTmp);

    Set_Sun(SunLevelTmp); //~1000 = noir total , ~2000= normal , ~3000= soleil
      
    Set_Inside_Temp_IR(insideTempIRTmp);

    Set_Object_Temp_IR(ObjectTempIRTmp);

    xSemaphoreGive(SemaphoreComz); //retourne a comz

  }
  vTaskDelete( NULL );
}

void Update_Inside_Temp_Analog()
{
  uint16_t Tempon;
  Tempon = analogRead(ANALOGTMP);
  insideTempAnalogTmp = ((Tempon*(3300.00/1024.00) - 350) / 100.000);

  return ;
}

void  Update_Inside_Temp_IR()
{
  insideTempIRTmp = IR_Sensor.readAmbientTempC();
  if (insideTempIRTmp > 100 )
  {
    Wire.begin(SDA1,SCL1);
  }

  return ;
}

void Update_Object_Temp_IR()
{
  ObjectTempIRTmp = IR_Sensor.readObjectTempC();

  return ;
}

void Update_Sun()
{
  SunLevelTmp = analogRead(ANALOGSUN);
  SunLevelTmp = (SunLevelTmp /4096 ) * 100;

  return ;
}




