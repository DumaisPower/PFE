#ifndef MOTOR_H_   /* Include guard */
#define MOTOR_H_

#include "freertos/FreeRTOS.h"
#include "comz.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 400
#define MICROSTEPS 1

// Target RPM for cruise speed
#define RPM 60

#define DIR 22
#define STEP 21
#define SLEEP 19 

#include "DRV8834.h"
#define M0 10
#define M1 11

void Motor_Init();


#endif // MOTOR_H_