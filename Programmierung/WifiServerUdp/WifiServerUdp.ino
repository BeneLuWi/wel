/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>  
#include <WiFiUdp.h>

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

// Variables will change:
int buttonState = HIGH;             // the current reading from the input pin: HIGH = pushed, Low = pushed
int lastButtonState = HIGH;   // the previous reading from the input pin
int pushed = 0;
int firstPush = 1;
int i = 0;
int packetSize = 0;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long intervalDuration = 0;    // duration of push
unsigned long intervalStart = 0;
unsigned long durations[65];
unsigned long inputEnd = 3000;

// Code to unlock Door
int codeLength = 0;
char code[64] = {'0','3','0','1','0','1','0','2','0','7','0','2','0'};

// Constants
const int resetButton = D1;
const int inputButton = D2;
const int relayPin = 13;
const int resetLED = D0;

// Wifi Credentials
const char *ssid = "wel";
const char *password = "password";

// UDP Credentials
WiFiUDP udp;
unsigned int localUdpPort = 4210;


WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  pinMode(relayPin, OUTPUT);

  pinMode(resetLED, OUTPUT);
  digitalWrite(resetLED, LOW);

  pinMode(resetButton, INPUT_PULLUP );
  pinMode(inputButton, INPUT_PULLUP );

  
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  
  Serial.println("Server started");

  if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  udp.begin(localUdpPort);

}

void evalCode(const char * message){

  int correct = 1;
  for(int n = 0; n < codeLength; n++){
      if ((abs((code[n] - '0') - (message[n] - '0'))) > 1){
        correct = 0;
        break;
      }
      
  }
  delay(100);
  if(!udp.beginPacket(udp.remoteIP(), udp.remotePort())) Serial.println("can't Start Package");
  switch(correct){
    case 1:{
      char answer[] = "Code accepted";
      udp.write(answer, sizeof answer);
      if(!udp.endPacket()) Serial.println("Error sending answer");
      //digitalWrite(ledOpen, HIGH);
      //digitalWrite(relayPin, HIGH);
      delay(3000);
      //digitalWrite(ledOpen, LOW);
      //digitalWrite(relayPin, LOW);
      break;
    }
    default:{
      char answer[] = "Code declined";
      udp.write(answer, sizeof answer);
      if(!udp.endPacket()) Serial.println("Error sending answer");
      //digitalWrite(ledClosed, HIGH);
      delay(3000);
      //digitalWrite(ledClosed, LOW);
      break;
    }
  }
}


void setCode(){
  Serial.println("Reseting code");
  digitalWrite(resetLED, HIGH);
  // reset vars to be ready for new input
  lastButtonState = HIGH;
  buttonState = HIGH;
  i = 0;
  firstPush = 1;
  memset(durations, 0, sizeof(durations));   
  memset(code, '0', sizeof(code));   
  codeLength = 0;
    
  while(true){
    if(checkButtonState()) break;
    delay(1);
  }
  Serial.println(code);
  Serial.println(codeLength);
  
  digitalWrite(resetLED, LOW);
}

int checkButtonState() {
// based on https://www.arduino.cc/en/Tutorial/Debounce
// LOW when the button is being pushed, HIGH when released
// read the state of the switch into a local variable:
  int reading = digitalRead(inputButton);
  
  int codeCompleted = 0;
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  
  if ( millis() - lastDebounceTime > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    
    if( millis() - lastDebounceTime > inputEnd && !firstPush){
      createOutput(codeLength);
      return 1;
    }

    // if the button state has changed:
    if (reading != buttonState) {
      codeLength++;
      buttonState = reading;
      if(buttonState == LOW){

        // If first push: clear the display and set the cursor to the upper left
        if(i == 0){
          // lcd.clear();
        }
        
        // if released-Interval ends
        if(firstPush == 0) {
          codeCompleted = addDuration(millis() - intervalStart); //intervalDuration
        }
        
        firstPush = 0;
        pushed = 1;
        
        // pushed-Interval starts
        intervalStart = millis();
        
        // go 1 to the right with output 
        // lcd.setCursor(i, 0);
        
      }
      if(buttonState == HIGH && pushed){
        intervalDuration = millis() - intervalStart;
        pushed = 0;
  
        // released-Interval starts
        if(i < codeLength - 1) intervalStart = millis();
  
        codeCompleted = addDuration(intervalDuration);

      }
    }
  }
  // displayCode(reading);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  return codeCompleted;
}

int addDuration(unsigned long interDuration){
// returns 1 if code is complete, 0 if not

    durations[i++] = interDuration;
    if(i < codeLength) return 0;
    
}
void createOutput(int len){
    int p;
    for(int j = 0; j < len - 1; j++){
      p = durations[j]/ 125;
      if(p > 7) p = 7;
      sprintf(&code[j], "%d", p);
    }

    // Counting one time too much before
    codeLength--;
    
    // reset vars to be ready for new input
    i = 0;
    firstPush = 1;
    memset(durations, 0, sizeof(durations));      
}

void getMessage(){
  packetSize = udp.parsePacket();
  if (packetSize){
      Serial.println(""); 
      char message[codeLength];
      int len = udp.read(message, packetSize);
      Serial.printf("Message received: length: %d content: %s \n", packetSize, message);
      Serial.println(message);
      evalCode(message);
      Serial.println("Transmission ended");
  }
  else delay(20);
}

void loop() {
  if(digitalRead(resetButton) == LOW) setCode();
  getMessage();
}
