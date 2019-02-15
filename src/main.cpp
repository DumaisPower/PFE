#include <Arduino.h>

#define LED1 9
#define LED2 10
 
void setup() 
{
  // Set pin mode
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
}
 
void loop() 
{
  delay(500);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,LOW);
  delay(500);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,HIGH);
}