#include <LiquidCrystal.h>


// constants won't change. They're used here to set pin numbers:
const int buttonPin = 5;    // the number of the pushbutton pin
const int pushes = 5;        // how many pushes are accounted

// Variables will change:
int buttonState;             // the current reading from the input pin: HIGH = pushed, Low = pushed
int lastButtonState = LOW;   // the previous reading from the input pin
int pushed = 0;
int firstPush = 1;
int i = 0;

//char output[5*sizeof(unsigned long) + 4];

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long intervalDuration = 0;    // duration of push
unsigned long intervalStart = 0;
unsigned long durations[2 * pushes];


void setup() {
  pinMode(buttonPin, INPUT);
  
  Serial.begin(115200);
  Serial.println("Init: Button Low");
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
        
        //LOW-Interval ends
        if(firstPush == 0) addDuration(millis() - intervalStart); //intervalDuration
        
        firstPush = 0;
        pushed = 1;
        // HIGH-Interval starts
        intervalStart = millis();
        
        //TODO go 1 to the right with output 
        
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

void checkCode(){
   //Check whether entered code opens door
    
}

void displayCode(){
  //Create Output for LCD
  //Check the duration
  
}


void loop() {
  checkButtonState();
}
