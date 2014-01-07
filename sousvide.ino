#include <OneWire.h>
#include <DallasTemperature.h>
#include <LedControl.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define POWER_SWITCH 7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

float current_temperature, set_temperature, diff, threshold;

/*
 * Create a new LedControl.
 * Pin 12 is connected to the DATA IN-pin of the MAX7221
 * Pin 11 is connected to the CLK-pin of the MAX7221
 * Pin 10 is connected to the LOAD(/CS)-pin of the MAX7221
 * There will only be a single MAX7221 attached to the arduino
 */
LedControl led_control=LedControl(12,11,10,1);


void setup(void)
{
  set_temperature = 100;
  threshold = 1.0;

  pinMode(POWER_SWITCH, OUTPUT);
  digitalWrite(POWER_SWITCH, LOW);

  // Start up the temperature sensor
  sensors.begin();

  // Take the LED driver out of power save mode
  led_control.shutdown(0,false);
}

// Display the given temperature on the 3-digit 7-segment display
void displayTemp(int temp) {
  led_control.setDigit(0, 0, (byte)(temp / 100), false);
  led_control.setDigit(0, 1, (byte)((temp / 10) % 10), false);
  led_control.setDigit(0, 2, (byte)(temp % 10), false);
  return;
}

void loop(void)
{
  // Read the pot value and scale it to an int from 100 to 202.
  set_temperature = ((analogRead(0) / 10) + 100);
  displayTemp(set_temperature);

  sensors.requestTemperatures(); // Send the command to get temperatures
  current_temperature = sensors.getTempFByIndex(0);

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


