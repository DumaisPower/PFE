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


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "1334465b93034f92ad14742fb88eb305";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "iPhone de Dany";
char pass[] = "12345678";

void Task_Communication(void * parameter);

TaskHandle_t TaskCom;

extern SemaphoreHandle_t	BarrierComz;
extern SemaphoreHandle_t	SemaphoreMotor;
extern SemaphoreHandle_t	SemaphoreSensor;

String tamponJson;
bool controle = STATE_MANUEL;
int tempExt;
int setStore;
float tempC = 0;
double niveauBatterie;
float niveauBatteriePourcent;
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

  delay(500); 

  Blynk.virtualWrite(V6, "http://api.openweathermap.org/data/2.5/weather?id=6077243&appid=46f4c65c517848ec7a0bb9282b4e0ec8");


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

      Blynk.virtualWrite(V5, niveauBatterie);

      StaticJsonDocument<2000> doc;
      DeserializationError error = deserializeJson(doc,tamponJson);     
      if (error) 
      {
        console_Debug("deserializeJson() failed with code ");             
        console_Debug(error.c_str());
      }  
 
      String name = doc["name"];                             
      float tempK = doc["main"]["temp"];                   
      float tempC = tempK - 273.15;                        

      Blynk.virtualWrite(V2, tempC);
      
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

BLYNK_WRITE(V6) //WEBHOOK
{
  //Serial.println("WebHook data:");
  tamponJson = param.asStr();

  //Serial.println(param.asStr());
}

BLYNK_WRITE(V2)//TEMPS EXTERIEUR
{
    tempExt = param.asInt();   //pour lire la valeur de la pin V1 et la mettre dans variable pour utiliser pour dany comparaison entre tempin/tempout
    console_Debug("temp exterieur");
    console_Debug_Int(tempExt);
}

BLYNK_WRITE(V3)
{
   setStore = param.asInt();  // met la valeur du slider dans la variable setStore
}

BLYNK_WRITE(V4) // SWITCH MANUEL/AUTO
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
}