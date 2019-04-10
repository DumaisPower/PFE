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
#include <stdbool.h>
#include <SPI.h>
#include <Ethernet.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

/*******************Your WiFi credentials******************/
char ssid[] = "HONDAGMC";
char pass[] = "18182321yougi";
char auth[] = "14c79904503442c1aaedbc97d0dd487a"; //marc app
//char auth[] = "1334465b93034f92ad14742fb88eb305"; //dan app

/********************Widget*****************************/
WidgetTerminal terminal(TERMINAL);
WidgetRTC rtc;

/*****************Semaphore and barier*****************/
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;
extern SemaphoreHandle_t	SemaphoreSensor;
extern SemaphoreHandle_t	SemaphoreComz;
extern SemaphoreHandle_t	SemaphoreIA;

TaskHandle_t TaskCom;

/****************Variable**************************/
//Controle Blynk
bool controle = STATE_MANUEL;
int SetStore;
int HauteurFenetre;
int TemperatureDesire = DEFAULT_TEMP_DESIRE;

//baterie
double NiveauBatteriePourcent;

//jason
String CityID = DEFAULT_CITY_ID;
String tamponJson;
double TempExtK;                 
double TempExtC;


//motor
bool MotorChange = false;
double PositionDesire;
double PositionDesirePercentage;
double MotorPosition;
double MotorPositionPercentage;
double MaxPosition;
double StepToMove;
int i = 0;
int Side = DROITE;

//capteur
double insideTempIR;
double ObjectTempIR;
double SunLevel;

//Time
unsigned long currentMillisSensor;
unsigned long nextMillisSensor = 0; 
String HeureOuverture;
String HeureFermeture;
String RealTime;
bool StayAwake = false;
bool WaitMotor = false;
unsigned long WaitMotorTime;

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

  //clear terminal
  Blynk_Clear_Terminal();

  delay(1000); 

  Blynk_Print_Terminal("Blynk : Device started");
  Blynk_Print_Terminal("-------------");
  

  console_Debug("Comz setup");
  
  Set_Max_Position(DEFAULT_LENGHT);

  delay(500);

  Set_State_Auto_Manuel(STATE_MANUEL);

  delay(500);

  Update_App_Configuration();

  
  return;
}

void Update_App_Configuration()
{
  Blynl_Sync_Virtual(CITY_ID, "City id number:", &CityID ,STRING);

  Blynl_Sync_Virtual(HAUTEUR_FENETRE, "Hauteur de la fenêtre en pied :", &HauteurFenetre ,INT);

  Blynl_Sync_Virtual(HEURE_FERMETURE, "Heure de fermeture du store :", &HeureFermeture ,STRING);
  
  Blynl_Sync_Virtual(HEURE_OUVERTURE, "Heure d'ouverture du store :", &HeureOuverture ,STRING);

  Blynl_Sync_Virtual(TEMP_DESIRE, "Température intérieur désiré :", &TemperatureDesire ,INT);

  return;

}

void Blynl_Sync_Virtual(int Pin, String Message, void* Variable ,TYPE t)
{
  Blynk.syncVirtual(Pin);

  console_Debug(Message);

  if(t == STRING)
  {
    String* StringTmp1 = (String*)Variable;
    String StringTmp2 = *StringTmp1;
    console_Debug(StringTmp2);
  }
  else if(t == DOUBLE)
  {
    double* DoubleTmp1 = (double*)Variable;
    double DoubleTmp2 = *DoubleTmp1;
    console_Debug_Double(DoubleTmp2);
  }
  else if(t == INT)
  {
    int* IntTmp1 = (int*)Variable;
    int IntTmp2 = *IntTmp1;
    console_Debug_Int(IntTmp2);
  }
  return;
}


void Task_Communication(void * parameter)
{

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

      xSemaphoreGive(SemaphoreIA);//do IA task                  not yet implement
      xSemaphoreTake(SemaphoreComz,portMAX_DELAY);//wait for return of sensor

      Update_Blynk_Sensor();

      if(StayAwake)
      {
        if(i == 4)
        {
          StayAwake = false;
          i=0;
        }  
        i++;  
      }
    }  

    if(MotorChange && !WaitMotor)
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
    else if(WaitMotor)
    {
      if(WaitMotorTime >= WaitMotorTime + 900000)
      {
        Reset_Motor_Wait();
      }
    }
    
    
    //while no change go to sleep
    if(!StayAwake)
    {
      //Go_To_Sleep();
    }
    
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

void console_Debug_Double(int DoubleToPrint)
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

BLYNK_CONNECTED()
{
  rtc.begin();
}

BLYNK_WRITE(WEBHOOK) //WEBHOOK
{
  tamponJson = param.asStr();
  return;
}

BLYNK_WRITE(HAUTEUR_FENETRE) 
{
  HauteurFenetre = param.asInt() + 1;
  return;
}

BLYNK_WRITE(NIV_STORE_MAN)
{
  SetStore = param.asInt();  
  if(controle == STATE_MANUEL)
  {
    Set_Position_Desire(SetStore);
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
    case 1: 
    { 
      controle = STATE_MANUEL;
      console_Debug("Mode Manuel");
      break;
    }
    case 0: 
    { 
      controle = STATE_AUTO;
      console_Debug("Mode Automatique");
      break;
    }
  }
  
  return;
}

BLYNK_WRITE(HEURE_OUVERTURE)
{
  HeureOuverture = param.asInt();
}

BLYNK_WRITE(HEURE_FERMETURE)
{
  HeureFermeture = param.asInt();
}

String Get_Heure_Ouverture()
{
  return HeureOuverture;
}

String Get_Heure_Fermeture()
{
  return HeureFermeture;
}

bool Get_State_Auto_Manuel()
{
  return  controle;
}

void Set_State_Auto_Manuel(bool State)
{
  Blynk.virtualWrite(AUTO_MAN,State);
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
  StepToMove = (PosDesire - MotorPosition) * Side;
  
  return;
}

int Get_Side_Motor()
{
  return Side;
}

double Get_Step_To_Move()
{
  return StepToMove;
}

void Set_Niv_Batterie(double NivBat,String ColorCode,int Notifiy)
{
  Blynk_Virtual_Write(NIV_BAT,NivBat);

  Blynk.setProperty(NIV_BAT,"color",ColorCode);
  if(Notifiy == 1)
  {
    Blynk.notify("La batterie du store est basse");
  }
  else if(Notifiy == 2)
  {
    Blynk.notify("La batterie est déconnecté");
  }
  
  return;
}

double Get_Niv_Batterie()
{
  return NiveauBatteriePourcent;
}

String Get_Real_Time()
{
  int houres = hour();
  int minutes = minute();
  int tmp;
  houres = houres * 3600;
  minutes = minutes * 60;
  tmp = houres + minutes;
  RealTime = (String)tmp;
  return RealTime;
}

void Set_Motor_Wait()
{
  WaitMotor = true;
  WaitMotorTime = millis();
  return;
}

void Reset_Motor_Wait()
{
  WaitMotor = false;
  return;
}