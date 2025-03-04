#include <Wire.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("I2C Scanner Starting...");

  // Start the I2C bus
  Wire.begin();
  Wire.setClock(100000); // Lower clock speed for stability
  delay(100);
}

void loop() {
  Serial.println("Scanning...");

  bool deviceFound = false; // Flag to track if a device was found

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(address, HEX);
      deviceFound = true; // Set flag to true if a device is found
      delay(500);
    }
  }

  // Only print "No device found" if we never detected anything
  if (!deviceFound) {
    Serial.println("No device found.");
  }

  delay(2000); // Wait before scanning again
}
