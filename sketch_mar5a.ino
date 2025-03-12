#include <Wire.h>
#include "MS5803_02.h"
#include <Arduino_LSM6DS3.h>

// MS5803 sensor with oversampling = 512
MS_5803 sensor(512);

// IMU Data Variables
float xAcc, yAcc, zAcc;

// MS5803 Coefficients
unsigned int coefficients[8];

// Flag to start CSV logging after "start" command
bool recordingActive = false;

void setup() {
    Serial.begin(9600);
    while (!Serial) { /* Wait for Serial */ }
    
    Wire.begin();
    Wire.setClock(400000);
    
    Serial.println("Initializing IMU...");
    if (!IMU.begin()) {
        Serial.println("IMU initialization failed. Check connections.");
        while (1);
    }
    Serial.println("IMU initialized successfully.");
    
    Serial.println("Initializing MS5803 sensor...");
    if (!sensor.initializeMS_5803(true)) {
        Serial.println("WARNING: MS5803 initialization returned false. Possible CRC error.");
    } else {
        Serial.println("MS5803 initialized successfully.");
    }
    
    Serial.println("Reading MS5803 Calibration Data...");
    for (int i = 0; i < 8; i++) {
        Wire.beginTransmission(0x76);
        Wire.write(0xA0 + (i * 2));
        if (Wire.endTransmission() != 0) {
            Serial.print("I2C Transmission Failed at PROM Register: 0x");
            Serial.println(0xA0 + (i * 2), HEX);
            return;
        }
        delay(10);
        Wire.requestFrom(0x76, 2);
        if (Wire.available() == 2) {
            uint8_t highByte = Wire.read();
            uint8_t lowByte = Wire.read();
            coefficients[i] = (highByte << 8) | lowByte;
            Serial.print("C");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(coefficients[i]);
        } else {
            Serial.print("PROM Read Error at C");
            Serial.println(i);
        }
    }
    
    Serial.println("Verifying CRC...");
    uint8_t stored_crc = coefficients[7] & 0x0F;
    uint8_t computed_crc = sensor.MS_5803_CRC(coefficients);
    Serial.print("Stored CRC (4-bit): ");
    Serial.println(stored_crc);
    Serial.print("Calculated CRC (4-bit): ");
    Serial.println(computed_crc);
    if (stored_crc != computed_crc) {
        Serial.println("CRC mismatch! Data may be unreliable.");
    } else {
        Serial.println("CRC match! Coefficients look good.");
    }
    
    Serial.println("\nDebugging complete. Type 'start' to begin CSV logging...");
    Serial.println("(Ensure your Serial Monitor sends 'start' with newline.)");
}

void loop() {
    // Listen for the "start" command
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.equalsIgnoreCase("start")) {
            recordingActive = true;
            Serial.println("tilt,pressure,temperature,CRC_expected,CRC_calculated");
        }
    }
    
    if (!recordingActive) {
        return;
    }

    // Read accelerometer
    if (IMU.readAcceleration(xAcc, yAcc, zAcc)) {
        // Calculate total magnitude of acceleration
        float gMag = sqrt(xAcc * xAcc + yAcc * yAcc + zAcc * zAcc);
        
        // Tilt angle (angle between sensor's XY-plane and gravity)
        float tilt = acos(zAcc / gMag) * 180.0 / PI;

        // Read MS5803 sensor data
        sensor.readSensor();
        float pressure = sensor.pressure();
        float temperature = sensor.temperature();
        
        // CRC values
        uint8_t p_crc = coefficients[7] & 0xF;
        uint8_t n_crc = sensor.MS_5803_CRC(coefficients);

        // Print CSV data
        Serial.print(tilt, 2);
        Serial.print(",");
        Serial.print(pressure, 2);
        Serial.print(",");
        Serial.print(temperature, 2);
        Serial.print(",");
        Serial.print(p_crc);
        Serial.print(",");
        Serial.println(n_crc);
    } else {
        Serial.println("Failed to read IMU data.");
    }

    delay(100); // Adjust for desired update rate
}
