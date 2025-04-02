#include <Wire.h>
#include "MS5803_14.h"
#include <Arduino_LSM6DS3.h>

// MS5803 sensor with oversampling = 512
MS_5803 sensor(512);

// IMU Data Variables
float xAcc, yAcc, zAcc;

// MS5803 Coefficients
unsigned int coefficients[8];

// Flags
bool calibrated = false;
bool recordingActive = false;

// Atmospheric pressure calibration
float atmosphericPressure = 0.0;

float calculatePressure(unsigned long D1, unsigned long D2, unsigned int* C, float &tempC_out) {
    int32_t dT = D2 - ((uint32_t)C[5] << 8);
    int32_t TEMP = 2000 + ((int64_t)dT * C[6]) / 8388608;

    int64_t OFF  = ((int64_t)C[2] << 17) + ((int64_t)C[4] * dT) / 64;
    int64_t SENS = ((int64_t)C[1] << 16) + ((int64_t)C[3] * dT) / 128;

    int32_t P = (((D1 * SENS) / 2097152) - OFF) / 32768;

    tempC_out = TEMP / 100.0;
    return P / 100.0; // mbar
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    Wire.begin();
    Wire.setClock(400000);

    Serial.println("Initializing IMU...");
    if (!IMU.begin()) {
        Serial.println("IMU initialization failed. Check connections.");
        while (1);
    }

    Serial.println("Initializing MS5803 sensor...");
    if (!sensor.initializeMS_5803(true)) {
        Serial.println("WARNING: MS5803 initialization returned false.");
    }

    Serial.println("Reading calibration coefficients...");
    for (int i = 0; i < 8; i++) {
        Wire.beginTransmission(0x76);
        Wire.write(0xA0 + (i * 2));
        if (Wire.endTransmission() != 0) {
            Serial.print("I2C error at coefficient C");
            Serial.println(i);
            return;
        }
        delay(10);
        Wire.requestFrom(0x76, 2);
        if (Wire.available() == 2) {
            uint8_t highByte = Wire.read();
            uint8_t lowByte = Wire.read();
            coefficients[i] = (highByte << 8) | lowByte;
            Serial.print("C"); Serial.print(i); Serial.print(" = ");
            Serial.println(coefficients[i]);
        }
    }

    uint8_t stored_crc = coefficients[7] & 0x0F;
    uint8_t computed_crc = sensor.MS_5803_CRC(coefficients);
    Serial.print("Stored CRC (C7 & 0x0F): ");
    Serial.println(stored_crc);
    Serial.print("Computed CRC: ");
    Serial.println(computed_crc);
    if (stored_crc != computed_crc) {
        Serial.println("⚠️ CRC mismatch! Calibration data may be corrupt.");
    } else {
        Serial.println("✅ CRC match. Coefficients look good.");
    }

    // Start 30s calibration immediately
    Serial.println("\nMeasuring atmospheric pressure for 30 seconds. DO NOT turn on the pump.");
    float pressureSum = 0.0;
    int pressureCount = 0;
    unsigned long start = millis();
    while (millis() - start < 30000) {
        sensor.readSensor();
        float dummyTemp;
        float p = calculatePressure(sensor.D1, sensor.D2, coefficients, dummyTemp);
        pressureSum += p;
        pressureCount++;
        delay(100);
    }
    atmosphericPressure = pressureSum / pressureCount;
    calibrated = true;

    Serial.print("Calibration complete. Atmospheric pressure = ");
    Serial.print(atmosphericPressure, 2);
    Serial.println(" mbar");
    Serial.println("\nType 'start' to begin recording gauge pressure data...");
}

void loop() {
    // Wait for 'start' after calibration
    if (!recordingActive && Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.equalsIgnoreCase("start")) {
            recordingActive = true;
            Serial.println("tilt,gauge_pressure,temperature,CRC_expected,CRC_calculated");
        }
    }

    if (!recordingActive || !calibrated) return;

    // Read and log gauge pressure + IMU
    if (IMU.readAcceleration(xAcc, yAcc, zAcc)) {
        float gMag = sqrt(xAcc * xAcc + yAcc * yAcc + zAcc * zAcc);
        float tilt = acos(zAcc / gMag) * 180.0 / PI;

        sensor.readSensor();
        float tempC;
        float absPressure = calculatePressure(sensor.D1, sensor.D2, coefficients, tempC);
        float gaugePressure = absPressure - atmosphericPressure;

        uint8_t p_crc = coefficients[7] & 0x0F;
        uint8_t n_crc = sensor.MS_5803_CRC(coefficients);

        Serial.print(tilt, 2);
        Serial.print(",");
        Serial.print(gaugePressure, 2);
        Serial.print(",");
        Serial.print(tempC, 2);
        Serial.print(",");
        Serial.print(p_crc);
        Serial.print(",");
        Serial.println(n_crc);
    } else {
        Serial.println("IMU read failed.");
    }

    delay(5); // 
}
