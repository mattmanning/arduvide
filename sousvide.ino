#include <OneWire.h>
#include <DallasTemperature.h>
#include <LedControl.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define POWER_SWITCH 7
#define POT_TIMEOUT 2000

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Global variables
float current_temperature, set_temperature, diff, threshold, pot_value;
int pot_time;

/*
 * Create a new LedControl.
 * Pin 12 is connected to the DATA IN-pin of the MAX7219
 * Pin 11 is connected to the CLK-pin of the MAX7219
 * Pin 10 is connected to the LOAD(/CS)-pin of the MAX7219
 * There will only be a single MAX7221 attached to the arduino
 */
LedControl led_control=LedControl(12,11,10,1);


void setup(void)
{
  // Set defaults
  set_temperature = 100;
  threshold = 1.0;
  pot_time = -POT_TIMEOUT;

  // Set up the relay output and turn it off initially
  pinMode(POWER_SWITCH, OUTPUT);
  digitalWrite(POWER_SWITCH, LOW);

  // Start up the temperature sensor
  sensors.begin();

  // Take the LED driver out of power save mode
  led_control.shutdown(0,false);
  led_control.setIntensity(0, 15);
}

// Display the given temperature on the 3-digit 7-segment display
void displayTemp(int temp) {
  led_control.setDigit(0, 0, (byte)(temp / 100), false);
  led_control.setDigit(0, 1, (byte)((temp / 10) % 10), false);
  led_control.setDigit(0, 2, (byte)(temp % 10), false);
  return;
}

// Get the temperature reading in degrees F
void readThermometer() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  current_temperature = sensors.getTempFByIndex(0);
}

// Implements a simple thermostat state machine to switch the relay
// based on the state and the temperature reading.
void switchOutput() {
  diff = current_temperature - set_temperature;

  // Do nothing if the temperature difference is outside our threshold.
  if (abs(diff) < threshold) { return; }

  // If the temperature is too low and the power is off
  if ((diff < 0) && !digitalRead(POWER_SWITCH)) {
    // Turn the power on.
    digitalWrite(POWER_SWITCH, HIGH);
  // Otherwise if the temp is too high and the power is on
  } else if ((diff > 0) && digitalRead(POWER_SWITCH)) {
    // Turn the power off.
    digitalWrite(POWER_SWITCH, LOW);
  }
}

// Read the potentiometer value from the ADC and calculate temperature
// from it. If it's different, set the set_temperature to it and reset
// the pot timeout.
void readPot() {
  int new_pot_value = analogRead(0);
  pot_value = (0.7 * new_pot_value + 0.3 * pot_value);
  int new_temperature = ((pot_value / 10) + 100);
  if (new_temperature != set_temperature) {
    pot_time = millis();
    set_temperature = new_temperature;
  }
}

// Returns true if the temperature is being changed.
// Returns false if it's been at least timeout ms since the temperature
// was changed.
boolean pot_timer(int timeout) {
  return((pot_time + timeout) > millis());
}

void loop(void)
{
  readPot();
  if (pot_timer(POT_TIMEOUT)) {
    displayTemp(set_temperature);
  } else {
    readThermometer();
    displayTemp(current_temperature);
    switchOutput();
  }
}


