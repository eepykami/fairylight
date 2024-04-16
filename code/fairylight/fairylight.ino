// TODO: state machine to change

#define LED1 0 // LED pins
#define LED2 1
 
#define ON HIGH // Macros for ON/OFF
#define OFF LOW

unsigned int pause = 10;  // time for solid color to stay on
int fadeIncrement = 1; // fading (for the timing)
 
// variables for pattern timing
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
unsigned long millisInterval = 10; // how quick fading speed is set
// this is the frequency of the sw PWM, frequency = 1/(2 * microInterval)
unsigned long microInterval = 100;  //  200 yield the least flickering
 
// variables for software PWM
unsigned long currentMicros = micros();
unsigned long previousMicros = 0;

unsigned int rotation = 0;
signed int brightness = 0;
signed int temp = 0;

signed int lowPattern = 0;
signed int highPattern = 0;

const byte pwmMax = 80;
const byte pwmMin = 0;
typedef struct pwmPins { // typedef for properties of each sw pwm pin
  int pin;
  int pwmValue;
  bool pinState;
  int pwmTickCount;
} pwmPin;
 
// create the sw pwm pins
// these can be any I/O pin
// that can be set to output!
const int pinCount = 1;
const byte pins[pinCount] = {2};

pwmPin myPWMpins[pinCount];
 
// function to "setup" the sw pwm pin states
// modify to suit your needs
// this creates an alternating fade pattern
void setupPWMpins() {
  for (int index=0; index < pinCount; index++) {
    myPWMpins[index].pin = pins[index];
    myPWMpins[index].pwmValue = 0;
    myPWMpins[index].pinState = ON;
    myPWMpins[index].pwmTickCount = 0;
  }
}
 
void pwmFadePattern() {
  // go through each sw pwm pin, and increase
  // the pwm value. this would be like
  // calling analogWrite() on each hw pwm pin
  for (int index=0; index < pinCount; index++) {
    if (rotation == 0)  brightness += fadeIncrement;
      else brightness -= fadeIncrement;
    if (brightness > highPattern ) {
      rotation = 1;
    } else if (brightness <= lowPattern) {
      rotation = 0;
    }
    temp = brightness;
    if (temp > pwmMax) temp = pwmMax;
    else if (temp < 0) temp = 0;
    myPWMpins[index].pwmValue = temp;
  }
}
 
void handlePWM() {
  currentMicros = micros();
  // check to see if we need to increment our PWM counters yet
    if (currentMicros - previousMicros >= microInterval) {
    // Increment each pin's counter
    for (int index=0; index < pinCount; index++) {
      // each pin has its own tickCounter
      // determine if we're counting on or off time
      
      if (myPWMpins[index].pwmValue == pwmMax || myPWMpins[index].pwmValue == 0) {
        // do nothing so LED do not flicker
        // also do not increase the pwmTickCount or it'll runaway and fail to reset
        break; 
      }
      
      myPWMpins[index].pwmTickCount++;
      if (myPWMpins[index].pinState == ON) {
        // see if we hit the desired on percentage. not as precise as 255 or 1024, but easier to do math
        if (myPWMpins[index].pwmTickCount >= myPWMpins[index].pwmValue) {
          myPWMpins[index].pinState = OFF;
        }
      } else {
        // if it isn't on, it is off
        if (myPWMpins[index].pwmTickCount >= pwmMax) {
          myPWMpins[index].pinState = ON;
          myPWMpins[index].pwmTickCount = 0;
        }
      }
      if (myPWMpins[index].pinState == ON) {
          digitalWrite(LED2, ON);
          digitalWrite(LED1, OFF);
      } else {
          digitalWrite(LED1, ON);
          digitalWrite(LED2, OFF);
      }
    }
    previousMicros = currentMicros; // reset the micros() tick counter.
  }
}
 
void setup() {
  highPattern = pwmMax + pause;
  lowPattern = pwmMin - pause;

  setupPWMpins();
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT); 
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
}
 
void loop() {
  handlePWM();
  currentMillis = millis(); // check timer for fading pattern
  if (currentMillis - previousMillis >= millisInterval) {
    pwmFadePattern();
    previousMillis = currentMillis; // setup clock for next tick
  }
}