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
double insideTempAnalogTmp;
double insideTempIRTmp;
double ObjectTempIRTmp;
double SunLevelTmp;
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

  }
  vTaskDelete( NULL );
}

void Update_Inside_Temp_Analog()
{
  uint16_t Tempon;
  Tempon = analogRead(AnalogTMP);
  insideTempAnalogTmp = ((Tempon*(3300.00/1024.00) - 500) / 100.000);
  console_Debug("ambiant Analog temp");
  console_Debug_Double(insideTempAnalogTmp);
  Blynk_Virtual_Write(TEMP_INT, insideTempAnalogTmp);
  return ;

}

void  Update_Inside_Temp_IR()
{
  insideTempIRTmp = IR_Sensor.readAmbientTempC();
  console_Debug("ambiant IR temp");
  console_Debug_Double(insideTempIRTmp); 
  return ;
}

void Update_Object_Temp_IR()
{
  ObjectTempIRTmp = IR_Sensor.readObjectTempC();
  console_Debug("object IR temp");
  console_Debug_Double(ObjectTempIRTmp);
  return ;
}

void Update_Sun()
{
  SunLevelTmp = analogRead(AnalogSUN);
  console_Debug("Sun Level");
  console_Debug_Double(SunLevelTmp);

  return ;
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


