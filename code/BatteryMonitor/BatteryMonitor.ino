#define BATTERY_CRITICAL 0
#define BATTERY_LOW 1
#define BATTERY_MID 2
#define BATTERY_HIGH 3

#define POWER_CONTROL_PIN 4
#define BATTERY_SAMPLE_PIN A3
#define RED_PIN 0
#define GREEN_PIN 1
#define BLUE_PIN 2

const int batteryMidReading = 867;
const int batteryCriticalReading = 818;
const int batteryLowReading = 840;
const int hysteresis = 20;

int batteryStatus;

void setup() {
  // Set safe initial pin states
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  digitalWrite(POWER_CONTROL_PIN, LOW);

  // Enable pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(POWER_CONTROL_PIN, OUTPUT);

  // Start with power enabled so that everything starts up and we get
  // the inrush current over with. This will wait a couple of seconds for
  // the sampling capacitor to charge up before we start monitoring.
  showGreenIndicator();
  turnPowerOn();
  
  setInitialState();
}

void loop() {
  setPower();
  setIndicator();

  readBattery();

  if (batteryStatus == BATTERY_CRITICAL) {
    // TODO: this should be a low-power sleep
    delay(3000);
  }
}

void setInitialState() {
  int voltageReading = analogRead(BATTERY_SAMPLE_PIN);

  if (voltageReading < batteryCriticalReading) {
    batteryStatus = BATTERY_CRITICAL;
  }
  else if (voltageReading < batteryLowReading) {
    batteryStatus = BATTERY_LOW;
  }
  else if (voltageReading < batteryMidReading) {
    batteryStatus = BATTERY_MID;
  }
  else {
    batteryStatus = BATTERY_HIGH;
  }
}

void readBattery() {
  int voltageReading = analogRead(BATTERY_SAMPLE_PIN);

  // TODO: fix so that we can jump multiple states upward at once
  
  if (batteryStatus == BATTERY_HIGH && voltageReading < batteryMidReading) {
    batteryStatus = BATTERY_MID;
  }
  else if (batteryStatus == BATTERY_MID && voltageReading < batteryLowReading) {
    batteryStatus = BATTERY_LOW;
  }
  else if (batteryStatus == BATTERY_LOW && voltageReading < batteryCriticalReading) {
    batteryStatus = BATTERY_CRITICAL;
  }
  else if (batteryStatus == BATTERY_CRITICAL && voltageReading > batteryCriticalReading + hysteresis) {
    batteryStatus = BATTERY_LOW;
  }
  else if (batteryStatus == BATTERY_LOW && voltageReading > batteryLowReading + hysteresis) {
    batteryStatus = BATTERY_MID;
  }
  else if (batteryStatus == BATTERY_MID && voltageReading > batteryMidReading + hysteresis) {
    batteryStatus = BATTERY_HIGH;
  }

  // TODO: if the voltage is > 14V, we're probably in full charge mode so set BATTERY_MID
  // Also fix initial state
}

void setPower() {
  if (batteryStatus == BATTERY_CRITICAL) {
    turnPowerOff();
  }
  else {
    digitalWrite(POWER_CONTROL_PIN, HIGH);
  }
}

void turnPowerOff() {
  digitalWrite(POWER_CONTROL_PIN, LOW);
}

void turnPowerOn() {
  digitalWrite(POWER_CONTROL_PIN, HIGH);
  delay(2000);
}

void setIndicator() {
  if (batteryStatus == BATTERY_HIGH) {
    showGreenIndicator();
  } else if (batteryStatus == BATTERY_MID) {
    showYellowIndicator();
  } else if (batteryStatus == BATTERY_LOW) {
    showRedIndicator();
  } else {
    showRedIndicator();
    delay(10);
    showNoIndicator();
  }
}

void showGreenIndicator() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH);
}

void showYellowIndicator() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH);
}

void showRedIndicator() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
}

void showNoIndicator() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
}

