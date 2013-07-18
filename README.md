Arduino Code for Balloon Assisted Stratospheric Experiments
===========================================================

This is the code used in the Arduino Mega for collecting and storing sensor data during high altitude balloon flight for BASE research.

There are three versions. 

* In the [first version](https://github.com/qtstc/Arduino-Code-for-Balloon-Assisted-Stratospheric-Experiments/tree/master/Xbee_Receiver),the Arduino board takes temperature, humidity, presure reading and store them in the onboard non-volatile memory(EEPROM) every time a packet is received from a Xbee receiver. 

* In the [second version](https://github.com/qtstc/Arduino-Code-for-Balloon-Assisted-Stratospheric-Experiments/tree/master/Photometer), the Arduino board takes voltage reading from eight LEDs(which act as photometers) and store them in a micro SD card mounted on an SD card shield. Readings from a triple axis accelerometer(Freescale MMA7361L) and dual axis compass(Hitachi HM55B) are also stored.

* In the [latest version](https://github.com/qtstc/Arduino-Code-for-Balloon-Assisted-Stratospheric-Experiments/blob/master/Photometer_GPS_Compass/Photometer_GPS_Compass.ino), the Arduino board takes voltage reading from eight LEDs, location and altitude information from a GPS receiver(GS407) and orientation information from a tilt-compensated compass(LSM303).

## Setup guide for the latest version of the system

The system takes readings from 
* 8 LEDs,
* [LSM303 breakout board for tilt-compensated compass](https://www.sparkfun.com/products/10703),
* [GS407 GPS receiver](https://www.sparkfun.com/products/11466).
 
The libarary used for LSM303 can be found at [here](https://github.com/qtstc/LSM303).
The [TinyGPS 12 library](http://arduiniana.org/libraries/tinygps/) is used to process the raw packet from the GPS receiver. It is slightly modified to allow the encode(char):bool method to return true even when no satellite lock is received. This is achieved by changing the if (_gps_data_good) to if(true) in term_complete:bool.

The LEDs are connecte to pin A8 to A15 of the board.

The LSM303 uses the [IIC](http://en.wikipedia.org/wiki/I%C2%B2C) interface. The connections for LSM303 to Arduino Mega is listed below:
```
LSM303 | Arduino Mega
Vin    |    5v
GND    |    GND
SDA    |    SDA
SCL    |    SCL
```

The connections for GS407 is listed below:
```
GS407  | Arduino Mega
Vin    |    5v
GND    |    GND
TX     |    RX1
RX     |    TX (Required because we need to set the receiver to flight mode everytime the receiver is powered up)
```

An Arduino SD card shield is also required for more non-volatile storage. Please note that the pins on the shield does not match with the pins on Mega. Some jumper wires are required to connect the respective [SPI](http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus) pins.

The file stored in the SD card is in CSV format. The data can be visualized using the program found [here](https://github.com/qtstc/Data-Visualization-for-Balloon-Assisted-Stratospheric-Experiments).

=================================================================
Copyright 2013 Tao Qian

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
