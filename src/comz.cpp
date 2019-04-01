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
extern SemaphoreHandle_t	SemaphoreComz;

TaskHandle_t TaskCom;

/****************Variable**************************/
//Controle Blynk
bool controle = STATE_MANUEL;
int setStore;
int HauteurFenetre;
bool StayAwake;

//baterie
double niveauBatterie;
int niveauBatteriePourcent;

//jason
String tamponJson;
double TempExtK;                 
double TempExtC;
String CityID = DEFAULT_CITY_ID;

//motor
bool MotorChange = false;
double PositionDesire;
double PositionDesirePercentage;
double MotorPosition;
double MotorPositionPercentage;
double MaxPosition;
double StepToMove;

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

  delay(1000); 

  return;
}

void Comz_Setup()
{
  Set_Max_Position(DEFAULT_LENGHT);

  delay(500);

  return;
}

void Task_Communication(void * parameter)
{
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
      xSemaphoreTake(SemaphoreComz,portMAX_DELAY);//wait for return of sensor

      Set_Outside_Temp();//do the json black magic

      //xSemaphoreGive(SemaphoreIA);//do IA task                  not yet implement

      Update_Blynk_Sensor();

    }  
    
    if(MotorChange)
    { 
      xSemaphoreGive(SemaphoreMotor);//do motor taksk
      xSemaphoreTake(SemaphoreComz,portMAX_DELAY);//wait for return of motor

      console_Debug_Double(MotorPosition);
      MotorPositionPercentage = Step_To_Percentage(MotorPosition);
      console_Debug_Double(MotorPositionPercentage);

      Update_Blynk_Motor_Pos();

      Reset_Motor_Change();

      if(controle == STATE_MANUEL)
      {
        //skip sleep for 30 seconde
        StayAwake = true;
      }
    }
    
    //while no change go to sleep

    //Go_To_Sleep();

  }
  vTaskDelete( NULL );

}

void Update_Blynk_Sensor()
{
  Blynk_Virtual_Write(TEMP_INT, insideTempIR);

  Blynk_Virtual_Write(TEMP_EXT, TempExtC);

  Blynk_Virtual_Write(NIV_SUN, SunLevel);

  console_Debug("Sun Level");
  console_Debug_Double(SunLevel);

  console_Debug("object IR temp");
  console_Debug_Double(ObjectTempIR);

  console_Debug("ambiant IR temp");
  console_Debug_Double(insideTempIR); 

  console_Debug("ambiant Analog temp");
  console_Debug_Double(insideTempAnalog);

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

void Set_Max_Position(double Feet)
{
  //formule qui convertie le nombre de metre en step
  MaxPosition = Feet * 4000 ; 
  return;
}

void Go_To_Sleep()
{
  console_Debug("Entering sleep");

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
  console_Debug(StringToPrint + err_name );
  Blynk_Flush_Terminal();
  
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
  Blynk_Flush_Terminal();
  delay(100);
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
  Blynk_Flush_Terminal();
  delay(100);
  return;
}

void console_Debug_Int(int IntToPrint)
{
  terminal.println(IntToPrint);
  Blynk_Flush_Terminal();
  delay(100);
  return;
}

void console_Debug_Double(double DoubleToPrint)
{
  terminal.println(DoubleToPrint);
  Blynk_Flush_Terminal();
  delay(100);
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

BLYNK_WRITE(HAUTEUR_FENETRE) 
{
  HauteurFenetre = param.asInt();
  return;
}

BLYNK_WRITE(NIV_STORE_MAN)
{
  setStore = param.asInt();  // met la valeur du slider dans la variable setStore
  if(controle ==  STATE_MANUEL)
  {
    Set_Position_Desire(setStore);
    Set_Step_To_Move(Get_Position_Desire());
    Set_Motor_Change();
  }
  else if(controle  ==  STATE_AUTO)
  {
    Update_Blynk_Motor_Pos();
  }

  return;
}

BLYNK_WRITE(CITY_ID)
{
  String CityIDTmp;
  CityIDTmp = param.asInt();  // met la valeur du slider dans la variable setStore
  if (CityIDTmp.length() == CITY_ID_CHAR )
  {
   CityID = CityIDTmp;
  }
  else
  {
    CityID = DEFAULT_CITY_ID;
  }
  return;
}

BLYNK_WRITE(TEMP_EXT)
{
  TempExtC = param.asInt();  
  return;
}

BLYNK_WRITE(AUTO_MAN) // SWITCH MANUEL/AUTO
{
  switch (param.asInt())
  {
    case 1: { 
        controle = STATE_MANUEL;
        break;
      }
    case 2: { 
        controle = STATE_AUTO;
        break;
      }
    }
    return;
}

void Set_Outside_Temp()
{
  
  Blynk.virtualWrite(WEBHOOK,"http://api.openweathermap.org/data/2.5/weather?id=" + CityID + "&appid=46f4c65c517848ec7a0bb9282b4e0ec8");
  StaticJsonDocument<2000> doc;

  if (tamponJson != "")
  {
    deserializeJson(doc,tamponJson);      
 
    String name = doc["name"];                             
    TempExtK = doc["main"]["temp"];                   
    TempExtC = TempExtK - 273.15;                        

    Blynk_Virtual_Write(TEMP_EXT, TempExtC);
  }
  
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
  PositionDesirePercentage = Pourcentage;
  PositionDesire = Percentage_To_Step(Pourcentage);

  return ;
}

double Get_Position_Desire()
{
  return PositionDesire;
}

void Set_Motor_Pos(double NewPosition)
{
  MotorPosition =  NewPosition;
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

void Set_Step_To_Move(double PosDesire)
{
  StepToMove = PosDesire - MotorPosition;
  
  return;
}

double Get_Step_To_Move()
{
  return StepToMove;
}