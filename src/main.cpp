// This is based on the oversized volume knob project of Mikolas Zuza from Prusaprinters.org
// Find the project here: https://blog.prusaprinters.org/3d-print-an-oversized-media-control-volume-knob-arduino-basics_30184/
// Read the full tutorial at prusaprinters.org

#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>
// for mute button w/ led
// arduino-libraries/Keyboard

#define ENCODER_CLK A0 // Change A0 to, for example, A5 if you want to use analog pin 5 instead
#define ENCODER_DT A1
#define ENCODER_SW A2

ClickEncoder *encoder; // variable representing the rotary encoder
int16_t last, value; // variables for current and last rotation value


// ---- for mute button w/ led
#define BUTTON_PIN A8
#define LED_PIN A6

// constants won't change. They're used here to set pin numbers:
const int DEBOUNCE_DELAY = 50;   // the debounce time; increase if the output flickers
// Variables will change:
int lastSteadyState = LOW;       // the previous steady state from the input pin
int lastFlickerableState = LOW;  // the previous flickerable state from the input pin
int currentState;                // the current reading from the input pin
int currentLedState;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled


void timerIsr() {
  encoder->service();
}

void setup() {
  Serial.begin(9600); // Opens the serial connection used for communication with the PC. 
  Consumer.begin(); // Initializes the media keyboard
  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW); // Initializes the rotary encoder with the mentioned pins

  Timer1.initialize(1000); // Initializes the timer, which the rotary encoder uses to detect rotation
  Timer1.attachInterrupt(timerIsr); 
  last = -1;

  // for mute button w/ led
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
} 

void loop() {
  value += encoder->getValue();

  // This part of the code is responsible for the actions when you rotate the encoder
  if (value != last) { // New value is different than the last one, that means to encoder was rotated
    if(last<value) // Detecting the direction of rotation
      Consumer.write(MEDIA_VOLUME_UP); // Replace this line to have a different function when rotating counter-clockwise
      else
      Consumer.write(MEDIA_VOLUME_DOWN); // Replace this line to have a different function when rotating clockwise
    last = value; // Refreshing the "last" varible for the next loop with the current value
    Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    Serial.println(value);
  }

  // This next part handles the rotary encoder BUTTON
  ClickEncoder::Button b = encoder->getButton(); // Asking the button for it's current state
  if (b != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
    //Serial.print("Button: "); 
    //#define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b) {
      case ClickEncoder::Clicked: // Button was clicked once
        Consumer.write(MEDIA_PLAY_PAUSE); // Replace this line to have a different function when clicking button once
      break;      
      
      case ClickEncoder::DoubleClicked: // Button was double clicked
         Consumer.write(MEDIA_NEXT); // Replace this line to have a different function when double-clicking
      break;      
    }
  }

  delay(10); // Wait 10 milliseconds, we definitely don't need to detect the rotary encoder any faster than that
  // The end of the loop() function, it will start again from the beggining (the begginging of the loop function, not the whole document)

  // for mute button w/ led
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);
  currentLedState = digitalRead(LED_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch/button changed, due to noise or pressing:
  if (currentState != lastFlickerableState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    // save the the last flickerable state
    lastFlickerableState = currentState;
  }

  //digitalWrite(LED_PIN, HIGH);
  //delay(1000); 
  //digitalWrite(LED_PIN, LOW);
  //delay(1000); 

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if(lastSteadyState == HIGH && currentState == LOW) {


      if (currentLedState == HIGH) {
        digitalWrite(LED_PIN, LOW);
      }
      else {
        digitalWrite(LED_PIN, HIGH);
      }

      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('1');
      delay(100);
      Keyboard.releaseAll();
      delay(100);
      // Keyboard.println("You pressed the button ");
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('m');
      delay(100);
      Keyboard.releaseAll();
    }
    // save the the last steady state
    lastSteadyState = currentState;
  }

}


/*
    This is just a long comment
    Here are some fun functions you can use to replace the default behaviour 
    Consumer.write(CONSUMER_BRIGHTNESS_UP);
    Consumer.write(CONSUMER_BRIGHTNESS_DOWN);
    Consumer.write(CONSUMER_BROWSER_HOME);
    Consumer.write(CONSUMER_SCREENSAVER);
    Consumer.write(HID_CONSUMER_AL_CALCULATOR); //launch calculator :)
    Consumer.write(HID_CONSUMER_AC_ZOOM_IN);
    Consumer.write(HID_CONSUMER_AC_SCROLL_UP);
    CONSUMER_SLEEP = 0x32,

    FULL LIST CAN BE FOUND HERE:
    https://github.com/NicoHood/HID/blob/master/src/HID-APIs/ConsumerAPI.h
*/
        
