/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/*
D0   = 16;
D1   = 5;
D2   = 4;
D3   = 0;
D4   = 2;
D5   = 14;
D6   = 12;
D7   = 13;
D8   = 15;
D9   = 3;
D10  = 1;
 */

// Code to unlock Door
const int codeLength = 13;
char code[codeLength] = {'0','7','0','4','0','4','0','4','0','4','0','5','0'};

// Constants
const int ledOpen = 5;
const int ledClosed = 4;

// Wifi Credentials
const char *ssid = "wel";
const char *password = "password";

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(ledClosed, OUTPUT);
  pinMode(ledOpen, OUTPUT);
  
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  
  Serial.println("Server started");
}

void readCode(){
  
}

void evalCode(const char * message){

  if(strcmp(message, code) == 0){
    Serial.println("Code accepted");
    client.println("Code accepted");
    digitalWrite(ledOpen, HIGH);
    delay(3000);
    digitalWrite(ledOpen, LOW);

  }else{
    Serial.println("Code declined");
    client.println("Code declined");
    digitalWrite(ledClosed, HIGH);
    delay(3000);
    digitalWrite(ledClosed, LOW);
    
  }
  Serial.println("Answer sent");
}

void setCode(){
  
}

void loop() {

  client = server.available();
   
  if(client){
    int j = 0;
    Serial.println("Client connected");
    Serial.print("Waiting for message:");
    while (client){
      if(client.available() != 0){
        Serial.println("");
        Serial.print("Message received: "); 

        char message[codeLength];
        client.readStringUntil('\n').toCharArray(message, codeLength);
        
        client.flush();
        
        Serial.println(message);
        delay(1000);

        evalCode(message);
        
        delay(200);
        client.stop();
        Serial.println("Transmission ended");
      }else if(j < 20){
        Serial.print(".");
        delay(500);
        j++;
      }
    }
  }
}
