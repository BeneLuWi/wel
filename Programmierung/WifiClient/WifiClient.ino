#include <LiquidCrystal.h>

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 16;    // the number of the pushbutton pin
const int pushes = 5;        // how many pushes are accounted


// Variables will change:
int buttonState;             // the current reading from the input pin: HIGH = pushed, Low = pushed
int lastButtonState = LOW;   // the previous reading from the input pin
int pushed = 0;
int firstPush = 1;
int i = 0;

// lcd Ouputdevice
LiquidCrystal lcd(D1, D2, D4, D5, D6, D7);  // RS, E, D4, D5, D6, D7

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
unsigned long durations[2 * pushes];


void setup() {
  lcd.begin(16,2);
  for (int j = 0; j < 7; j++){
    lcd.createChar(j, customChar[j]);
  }
  
  pinMode(buttonPin, INPUT);
  
  Serial.begin(115200);
  Serial.println("Init: Button Low");

  lcd.setCursor(0,0); 
  //lcd.print("Hello Aaron");
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
          lcd.clear();
        }
        
        // if LOW-Interval ends
        if(firstPush == 0) addDuration(millis() - intervalStart); //intervalDuration
        
        firstPush = 0;
        pushed = 1;
        
        // HIGH-Interval starts
        intervalStart = millis();
        
        //go 1 to the right with output 
        lcd.setCursor(i, 0);
        
      }
    }
    if(buttonState == LOW && pushed){
      intervalDuration = millis() - intervalStart;
      pushed = 0;
      // LOW-Interval starts
      intervalStart = millis();

      addDuration(intervalDuration);

    }
  }

  displayCode(reading);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  
}

void addDuration(unsigned long int interDuration){

    if(i < 2*pushes - 1){
      durations[i++] = interDuration;
    }
    else if(i == 2*pushes-1){
      durations[i++] = interDuration;
      for(int j = 0; j < 2*pushes; j++){
        Serial.println(durations[j]);
      }
      i = 0;
    }
}

// Check whether entered code opens door    
void checkCode(){
   
}

// Create Output for LCD
// Check the duration
void displayCode(int current){
  // get one more row for each 125ms that remain in the same state  
  int p = (millis() - intervalStart) / 125;
  if(p > 7) p = 7;
  Serial.println(p);
  lcd.write(p);
}


void loop() {
  checkButtonState();
}
