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

//define widget
#define LED1 5
#define LED2 10
WidgetTerminal terminal(V0);
WidgetLED led(V2);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1334465b93034f92ad14742fb88eb305";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HONDAGMC";
char pass[] = "18182321yougi";

void Task_Communication(void * parameter);

TaskHandle_t TaskCom;

extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;
extern SemaphoreHandle_t	SemaphoreSensor;

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


}

void Task_Communication(void * parameter)
{
  //setup for comz
  //terminal.println("Task Comz Start");

  unsigned long currentMillis;
  unsigned long nextMillis = 0;  
  String err;

  /* The parameter value is expected to be 1 as 1 is passed in the
  pvParameters value in the call to xTaskCreate() below.*/ 
  //configASSERT( ( ( uint32_t ) parameter ) == 1 );

 //waiting every task to be setup
  xSemaphoreTake(BarrierComz, portMAX_DELAY);

 //run task comz
  while(1)
  {
    currentMillis = millis();
    if (currentMillis  > nextMillis + 7000)
    {
      console_Debug("test0");
      nextMillis = currentMillis;
      
    }
    xSemaphoreGive(SemaphoreMotor);

    xSemaphoreGive(SemaphoreSensor);
    //give ping to blynk and get value if change

    // //if no new value shut down wifi
    //GoToSleep();

  }
  vTaskDelete( NULL );

}

void Go_To_Sleep()
{
  //consoleDebug("Entering sleep mode");

  //stop wifi connection
  esp_wifi_stop();
  
  //go to sleep for 7 sec or GPIO press
  esp_light_sleep_start();
 
  //restart wifi
  esp_wifi_start();

  //connect to blynk
  //Blynk.begin(auth, ssid, pass);

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
}

void Blynk_Clear_Terminal()
{
  terminal.clear();
}

void Blynk_Print_Terminal(String StringToPrint)
{
  terminal.println(StringToPrint);
}

void Blynk_Flush_Terminal()
{
  terminal.flush();
}

void console_Debug(String StringToPrint)
{
  terminal.println(StringToPrint);
  Blynk.run();
  delay(200);
  terminal.flush();
  
  return;
}

void console_Debug_Int(int IntToPrint)
{
  terminal.println(IntToPrint);
  Blynk.run();
  delay(200);
  terminal.flush();
  
  return;

}

void console_Debug_Double(double DoubleToPrint)
{
  terminal.println(DoubleToPrint);
  Blynk.run();
  delay(200);
  terminal.flush();
  
  return;

}

void Blynk_Virtual_Write(int pin, double Value)
{
  Blynk.virtualWrite(pin, Value);
}

