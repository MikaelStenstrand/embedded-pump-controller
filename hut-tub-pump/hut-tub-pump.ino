
#include <TimeLib.h>
#include <Time.h>

// INPUT PINS
const int button = 3;
// OUTPUT PINS
const int ledRed =  12;
const int ledGreen =  10;
const int ledYellow =  11;
const int releOutput = 5;

int currentButtonState = 0;
long buttonTimer = 0;
long longPressTime = 500;         // CONFIGURABLE
boolean buttonActive = false;
boolean longPressActive = false;
int buttonOutput = 0;

unsigned long previousRelayAction = -86400000;
unsigned long previousBlinkingLed = 0;
unsigned long previousResetErrorCheck = 0;

int releState = LOW;
bool tempState;
int releChanges = 0;
boolean setupMode = true;
int currentMode;
unsigned long HOUR_TO_MILLISECONDS = 3600000;
unsigned long LED_ON_BLINKING_TIME = 5000;
unsigned long LED_OFF_BLINKING_TIME = 5000;
unsigned long ERROR_CHECK_TIME = 10000;

boolean resetAutoMode = true;
int autoMode = 0;
int autoModeTimes[3][2]={      // CONFIGURABLE
 {3,12},                       // SUMMER: 3h duration, every 12h
 {3,6},                        // SPRING
 {2,4}                         // WINTER
};

/*
   0 = off
   1 = on
   2 = auto
*/

void setup() {
  pinMode(button, INPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(releOutput, OUTPUT);

  switchRelayState(LOW);
  setTime(00, 00, 00, 1, 1, 2018);
  Serial.begin(115200);
  setupAutoMode();
  currentMode = 0;

  displaySetupMessage();
}

/*
 * Choosing the autoMode
 * 0 - SUMMER
 * 1 - SPRINT
 * 2 - WINTER
*/
boolean setupAutoMode() {
  Serial.println("SETUP INITIALIZED");
  int buttonPress;
  while(setupMode)  {
    buttonPress = handleButtonPress();

    // change autoMode by button press
    if (buttonPress == 1) {
      autoMode++;
      if (autoMode > 2)
        autoMode = 0;
    }
    // exit setup with long press
    if(buttonPress == 2) {
      setupMode = false;
      return true;
    }

    showSetupLed(autoMode);
    delay(50);
  }
}

// digital clock display the time
void displaySetupMessage(){
  Serial.println("STARTED");
  Serial.print("AUTO MODE: ");
  Serial.print(autoMode);
  Serial.println();
  Serial.print("IDLE FOR ");
  Serial.print(autoModeTimes[autoMode][1]);
  Serial.print(" SECONDS");
  Serial.println();
  Serial.print("ON FOR ");
  Serial.print(autoModeTimes[autoMode][0]);
  Serial.print(" SECONDS");
  Serial.println();
}

int handleButtonPress() {
  buttonOutput = -1;
  currentButtonState = digitalRead(button);

  if (currentButtonState == HIGH) {
    if (buttonActive == false)  {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      // LONG PRESS
      buttonOutput = 2;
      currentMode = 2;
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        // Serial.println(currentMode);
        // SHORT PRESS
        buttonOutput = 1;
        currentMode++;
        if (currentMode > 1) {
          currentMode = 0;
        }
      }
      buttonActive = false;
    }
  }
  if (buttonOutput >= 0) {
    return buttonOutput;
  } else {
    return 0;
  }
}

void showSetupLed(int currentAutoMode) {
  switch (currentAutoMode)  {
    case 0:
      blinkLed(ledRed, 500, -1);
      break;
    case 1:
      blinkLed(ledYellow, 500, -1);
      break;
    case 2:
      blinkLed(ledGreen, 500, -1);
      break;
    default:
      break;
  }
}

// switchLedColor
void switchLedColor(int ledPin) {
  digitalWrite(ledRed, 0);
  digitalWrite(ledGreen, 0);
  digitalWrite(ledYellow, 0);
  digitalWrite(ledPin, 1);
}

void blinkLed(int blinkingLed, int frequence, int onLed) {
  unsigned long currentTime = millis();
  byte currentLedState = digitalRead(blinkingLed);
  
  digitalWrite(ledRed, 0);
  digitalWrite(ledGreen, 0);
  digitalWrite(ledYellow, 0);
  if (onLed != -1)
    digitalWrite(onLed, 1);
   
  if (currentTime - previousBlinkingLed >= frequence)  {
    previousBlinkingLed = currentTime;
    if (currentLedState == LOW)  {
      digitalWrite(blinkingLed, 1);
    } else  {
      digitalWrite(blinkingLed, 0);
    }
  }
}

// switchRelayState
// HIGH - Relay on
// LOW  - Relay off
void switchRelayState(int state) {
  tempState = !state;
  if (tempState != digitalRead(releOutput)) {
    digitalWrite(releOutput, tempState);
    releChanges++;
  }
}

byte readRelayState() {
  return !digitalRead(releOutput);
}

void turnOffAutoMode()  {
  resetAutoMode = true;
}

// Auto logic
void runAutoLogic() {
  unsigned long currentTime = millis();

  if (resetAutoMode) {
    Serial.println("reset Auto Mode");
    resetAutoMode = false;
    switchRelayState(LOW);
    previousRelayAction = -86400000;
  }
//  Serial.print(currentTime - previousRelayAction);
//  Serial.print(" >= ");
//  Serial.print(autoModeTimes[autoMode][1] * HOUR_TO_MILLISECONDS);
//  Serial.println();
  if (readRelayState() == LOW && (currentTime - previousRelayAction >= autoModeTimes[autoMode][1] * HOUR_TO_MILLISECONDS)) {
    Serial.println("ON");
    previousRelayAction = currentTime;
    switchRelayState(HIGH);
  } else if (readRelayState() == HIGH && (currentTime - previousRelayAction >= autoModeTimes[autoMode][0] * HOUR_TO_MILLISECONDS))  {
    Serial.println("OFF");
    previousRelayAction = currentTime;
    switchRelayState(LOW);
  }

  if (readRelayState() == LOW) {
    blinkLed(ledRed, LED_OFF_BLINKING_TIME, ledYellow);
  } else {
    blinkLed(ledGreen, LED_ON_BLINKING_TIME, ledYellow); 
  }
}

bool stateMachine() {
  if (currentMode == 0) {
    // OFF
    switchLedColor(ledRed);
    turnOffAutoMode();
    switchRelayState(LOW);
    return true;
  } else if (currentMode == 1) {
    // ON
    switchRelayState(HIGH);
    turnOffAutoMode();
    switchLedColor(ledGreen);
    return true;
  } else if (currentMode == 2) {
    // AUTO
    switchLedColor(ledYellow);  // changed in runAutoLogic
    runAutoLogic();
    return true;
  } else {
    return false;
  }
}

// stuck in error mode, requires boot
void runErrorMode() {
  Serial.println("ERROR MODE ACTIVATED... BOOT TO CONTINUE");
  switchRelayState(LOW);
  turnOffAutoMode();
  
  while(true) {
    switchLedColor(ledRed);
    delay(250);
    switchLedColor(ledGreen);
    delay(250);
    switchLedColor(ledYellow);
    delay(250);
  }
}

// MAIN
void loop() {
  unsigned long currentTime = millis();
  handleButtonPress();
  stateMachine();

  if (currentTime - previousResetErrorCheck >= ERROR_CHECK_TIME)  {
    releChanges = 0;
    previousResetErrorCheck = currentTime;
  }
  
  if (releChanges > 10) {
    runErrorMode();  
  }
  delay(50);
}

