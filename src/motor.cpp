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
#include "motor.h"

DRV8834 stepper(MOTOR_STEPS, DIR, STEP, SLEEP/*, M0, M1*/);

TaskHandle_t TaskMot;

extern SemaphoreHandle_t	BarrierMotor;
extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;
extern SemaphoreHandle_t	SemaphoreComz;

unsigned long currentMillisMotor;
unsigned long nextMillisMotor = 0;
double TimeToMove;

double MotorPositionTmp = 0;
double PositionDesireTmp=0;
double StepToMoveTmp = 0;
int SideMotor;
double MotorPositionOffset;


void Motor_Init()
{
    xTaskCreatePinnedToCore(
                    Task_Moteur,                 /* Task function. */
                    "TaskMot",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */
    
  delay(500); 

  stepper.begin(RPM);

  Motor_OFF();

  Position_Init();

  Update_Blynk_Motor_Pos();

  return;
}

void Position_Init()
{
  //change speed too limit stall
  stepper.begin(25);

  MotorPositionOffset = MOTOR_STEPS * MOTOR_POS_OFFSET ;

  Motor_ON();

  StepToMoveTmp = -1 *  Get_Max_Position() * Get_Side();

  Motor_Turn();

  StepToMoveTmp =   MotorPositionOffset * Get_Side();

  Motor_Turn();

  Motor_OFF();

  Set_Motor_Pos(0);

  //change back to 20 rpm
  stepper.begin(RPM);

  return;

}


void Task_Moteur(void * parameter)
{
  
  console_Debug("Task Motor Start");

  delay(1000);

  xSemaphoreTake(BarrierMotor, portMAX_DELAY);
  xSemaphoreGive(BarrierComz);
  //run task motor
  while(true)
  {

    //wating for a command
    xSemaphoreTake(SemaphoreMotor, portMAX_DELAY);

    console_Debug("Doing Motor Task");

    PositionDesireTmp = Get_Position_Desire();
    MotorPositionTmp = Get_Motor_Pos();
    StepToMoveTmp = Get_Step_To_Move();
    Time_To_Move(StepToMoveTmp);

    Motor_ON();

    Motor_Turn();
    
    Motor_OFF();

    Set_Motor_Pos(PositionDesireTmp);

    console_Debug("Exit Motor Task");

    xSemaphoreGive(SemaphoreComz); //retourne a comz

  }
  vTaskDelete( NULL );
}

bool Timer_Motor(int MiliSeconde)
{
    currentMillisMotor = millis();
    if (currentMillisMotor  > nextMillisMotor + MiliSeconde)
    {     
      nextMillisMotor = currentMillisMotor;
      return true;
    }
    return false;
}

void Motor_OFF()
{
digitalWrite(SLEEP, LOW);
return;
}

void Motor_ON()
{
digitalWrite(SLEEP, HIGH);
return;
}

void Time_To_Move(double Step)
{
  TimeToMove = (((Step / MOTOR_STEPS) * (RPM / 60))*1000);
  return ;
}

void Motor_Turn()
{

  while(StepToMoveTmp != 0)
  {
    if(StepToMoveTmp > 2400)
    {
      stepper.move(2400);
      StepToMoveTmp = StepToMoveTmp - 2400;
      Blynk_Run();
    }
    else if(StepToMoveTmp < -2400)
    {
      stepper.move(-2400);
      StepToMoveTmp = StepToMoveTmp + 2400;
      Blynk_Run();
    }
    else
    {
      stepper.move(StepToMoveTmp);
      StepToMoveTmp = 0;
    }
  }
}


