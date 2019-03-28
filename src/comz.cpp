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
#include "comz.h"

#include <BlynkSimpleEsp32.h>

WidgetTerminal terminal(V0);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1334465b93034f92ad14742fb88eb305";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "iPhone de Dany";
char pass[] = "12345678";

void Task_Communication(void * parameter);

TaskHandle_t TaskCom;

extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;
extern SemaphoreHandle_t	SemaphoreSensor;

/****************Variable**************************/
String tamponJson;
bool controle = STATE_MANUEL;
int setStore;
float tempC = 0;
double niveauBatterie;
int niveauBatteriePourcent;
float temp_ext_K;                 
float temp_ext_C;
bool MotorChange;
double PositionDesire;
double MotorPosition;

unsigned long currentMillisSensor;
unsigned long nextMillisSensor = 0; 


void Comz_Init()
{

    xTaskCreatePinnedToCore(
                    Task_Communication,          /* Task function. */
                    "TaskCom",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */

  delay(500); 

  //Connect to wifi
  Blynk.begin(auth, ssid, pass);

  delay(500); 

}

void Comz_Setup()
{
  Blynk.virtualWrite(V6, "http://api.openweathermap.org/data/2.5/weather?id=6077243&appid=46f4c65c517848ec7a0bb9282b4e0ec8");

  return;
}

void Task_Communication(void * parameter)
{
  //setup for comz
  console_Debug("Task Comz Start");
 
  Comz_Setup();

  //waiting every task to be setup
  xSemaphoreTake(BarrierComz, portMAX_DELAY);

 //run task comz
  while(true)
  {
    //give ping to blynk and get value if change
    Blynk_Run();

    if(Timer_Sensor(SENSOR_REFRESH_MILISEC))
    {
      xSemaphoreGive(SemaphoreSensor);//do sensor task
    }  
    
    if(MotorChange)
    {
      xSemaphoreGive(SemaphoreMotor);//do motor taksk
    }
    
    //while no change go to sleep
    Go_To_Sleep();

  }
  vTaskDelete( NULL );

}

void Go_To_Sleep()
{
  console_Debug("Entering sleep mode");

  //stop wifi connection
  esp_wifi_stop();
  
  //go to sleep for 7 sec or GPIO press
  esp_light_sleep_start();
 
  //restart wifi
  esp_wifi_start();

  //connect to blynk
  Blynk.begin(auth, ssid, pass);

  console_Debug("just woke up");
}

void console_Err(esp_err_t  err, String StringToPrint)
{
  const char * err_name;
  err_name = esp_err_to_name(err);
  terminal.println(StringToPrint + err_name );
  terminal.flush();
  
  return;
}

void Blynk_Run()
{
  Blynk.run();
}

void Blynk_Clear_Terminal()
{
  terminal.clear();
}

void Blynk_Print_Terminal(String StringToPrint)
{
  terminal.println(StringToPrint);
}

void Blynk_Flush_Terminal()
{
  terminal.flush();
}

void console_Debug(String StringToPrint)
{
  terminal.println(StringToPrint);
  Blynk.run();
  
  return;
}

void console_Debug_Int(int IntToPrint)
{
  terminal.println(IntToPrint);
  Blynk.run();
 
  return;
}

void console_Debug_Double(double DoubleToPrint)
{
  terminal.println(DoubleToPrint);
  Blynk.run();

  return;
}

void Blynk_Virtual_Write(int pin, double Value)
{
  Blynk.virtualWrite(pin, Value);
}

BLYNK_WRITE(WEBHOOK) //WEBHOOK
{
  tamponJson = param.asStr();
}

BLYNK_WRITE(NIV_STORE_MAN)
{
   setStore = param.asInt();  // met la valeur du slider dans la variable setStore
}

BLYNK_WRITE(AUTO_MAN) // SWITCH MANUEL/AUTO
{
  switch (param.asInt())
  {
    case 1: { 
        controle = STATE_MANUEL;
        Set_Motor_Change();
        break;
      }
    case 2: { 
        controle = STATE_AUTO;
        Set_Motor_Change();
        break;
      }
    }
    return;
}

float get_temp_ext()
{

  StaticJsonDocument<2000> doc;
  DeserializationError error = deserializeJson(doc,tamponJson);     
  if (error) 
  {
    console_Debug("deserializeJson() failed with code ");             
    console_Debug(error.c_str());
  }  
 
    String name = doc["name"];                             
    temp_ext_K = doc["main"]["temp"];                   
    temp_ext_C = temp_ext_K - 273.15;                        

    Blynk.virtualWrite(TEMP_EXT, temp_ext_C);

    return temp_ext_C;
}

bool Timer_Sensor(int MiliSeconde)
{
    currentMillisSensor = millis();
    if (currentMillisSensor  > nextMillisSensor + MiliSeconde)
    {     
      nextMillisSensor = currentMillisSensor;
      return true;
    }
    return false;
}

void Set_Motor_Change()
{
  MotorChange = true;
  return;
}

void Reset_Motor_Change()
{
  MotorChange = false;
  return;
}

void Set_Position_Desire(int Pourcentage)
{

  //formule qui calcul le nombre de step du moteur
  return ;
}

double Get_Position_Desire()
{
  return PositionDesire;
}

void Set_Motor_Pos(double NewPosition)
{

  MotorPosition = NewPosition;
  //formule qui calcul le nombre de step du moteur
  return ;
}

double Get_Motor_Pos()
{
  return MotorPosition;
}