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

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Stepper.h>
#include "soc/rtc.h"
#include "esp32/pm.h"



/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

//task handle
TaskHandle_t TaskCom;


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1334465b93034f92ad14742fb88eb305";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HONDAGMC";
char pass[] = "18182321yougi";

//define
#define LED1 5
#define LED2 10
WidgetLED led(V2);

//sub program
void stepperMotor();
void blink();
void consoleDebug(String String1ToPrint,String String2ToPrint);
void TaskCommunication(void * parameter);

//constante
const int stepsPerRevolution = 2038;  // change this to fit the number of steps per revolution
const long interval = 1000; 

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 23, 19, 22, 21);

//variable
int ledState = LOW;   
int stepCount = 0;  // number of steps the motor has taken
unsigned long previousMillis = 0;  
 
void setup() 
{
  //close what we don't use
  esp_bluedroid_disable();
  esp_bt_controller_disable();

  //enable wakup sourcce
  esp_sleep_enable_timer_wakeup();
   
  // Set pins modes
  pinMode(25,INPUT);

  //Connect to wifi
  Blynk.begin(auth, ssid, pass);

  //initialsation of new task 
  xTaskCreatePinnedToCore(TaskCommunication, "TaskCom", 1000, NULL, 2, &TaskCom, 1);
  xTaskCreatePinnedToCore(TaskMoteur, "TaskMot", 1000, NULL, 1, &TaskMot, 1);
  xTaskCreatePinnedToCore(TaskSensor, "TaskSen", 1000, NULL, 1, &TaskSen, 1);

  //changing cpu speed
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  //initial setup of the blind
  
}
 
void loop() 
{
  //stepperMotor();

  //get core id running on
  //int test = xPortGetCoreID(); 

  blink();
}

void TaskCommunication(void * parameter)
{
  //setup for comz 

  //run task comz
  for(;;)
  {
    //give ping to blynk and get value if change
    Blynk.run();

    //if no new value shut down wifi
    esp_wifi_stop();

    //go to sleep for 10 sec or GPIO press
    esp_light_sleep_start() 
    
  }
}

void TaskMoteur(void * parameter)
{
  //setup for motor

  //run task motor
  for(;;)
  {
    //si manuelle attend un mouvement des GPIO/Blynk

      //compte le nombre de step a tournee 

      //met a jour blynk

      //retourne a attendre une nouvelle commande

    //si automatique attend un mouvement des Sensor/IA/GPIO/Blynk

      //compte le nombre de step a tournee 

      //met a jour blynk

      //retourne a attendre une nouvelle commande
  }
}

void TaskSensor(void * parameter)
{
  //setup for sensor

  //run task sensor
  for(;;)
  {
    //A chaque X wake up 

    //update inside temp

    //update outside temp

    //update sun on the window
  }
}

void blink()
{
  //use de millis function because non blocking function and when cpu freq is change delay() suck
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) 
    {
      ledState = HIGH;
      led.on() ;
    }
    else
    {
      ledState = LOW;
      led.off() ;
    }
  }

  return;
}

void stepperMotor()
{
  // read the sensor value:


  // map it to a range from 0 to 100:
  //int motorSpeed = map(500, 0, 1023, 0, 100);
  
  myStepper.setSpeed(6); // 1 rpm
  myStepper.step(2038); // do 2038 steps -- corresponds to one revolution in one minute
  delay(4000); // wait for one second
  myStepper.setSpeed(4); // 6 rpm
  myStepper.step(-2038); // do 2038 steps in the other direction with faster 
  delay(4000); // wait for one second

  return;

}

void consoleDebug(String String1ToPrint,String String2ToPrint)
{
  Blynk.virtualWrite(V0, String1ToPrint);
  Blynk.virtualWrite(V1, String2ToPrint);

  return;
}