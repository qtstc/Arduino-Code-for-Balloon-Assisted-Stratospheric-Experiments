#include <Wire.h>
#include <LSM303.h>

/**
  * Code for testing the LSM303 sensor.
  */

LSM303 compass;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  // Calibration values. Use the Calibrate example program to get the values for
  // your compass.
  compass.m_min.x = -520; compass.m_min.y = -570; compass.m_min.z = -770;
  compass.m_max.x = +540; compass.m_max.y = +500; compass.m_max.z = 180;
}

void loop() {
  compass.read();
  Serial.print("Pitch: ");
  Serial.print(compass.pitch());
  Serial.print(" Roll: ");
  Serial.print(compass.roll());
  Serial.print(" Heading: ");
  Serial.println(compass.heading());
  delay(500);
}
