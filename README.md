# SeniorDesign-2024-2025
This repository contains codes and an updated pressor sensor library for Arduino in order for our pulmonary artery simulator to work.

The code is to be utilized with Arduino Nano 33 IOT. LSM6DS3 library has to be dowloaded in Arduino IDE before the launch of the script. 
MS5803_02 library is **NOT** an original creation. The original library was created by Luke Miller and can be found here: https://github.com/millerlp/MS5803_02

We experienced I2C collapse when using both internal IMU Arduino sensor, so we added error handling and debug output to address the issue.

To run our scripts, you need to manually installed Arduino library as explained in the Manuall Installation section of this tutorial: https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries/

First launch Arduino file. *Current* version is **version2_February25th.ino**. The file includes initiation of the internal accelerometer and gyroscope sensors embedded on Arduino Nano 33 IOT and the initiation of the pressure sensor connected to the atduino via I2C protocol. Refer to the following link for the wiring schema: **link is coming soon**. 

If your machine is not initiating the sensors, it might be having an issue with an I2C protocol. We include a script (**scan_for_pressure_sensor**) for scanning of I2C addresses that could be used for debugging the I2C connection. Tutorial on how to resolve I2C connection issues is not included in this repository. 

Ensure both sensors initialize and you see the output change in the **Serial Monitor** as you change the spatial axial position of the device. Once you see the output, please **CLOSE THE SERIAL MONITOR IN ARDUINO**.

Proceed with launching python file  *current* version is **data_acq2.py**. You can launch both from the terminal or python IDE. This script handles writing the data into csv files. 

*More description to come on March 16th*
