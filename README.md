# SeniorDesign-2024-2025
This repository contains codes and an updated pressor sensor library for Arduino in order for our pulmonary artery simulator to work.
The code is to be utilized with Arduino Nano 33 IOT. LSM6DS3 library has to be dowloaded in Arduino IDE before the launch of the script. 
MS5803_02 library is **NOT** an original creation. The original library was created by Luke Miller and can be found here: https://github.com/millerlp/MS5803_02
We experienced I2C collapse when using both internal IMU Arduino sensor, so we added error handling and debug output to address the issue. 
*More description to come on March 16th*
