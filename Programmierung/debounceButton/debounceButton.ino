// constants won't change. They're used here to set pin numbers:
const int buttonPin = 16;    // the number of the pushbutton pin
const int pushes = 5;        // how many pushes are accounted

// Variables will change:
int buttonState;             // the current reading from the input pin: HIGH = pushed, Low = pushed
int lastButtonState = LOW;   // the previous reading from the input pin
int pushed = 0;
int i = 0;

//char output[5*sizeof(unsigned long) + 4];

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long pushedDuration = 0;    // duration of push
unsigned long pushedStart = 0;
unsigned long durations[pushes];


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
        pushed = 1;
        pushedStart = millis();
      }
    }
    if(buttonState == LOW && pushed){
      pushedDuration = millis() - pushedStart;
      pushed = 0;

      if(i < pushes - 1){
        durations[i++] = pushedDuration;
      }
      else if(i == pushes-1){
        durations[i++] = pushedDuration;
        for(int j = 0; j < pushes; j++){
          Serial.println(durations[j]);
        }
        i = 0;
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  
}



void loop() {
  checkButtonState();
}
