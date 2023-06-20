/**
* ThermoLogger
**/

// for OLED
#include <Wire.h> 
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
SSD1306AsciiWire OLED;

// for Thermo-Couple
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#define MAXCS     10
Adafruit_MAX31855 thermocouple(MAXCS);


void setup() {
  // OLEDの初期化
  Wire.begin();
  Wire.setClock(400000L);
  OLED.begin(&Adafruit128x32, 0x3C);

  OLED.setFont(Callibri15);
  OLED.clear();
  OLED.setCursor(15, 1); OLED.print("THERMO-LOGGER");

  // MAX31855の初期化
  Serial.begin(9600);

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }



  delay(3000);
  OLED.clear();

}

void loop() {
  // basic readout test, just print the current temp
   Serial.print("Internal Temp = ");
   Serial.println(thermocouple.readInternal());

   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Thermocouple fault(s) detected!");
     uint8_t e = thermocouple.readError();
     if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
     if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
     if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
   } else {
     Serial.print("C = ");
     Serial.println(c);
   }
   //Serial.print("F = ");
   //Serial.println(thermocouple.readFahrenheit());

   delay(1000);
}
