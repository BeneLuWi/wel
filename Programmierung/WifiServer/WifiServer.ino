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

// Variables will change:
int buttonState;             // the current reading from the input pin: HIGH = pushed, Low = pushed
int lastButtonState = LOW;   // the previous reading from the input pin
int pushed = 0;
int firstPush = 1;
int i = 0;
int pushes = 7;              // how many pushes are accounted
const int codeLength = 13;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long intervalDuration = 0;    // duration of push
unsigned long intervalStart = 0;
unsigned long durations[14];

// Code to unlock Door
const int codeLength = 13;
char code[codeLength] = {'0','7','0','4','0','4','0','4','0','4','0','5','0'};

// Constants
const int ledOpen = 5;
const int ledClosed = 4;
const int resetButton = 0;
const int inputButton = 2;
const int relayPin = 13;
const int doorLedPin = 12;

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

  pinMode(relayPin, OUTPUT);
  pinMode(doorLedPin, OUTPUT);

	//pinMode(resetButton, INPUT);
	//pinMode(inputButton, INPUT); 

  digitalWrite(doorLedPin, HIGH);
  
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
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledOpen, HIGH);
    delay(3000);
    digitalWrite(relayPin, LOW);
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

void checkButtonState() {
// based on https://www.arduino.cc/en/Tutorial/Debounce
// read the state of the switch into a local variable:
  int reading = digitalRead(inputButton);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      if(buttonState == HIGH){

        // If first push: clear the display and set the cursor to the upper left
        if(i == 0){
          // lcd.clear();
        }
        
        // if LOW-Interval ends
        if(firstPush == 0) addDuration(millis() - intervalStart); //intervalDuration
        
        firstPush = 0;
        pushed = 1;
        
        // HIGH-Interval starts
        intervalStart = millis();
        
        // go 1 to the right with output 
        // lcd.setCursor(i, 0);
        
      }
    }
    if(buttonState == LOW && pushed){
      intervalDuration = millis() - intervalStart;
      pushed = 0;

      // LOW-Interval starts
      if(i < codeLength - 1) intervalStart = millis();

      addDuration(intervalDuration);

    }
  }

  // displayCode(reading);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  
}

void addDuration(unsigned long interDuration){
    if(i < codeLength - 1){
      durations[i++] = interDuration;
    }
    else if(i == codeLength-1){    
      durations[i++] = interDuration;
      char code[codeLength + 1];
      int p;
      for(int j = 0; j < codeLength; j++){
        p = durations[j]/ 125;
        if(p > 7) p = 7;
        sprintf(&code[j], "%d", p);
      }
      code[codeLength] = '\0';

      // reset vars to be ready for new input
      i = 0;
      firstPush = 1;
			memset(durations, 0, sizeof(durations));			


    }
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
        
        // wait until client read input and received and flushed the buffer
        
        while(client.available) delay(200);
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
