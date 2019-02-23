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



#include <esp_bt.h>
#include <BlynkSimpleEsp32.h>
#include <Stepper.h>
#include "soc/rtc.h"
#include "esp32/pm.h"
#include "esp_bt_main.h"


/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1334465b93034f92ad14742fb88eb305";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HONDAGMC";
char pass[] = "18182321yougi";

//define widget
#define LED1 5
#define LED2 10
WidgetTerminal terminal(V0);
WidgetLED led(V2);

//sub program declaration
void stepperMotor();
void blink();
void consoleDebug(String String1ToPrint);

//task handle
TaskHandle_t TaskCom;
TaskHandle_t TaskMot;
TaskHandle_t TaskSen;

//task declaration
void TaskMoteur(void * parameter);
void TaskSensor(void * parameter);
void TaskCommunication(void * parameter);

//constante
const int stepsPerRevolution = 2038;  // change this to fit the number of steps per revolution
const long interval = 1000; 

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 23, 19, 22, 21);

//variable
int ledState = LOW;   
int stepCount = 0;  // number of steps the motor has taken

 
void setup()
{

    //close what we don't use
  esp_bluedroid_disable();
  esp_bt_controller_disable();

  //enable wakup sourcce
  //esp_sleep_enable_timer_wakeup(10000000);
   
  // Set pins modes
  pinMode(25,INPUT);

  //Connect to wifi
  Blynk.begin(auth, ssid, pass);
  delay(500); 

  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println(F("-------------"));
  terminal.println(F("Type 'Marco' and get a reply, or type"));
  terminal.println(F("anything else and get it printed back."));
  terminal.flush();

  delay(500); 

  //initialsation of new task 
  xTaskCreatePinnedToCore(
                    TaskCommunication,          /* Task function. */
                    "TaskCom",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */

  delay(500); 

    xTaskCreatePinnedToCore(
                    TaskMoteur,                 /* Task function. */
                    "TaskMot",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */

  delay(500); 

      xTaskCreatePinnedToCore(
                    TaskSensor,                  /* Task function. */
                    "TaskSen",                  /* String with name of task. */
                    10000,                      /* Stack size in bytes. */
                    NULL,                       /* Parameter passed as input of the task */
                    15,                         /* Priority of the task. */
                    NULL,                       /* task handler. */     
                    1);                         /* Core id. */

  delay(500); 

  //changing cpu speed
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  //initial setup of the blind

}
 
void TaskCommunication(void * parameter)
{
  //setup for comz 
  unsigned long currentMillis;
  unsigned long nextMillis = 0;  
  /* The parameter value is expected to be 1 as 1 is passed in the
  pvParameters value in the call to xTaskCreate() below.*/ 
  //configASSERT( ( ( uint32_t ) parameter ) == 1 );
 
 //run task comz
  while(1)
  {
    currentMillis = millis();
    if (currentMillis  > nextMillis + 7000)
    {
      consoleDebug("test0");
      nextMillis = currentMillis;
    }


    //give ping to blynk and get value if change
    //Blynk.run();

    //if no new value shut down wifi
    //esp_wifi_stop();

    //go to sleep for 10 sec or GPIO press
    //esp_light_sleep_start() ;
    
  }
  vTaskDelete( NULL );

}

void TaskMoteur(void * parameter)
{
  //setup for motor
  myStepper.setSpeed(8); // 8 rpm
  unsigned long currentMillis;
  unsigned long nextMillis = 0;

  /* The parameter value is expected to be 1 as 1 is passed in the
  pvParameters value in the call to xTaskCreate() below.*/ 
 

  //run task motor
  while(1)
  {
    //step.perMotor();

    currentMillis = millis();
    if (currentMillis  > nextMillis + 7000)
    {
      consoleDebug("test1");
      nextMillis = currentMillis;
    }


    //si manuelle attend un mouvement des GPIO/Blynk

      //compte le nombre de step a tournee 

      //met a jour blynk

      //retourne a attendre une nouvelle commande

    //si automatique attend un mouvement des Sensor/IA/GPIO/Blynk

      //compte le nombre de step a tournee 

      //met a jour blynk

      //retourne a attendre une nouvelle commande
  }
  vTaskDelete( NULL );
}

void TaskSensor(void * parameter)
{
  //setup for sensor
  unsigned long currentMillis ;
  unsigned long nextMillis = 0;  
  //run task sensor
  while(1)
  {
    currentMillis = millis();
    if (currentMillis  > nextMillis + 7000)
    {
      consoleDebug("test2");
      nextMillis = currentMillis;
    }

   
    //A chaque X wake up 

    //update inside temp

    //update outside temp

    //update sun on the window
  }
  vTaskDelete( NULL );
}

void blink()
{
  //use de millis function because non blocking function and when cpu freq is change delay() suck
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;  

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
  
  
  myStepper.step(2038); // do 2038 steps -- corresponds to one revolution in one minute
   // wait for one second
  // do 2038 steps in the other direction with faster 
  // wait for one second

  return;

}

void consoleDebug(String String1ToPrint)
{
  Blynk.run();
  terminal.print(String1ToPrint);
  terminal.flush();
  
  return;
}

void loop() 
{
  

}