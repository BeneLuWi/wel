#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#define LED D0


void setup()
{
 Serial.begin(115200);
 pinMode(LED, OUTPUT); // Port aus Ausgang schalten

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("Sender_01", "abcdef123456!");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("- AP IP address is :");
  Serial.println(myIP);
  
  Serial.println("** SETUP SERVER **");
  Serial.println("- starting server :");
  server.on("/high", ledHigh);
  server.on("/low", ledLow);
  server.begin();
}

void ledHigh(){
  digitalWrite(LED, HIGH); //Led port einschalten
  Serial.println("Led High");
  }

void ledLow(){
   digitalWrite(LED, LOW); //Led port ausschalten
   Serial.println("Led Low");
}


void loop()
{
  server.handleClient();  
}
