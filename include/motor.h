#ifndef MOTOR_H_   /* Include guard */
#define MOTOR_H_

/***************************Library***************/
#include "freertos/FreeRTOS.h"
#include "comz.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 800
#define MICROSTEPS 1

// Target RPM for cruise speed
#define RPM 100

#define DIR 22
#define STEP 21
#define SLEEP 19 
#define CURRENT

#include "DRV8834.h"
#define M0 10
#define M1 11

#define DELTA_CURENT_MAX 0.1
#define MOTOR_POS_OFFSET 6400
#define MOTOR_POS_MAX

void Task_Moteur(void * parameter);

void Motor_Init();
void Motor_Setup();
void Position_Init();
float Get_Current();
bool Timer_Motor(int MiliSeconde);
void Motor_OFF();
void Motor_ON();
void Time_To_Move(double Step);

#endif // MOTOR_H_