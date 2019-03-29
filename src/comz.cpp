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

/*******************Your WiFi credentials******************/
char ssid[] = "HONDAGMC";
char pass[] = "18182321yougi";
char auth[] = "1334465b93034f92ad14742fb88eb305";

/********************Widget*****************************/
WidgetTerminal terminal(TERMINAL);

/*****************Semaphore and barier*****************/
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;
extern SemaphoreHandle_t	SemaphoreSensor;

TaskHandle_t TaskCom;

/****************Variable**************************/
//Controle Blynk
bool controle = STATE_MANUEL;
int setStore;
int HauteurFenetre;

//baterie
double niveauBatterie;
int niveauBatteriePourcent;

//jason
String tamponJson;
double TempExtK;                 
double TempExtC;

//motor
bool MotorChange;
double PositionDesire;
double MotorPosition;
double MotorPositionPercentage;
double MaxPosition;

//capteur
double insideTempAnalog;
double insideTempIR;
double ObjectTempIR;
double SunLevel;

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
  Blynk.virtualWrite(WEBHOOK, "http://api.openweathermap.org/data/2.5/weather?id=6077243&appid=46f4c65c517848ec7a0bb9282b4e0ec8");

  Set_Max_Position();

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

      //xSemaphoreGive(SemaphoreIA);//do IA task                  not yet implement

      Update_Blynk_Sensor();
    }  
    
    if(MotorChange)
    {
      xSemaphoreGive(SemaphoreMotor);//do motor taksk

      MotorPositionPercentage = Step_To_Percentage(MotorPosition);

      Update_Blynk_Motor_Pos();
    }
    
    //while no change go to sleep
    Go_To_Sleep();

  }
  vTaskDelete( NULL );

}

void Update_Blynk_Sensor()
{
  Blynk_Virtual_Write(TEMP_INT, insideTempIR);

  Blynk_Virtual_Write(TEMP_EXT, TempExtC);

  Blynk_Virtual_Write(NIV_SUN, SunLevel);

  return;
}

void Update_Blynk_Motor_Pos()
{
  Blynk_Virtual_Write(NIV_STORE_MAN, MotorPositionPercentage);

  return;
}

double Step_To_Percentage(double step)
{
  return (step / MaxPosition) * 100;
}

double Percentage_To_Step(double percentage)
{
  return (percentage / 100) * MaxPosition;
}

void Set_Max_Position()
{
  //formule qui convertie le nombre de metre en step
  MaxPosition = 10000 ; 
  return;
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
  return;
}

void Blynk_Clear_Terminal()
{
  terminal.clear();
  return;
}

void Blynk_Print_Terminal(String StringToPrint)
{
  terminal.println(StringToPrint);
  return;
}

void Blynk_Flush_Terminal()
{
  terminal.flush();
  return;
}

void console_Debug(String StringToPrint)
{
  terminal.println(StringToPrint); 
  return;
}

void console_Debug_Int(int IntToPrint)
{
  terminal.println(IntToPrint);
  return;
}

void console_Debug_Double(double DoubleToPrint)
{
  terminal.println(DoubleToPrint);
  return;
}

void Blynk_Virtual_Write(int pin, double Value)
{
  Blynk.virtualWrite(pin, Value);
  return;
}

BLYNK_WRITE(WEBHOOK) //WEBHOOK
{
  tamponJson = param.asStr();
  return;
}

BLYNK_WRITE(HAUTEUR_FENETRE) //WEBHOOK
{
  HauteurFenetre = param.asInt();
  return;
}

BLYNK_WRITE(NIV_STORE_MAN)
{
  setStore = param.asInt();  // met la valeur du slider dans la variable setStore
  return;
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

void Set_Outside_Temp()
{

  StaticJsonDocument<2000> doc;
  DeserializationError error = deserializeJson(doc,tamponJson);     
  if (error) 
  {
    console_Debug("deserializeJson() failed with code ");             
    console_Debug(error.c_str());
  }  
 
    String name = doc["name"];                             
    TempExtK = doc["main"]["temp"];                   
    TempExtC = TempExtK - 273.15;                        

    Blynk.virtualWrite(TEMP_EXT, TempExtC);

    return;
}

double Get_Outside_Temp()
{
  return TempExtC;
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

double Get_Inside_Temp_Analog()
{
  return insideTempAnalog;
}

void Set_Inside_Temp_Analog(double NewTempAnalog)
{
  insideTempAnalog = NewTempAnalog;
  return ;
}

double  Get_Inside_Temp_IR()
{
  return insideTempIR;
}

void  Set_Inside_Temp_IR(double NewTempIR)
{
  insideTempIR = NewTempIR;
  return ;
}

void Set_Object_Temp_IR(double NewObjectIR)
{
  ObjectTempIR = NewObjectIR;
  return ;
}

double Get_Object_Temp_IR()
{
  return ObjectTempIR;
}

void Set_Sun(double NewSunLevel)
{
  SunLevel = NewSunLevel;
  return ;
}

double Get_Sun()
{
  return SunLevel;
}