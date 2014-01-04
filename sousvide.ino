#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define POWER_SWITCH 7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

float current_temperature, set_temperature, diff, threshold;

void setup(void)
{
  set_temperature = 80.0;
  threshold = 1.0;

  pinMode(POWER_SWITCH, OUTPUT);
  digitalWrite(POWER_SWITCH, LOW);

  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
}


void loop(void)
{   
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  current_temperature = sensors.getTempFByIndex(0);
  Serial.println("DONE");

  Serial.print("Temperature for Device 1 is: ");
  Serial.println(current_temperature);

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


