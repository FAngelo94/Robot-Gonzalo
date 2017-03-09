#include <Wire.h>

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
}

byte x = 0;

void loop() {
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write("p");        // sends five bytes
  Wire.endTransmission();    // stop transmitting
  delay(5000);
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write("s");        // sends five bytes
  Wire.endTransmission();    // stop transmitting
  delay(5000);
}
