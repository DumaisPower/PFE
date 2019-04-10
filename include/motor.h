#ifndef MOTOR_H_   /* Include guard */
#define MOTOR_H_

/***************************Library***************/
#include "freertos/FreeRTOS.h"
#include "comz.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 800
#define MICROSTEPS 1

// Target RPM for cruise speed
#define RPM 20

#define DIR 22
#define STEP 21
#define SLEEP 19 

#include "DRV8834.h"
#define M0 10
#define M1 11

#define MOTOR_POS_OFFSET 4

void Task_Moteur(void * parameter);

void Motor_Init();
void Position_Init();
bool Timer_Motor(int MiliSeconde);
void Motor_OFF();
void Motor_ON();
void Time_To_Move(double Step);
void Motor_Turn();

#endif // MOTOR_H_