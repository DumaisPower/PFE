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
#include "ia.h"
#include <stdio.h>

TaskHandle_t TaskIA;

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	BarrierSensor;
extern SemaphoreHandle_t	SemaphoreComz;
extern SemaphoreHandle_t	SemaphoreIA;
extern SemaphoreHandle_t	SemaphoreSensor;

/*********************************Variable***********************/
double InsideTempIRIA ;
double ObjectTempIRIA ;
double OutsideTempIA ;
int TemperatureDesireIA;
double MotorPosIA;
double SunLevelIA;
bool DayNight;
bool CurrentState;
bool WatingChangeState = false;
bool WaitingDayNight = false;
double NiveauBatterieTmp;
double NiveauBatteriePourcentTmp;
int Notifiy = 0;
String BatColor;
String RealTimeIA;
String HeureOuvertureIA;
String HeureFermetureIA;

void IA_Init()
{
    xTaskCreatePinnedToCore(
                    Task_IA,                 /* Task function. */
                    "TaskIA",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */
    
  delay(500); 

  console_Debug("IA setup");

  pinMode(36,INPUT);

  return;
}


void Task_IA(void * parameter)
{
  
  console_Debug("Task IA Start");

  delay(1000);
  
  xSemaphoreGive(BarrierSensor); //retourne a sensor

  //run task motor
  while(true)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreIA, portMAX_DELAY);

    console_Debug("Doing IA Task");

    Update_Local_Variable();

    Change_Mode();//if too long in manual mode 

    Automatique_Position();

    Set_Bat_Niv_Color();

    Heure_Ouverture();

    Heure_Fermeture();

    console_Debug("Exit IA Task");
    
    xSemaphoreGive(SemaphoreComz); //retourne a comz

  }
  vTaskDelete( NULL );
}

void Change_Mode()
{
    
    if(CurrentState == STATE_MANUEL and not WatingChangeState)
    {
        if(DayNight == DAY)
        {
            WaitingDayNight = NIGHT;
        }
        else 
        {
            WaitingDayNight = DAY;
        }
        WatingChangeState = true;
    }

    if (CurrentState == STATE_AUTO or DayNight == WaitingDayNight)
    {
        WatingChangeState = false;
        Set_State_Auto_Manuel(STATE_AUTO);
    }
    return;
}

void Automatique_Position()
{
  if(CurrentState == STATE_AUTO)
  {
    //condition pour ouvrir le store
    if(((InsideTempIRIA - OutsideTempIA >= 1) or (InsideTempIRIA <= TemperatureDesireIA ))  and (SunLevelIA >= WINDOWSUN))
    {
      if(MotorPosIA != 0)
      {
      Set_Position_Desire(0);
      Set_Motor_Change();
      }
    }
    //condition pour fermer le store
    else if((((OutsideTempIA - InsideTempIRIA >= 1) or (InsideTempIRIA >= TemperatureDesireIA )) and (SunLevelIA >= WINDOWSUN)) or ((ObjectTempIRIA >= InsideTempIRIA) and (OutsideTempIA - InsideTempIRIA >= 4)))
    {
      if(MotorPosIA != 100)
      {
      Set_Position_Desire(100);
      Set_Motor_Change();
      }
    }
    //si la temperature est atteinte et neutre agit selon le moment de la journée
    else
    {
      if((DayNight == DAY) and (MotorPosIA != 20))
      {
        Set_Position_Desire(20);
        Set_Motor_Change();
      }
      else if((DayNight == NIGHT) and (MotorPosIA != 100))
      {
        Set_Position_Desire(100);
        Set_Motor_Change();
      }
    }
  }
  return;
}

void Update_Local_Variable()
{

  InsideTempIRIA = Get_Inside_Temp_IR();

  ObjectTempIRIA = Get_Object_Temp_IR();

  OutsideTempIA = Get_Outside_Temp();

  TemperatureDesireIA = Get_Temp_Desirer();

  SunLevelIA = Get_Sun();

  CurrentState = Get_State_Auto_Manuel();

  Update_Day_Night();

  Blynk_Run();

  Read_Niv_Bat();

  Bat_To_Pourcentage();

  HeureOuvertureIA = Get_Heure_Ouverture();

  HeureFermetureIA = Get_Heure_Fermeture();

  RealTimeIA = Get_Real_Time();

  MotorPosIA = Get_Motor_Pos();

  return;
}

void Update_Day_Night()
{
    if(SunLevelIA >= DAYLIGHTMIN)
    {
        DayNight = DAY;
    }
    else 
    {
        DayNight = NIGHT;
    }
    return ;
}

void Bat_To_Pourcentage()
{
  
  if(NiveauBatterieTmp >= 3.9)
  {
    NiveauBatteriePourcentTmp =  41.6667 * NiveauBatterieTmp - 75;
  }
  else if((NiveauBatterieTmp < 3.9) && (NiveauBatterieTmp > 3.6))
  {
    NiveauBatteriePourcentTmp = 250 * NiveauBatterieTmp - 887.5;
  }
  else         
  {
    NiveauBatteriePourcentTmp = 16.129 * NiveauBatterieTmp - 45.5645;
  }
  NiveauBatteriePourcentTmp = (int) NiveauBatteriePourcentTmp;

  Blynk_Virtual_Write(NIV_BAT, NiveauBatteriePourcentTmp);
  return;
}

void Read_Niv_Bat()
{
    //read tension on bat
    NiveauBatterieTmp = analogRead(36);
    NiveauBatterieTmp =  NiveauBatterieTmp / 600;
    return;
}

void Set_Bat_Niv_Color()
{
    if(NiveauBatteriePourcentTmp>80)
   {
        BatColor =  "#23C48E"; //green
        Notifiy = 0;
   }
   else if(NiveauBatteriePourcentTmp <= 80 && NiveauBatteriePourcentTmp > 55)
   {
        BatColor = "#d69e04"; //orange
        Notifiy = 0;
   }
   else if(NiveauBatteriePourcentTmp <= 55 && NiveauBatteriePourcentTmp > 30)
   {
        BatColor = "#ED9D00"; //yellow
        Notifiy = 0;
   }
   else if(NiveauBatteriePourcentTmp <= -10)
   {
      BatColor =  "#FF0000"; //red flash
      //Notifiy  = 2;
   }
   else
   {
      BatColor =  "#D3435C"; //red 
      //Notifiy = 1;
   }
    Set_Niv_Batterie(NiveauBatteriePourcentTmp,BatColor,Notifiy);
    return;
}

void Heure_Ouverture()
{
  int RealTimeInt =  (atoi(RealTimeIA.c_str())) /100;
  int HeureOuvertureInt = (atoi(HeureOuvertureIA.c_str()))/100;

  if(RealTimeInt == HeureOuvertureInt)
  {
    Set_Position_Desire(100);
    Set_Motor_Change();
  }
  return;
}

void Heure_Fermeture()
{
  int RealTimeInt = (atoi(RealTimeIA.c_str())) /100;
  int HeureFermetureInt = (atoi(HeureFermetureIA.c_str()))/100;

  if(RealTimeInt == HeureFermetureInt)
  {
    Set_Position_Desire(0);
    Set_Motor_Change();
  }
  return;
}
