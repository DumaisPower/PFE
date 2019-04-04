#ifndef COMZ_H_   /* Include guard */
#define COMZ_H_

/***************************Library***************/
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "WString.h"
#include "esp32/pm.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include <ArduinoJson.h> 


/*************************Blynk Comz ***********************/
#define BLYNK_PRINT Serial
#define CITY_ID_CHAR 7

/*************************Run mode********************************/
#define STATE_MANUEL 0
#define STATE_AUTO 1
/*************************Default value******************************/
#define DEFAULT_LENGHT 3
#define DEFAULT_TEMP_DESIRE 21
#define DEFAULT_CITY_ID "6077243"

/**************************Blynk Widget pin***********************/
#define TERMINAL 0
#define TEMP_INT 1
#define TEMP_EXT 2
#define NIV_STORE_MAN 3
#define AUTO_MAN 4
#define NIV_BAT 5
#define WEBHOOK 6
#define CITY_ID 7
#define HAUTEUR_FENETRE 8
#define HEURE_OUVERTURE 9
#define HEURE_FERMETURE 10
#define TEMP_DESIRE 11
#define NIV_SUN 12

#define SENSOR_REFRESH_MILISEC 20000 //900000 = 15 min

//tâche principal
void Task_Communication(void * parameter);

/*************************Initialisation des communcation****************************/
//Creation de la tâche comz
void Comz_Init();

//Setup des comz
void Comz_Setup();

/**************************Terminal Function******************************************/
//Print une erreur dans une fonction
void console_Err(esp_err_t err, String StringToPrint);

//Efface le terminal
void Blynk_Clear_Terminal();

//Ensure that data was sent out of device
void Blynk_Flush_Terminal();

//print string to terminal
void Blynk_Print_Terminal(String StringToPrint);

//debug string to terminal
void console_Debug(String StringToPrint);

//debug integer to terminal
void console_Debug_Int(int IntToPrint);

//debug double to terminal
void console_Debug_Double(double DoubleToPrint);

String Get_Real_Time();

/***************************Blynk Common fonction********************/
//ping blynk server
void Blynk_Run();

//send a value to a widget in blynk
void Blynk_Virtual_Write(int pin, double Value);

//send all sensor update to blynk
void Update_Blynk_Sensor();

//send motor position to blynk
void Update_Blynk_Sensor();

//shut down everything and wake up after 7 sec
void Go_To_Sleep();

//timer that update sensor when done
bool Timer_Sensor(int MiliSeconde);

//return current state of the module
bool Get_State_Auto_Manuel();

//set state auto manuel
void Set_State_Auto_Manuel(bool STATE);

//return l'heure d'ouverture des store
String Get_Heure_Ouverture();

//return l'heure de fermeture des store
String Get_Heure_Fermeture();

void Set_Niv_Batterie(double NivBat,String ColorCode,bool Notifiy);

double Get_Niv_Batterie();

/*************************Get/Set Motor variable************************/
//set max position in step for the blind
void Set_Max_Position(double Feet);

//put the slider bar to the real postion of the motor
void Update_Blynk_Motor_Pos();

//put motor change to true to advertise that the position need to change
void Set_Motor_Change();

//put motor change to false to stop moving the motor
void Reset_Motor_Change();

//set the position desire of the store
void Set_Position_Desire(int Pourcentage);

//get the value of the desire position
double Get_Position_Desire();

//set the new actual position of the motor 
void Set_Motor_Pos(double NewPosition);

//return the motor actual position
double Get_Motor_Pos();

//convert motor step to percentage
double Step_To_Percentage(double step);

//convert percentage to motor step 
double Percentage_To_Step(double percentage);

//calculate how many step the motor as to turn
void Set_Step_To_Move(double PosDesire);

//return the value of the turn to do
double Get_Step_To_Move();


/*************************Get/Set Sensor variable************************/
//return the inside temperature get by the analog sensor
double Get_Inside_Temp_Analog();

//set the inside temperature get by the analog sensor
void Set_Inside_Temp_Analog(double NewTempAnalog);

//return the inside temperature get by the Infrared sensor
double  Get_Inside_Temp_IR();

//set the inside temperature get by the infrared sensor
void  Set_Inside_Temp_IR(double NewTempIR);

//return the outside temperature get by the json
double  Get_Outside_Temp();

//set the outside temperature get by the json
void  Set_Outside_Temp();

//return the object temperature get by the Infrared sensor
double Get_Object_Temp_IR();

//set the inside temperature get by the infrared sensor
void Set_Object_Temp_IR(double NewObjectIR);

//return the value of the light level 0-5000 aprox
double Get_Sun();

//set the sun level get by the photoresistor sensor
void Set_Sun(double NewSunLevel);

#endif // COMZ_H_
