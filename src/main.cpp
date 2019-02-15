#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

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