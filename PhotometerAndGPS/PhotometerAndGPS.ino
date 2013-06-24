#include <SdFat.h>
#include <Time.h>
#include <Wire.h>
#include <LSM303.h>
#include <TinyGPS.h>

/**
  * This is the code to be run on the Arduino Mega used for BASE 2013 photometer project.
  * 
  * Author: Tao Qian, DePauw University 
  */

#define GPSBAUD  9600
#define TERMBAUD  115200

LSM303 compass;
TinyGPS gps;

//Analog pins for LED readings.
const int IR940_ANALOG = 8;
const int IR830_ANALOG = 9;
const int RED_ANALOG = 10;
const int YELLOW_ANALOG = 11;
const int GREEN_ANALOG = 12;
const int BLUE_ANALOG = 13;
const int VIOLET400_ANALOG = 14;
const int UV351_ANALOG = 15;

//Mode for the SD card shield.
const int chipSelect = 8;
SdFat sd;
SdFile myFile;

void setup() {
  
  // Sets baud rate of your terminal program
  Serial.begin(TERMBAUD);
  // Sets baud rate of your GPS
  Serial1.begin(GPSBAUD);
  
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  initializeCSVFile();
  initializeCompass();
}

void loop()
{
  delay(500);//Wait one second;
  takeData();
}

/**
  * Method for taking data and storing them to the sd card.
  */
void takeData()
{
  openFile();
  writeTime();
  writeLED();
  writeCompass();
  writeGPS();
  myFile.close();
}

/**
  * Converts the tilt-axis reading to values in g.
  */
float toG(int data)
{
  return (((float)data)*5/1024-1.65)/1.1;
}

void initializeCompass()
{
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  // Calibration values.
  compass.m_min.x = -520; compass.m_min.y = -570; compass.m_min.z = -770;
  compass.m_max.x = +540; compass.m_max.y = +500; compass.m_max.z = 180;
}


/**
  * Initalize the file used to store data.
  * It writes the headers for all columns to the file.
  */
void initializeCSVFile()
{
  openFile();
  //Initialize the column names.
  myFile.print("Time");
  myFile.println(",IR940,IR830,RED,YELLOW,GREEN,BLUE,VIOLET400,UV351,Pitch,Roll,Heading,GPSTime,Latitude,Longitude,Altitude,Course(degree),Speed(kmph)");
  myFile.close();
  //setTime(hr,min,sec,day,month,yr);
}

void writeTime()
{
  myFile.print(millis());
  myFile.print(",");
}

void writeLED()
{
  int data[8];
  data[0]= analogRead(IR940_ANALOG);
  data[1]= analogRead(IR830_ANALOG);
  data[2]= analogRead(RED_ANALOG);
  data[3]= analogRead(YELLOW_ANALOG);
  data[4]= analogRead(GREEN_ANALOG);
  data[5]= analogRead(BLUE_ANALOG);
  data[6]= analogRead(VIOLET400_ANALOG);
  data[7]= analogRead(UV351_ANALOG);
  for(int i = 0;i<8;i++)
  {
    myFile.print(data[i]);
    myFile.print(",");
  }
}

void writeCompass()
{
  compass.read();
  myFile.print(compass.pitch());
  myFile.print(",");
  myFile.print(compass.roll());
  myFile.print(",");  
  myFile.print(compass.heading());
  myFile.print(",");
}

void writeGPS()
{
   while(Serial1.available())
  {
    char c = Serial1.read();
    if(!gps.encode(c))
    {
      myFile.println(",,,,,");
      return;
    }
    
    int year;
    byte month, day, hour, minute, second, hundredths;
    gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
    myFile.print(month, DEC); 
    myFile.print("-"); 
    myFile.print(day, DEC);
    myFile.print("_"); 
    myFile.print(hour, DEC);
    Serial.print(":"); 
    Serial.print(minute, DEC);
    Serial.print(":");
    Serial.print(second, DEC); 
    Serial.print(".");
    Serial.print(hundredths, DEC);
    Serial.print(",");
    
    float latitude,longitude;
    gps.f_get_position(&latitude,&longitude);
    myFile.print(latitude,5);
    myFile.print(",");
    myFile.print(longitude,5);
    myFile.print(",");
    
    myFile.print(gps.f_altitude());
    myFile.print(",");
    myFile.print(gps.f_course());
    myFile.print(",");
    myFile.println(gps.f_speed_kmph());  
  }
}

/**
  * Open or create the file in the sd card.
  */
void openFile()
{
     // open the file for write at end like the Native SD library
  if (!myFile.open("data.csv", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
}
