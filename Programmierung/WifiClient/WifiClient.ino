// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <string.h>

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

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 16;    // the number of the pushbutton pin

// Variables will change:
int buttonState;             // the current reading from the input pin: HIGH = pushed, Low = pushed
int lastButtonState = LOW;   // the previous reading from the input pin
int pushed = 0;
int firstPush = 1;
int i = 0;
int pushes = 7;              // how many pushes are accounted
const int codeLength = 13;


// lcd Ouputdevice
// old example /wno I2C LiquidCrystal lcd(D1, D2, D4, D5, D6, D7);  // RS, E, D4, D5, D6, D7

// LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Different States of a Character
 byte customChar[8][8] = {
  {0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x1F},
  {0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x1F,  0x1F},
  {0x00,  0x00,  0x00,  0x00,  0x00,  0x1F,  0x1F,  0x1F},
  {0x00,  0x00,  0x00,  0x00,  0x1F,  0x1F,  0x1F,  0x1F},
  {0x00,  0x00,  0x00,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F},
  {0x00,  0x00,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F},
  {0x00,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F},
  {0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F},
};

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long intervalDuration = 0;    // duration of push
unsigned long intervalStart = 0;
unsigned long durations[14];

// AP Wi-Fi credentials
const char* ssid = "wel";
const char* password = "password";

int counter = 0;

WiFiClient client;

const char* host = "192.168.4.1";

/**************
 * 
 * Start Code
 * 
 *************/
void setup() {
 /* Wire.begin(2,0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight 
  lcd.print(" Hello Makers "); // Start Printin

  for (int j = 0; j < 7; j++){
    lcd.createChar(j, customChar[j]);
  }
  */
  Serial.begin(115200);
  Serial.println("");
  
  // Establish connection to dooropener
  ESP.eraseConfig();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    if(counter > 20){
       Serial.println("- can't connect, going to sleep");
    }
    delay(500);
    Serial.print(".");
    counter++;
  }

  
  Serial.println("- wifi connected");
  connectToHost();  
  pinMode(buttonPin, INPUT);
  
}

void checkButtonState() {
// based on https://www.arduino.cc/en/Tutorial/Debounce
// read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

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

      sendCode();

    }
}

void connectToHost(){
    if (client.connect(host, 80)){
    Serial.println("Connected to Host");
  }
  else{
    Serial.println("Connection to host failed");
  }
}
 
void sendCode(){

   while(!client.connected()){
    connectToHost();
    delay(50);
   }
   
   if(client.connected()){
      // Create Output (code)
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

      // Send code to the server and wait for answer
		//
      // Test : char code1[codeLength+1] = {'0','7','0','4','0','4','0','4','0','4','0','5','0'};
      //        code1[codeLength] = '\0';
      Serial.println(code);
      Serial.print(client.println(code));
      Serial.println(" Characters sent ");
      
      Serial.print("Waiting for answer:");
      int j=0;
      code[0] = '\0';

      // Wait 10 seconds for server's response
      while((!client.available()) && (j<1500)){
        delay(10);
        if(j % 100 == 0){
          Serial.print(".");
	     }
        j++;
      }
      Serial.println("");

      if(!client.available()){
        Serial.println("Server not responding");
        client.flush(); 
        client.stop();
      }else{
        Serial.println(client.readStringUntil('\n'));
        client.flush(); 
      }
      
   }else{
      Serial.println("Connection to host lost...trying again");
      sendCode();
   }

}

// Create Output for LCD
// Check the duration
void displayCode(int current){
  // get one more row for each 125ms that remain in the same state  
  int p = (millis() - intervalStart) / 125;
  if(p > 7) p = 7;
  Serial.println(p);
  // lcd.write(p);
}


void loop() {
  checkButtonState();
}
