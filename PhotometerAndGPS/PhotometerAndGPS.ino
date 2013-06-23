#include <SdFat.h>
#include <Time.h>
#include <Wire.h>
#include <LSM303.h>

LSM303 compass;

/**
  * This is the code to be run on the Arduino Mega used for BASE 2013 photometer project.
  * 
  * Author: Tao Qian, DePauw University 
  */

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
  Serial.begin(9600);
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  initializeCSVFile();
  initializeCompass();
}

void loop()
{
  delay(1000);//Wait one second;
  takeData();
}

/**
  * Method for taking data and storing them to the sd card.
  */
void takeData()
{
  compass.read();
  int data[11];
  data[0] = analogRead(IR940_ANALOG);
  data[1]= analogRead(IR830_ANALOG);
  data[2]= analogRead(RED_ANALOG);
  data[3]= analogRead(YELLOW_ANALOG);
  data[4]= analogRead(GREEN_ANALOG);
  data[5]= analogRead(BLUE_ANALOG);
  data[6]= analogRead(VIOLET400_ANALOG);
  data[7]= analogRead(UV351_ANALOG);
  data[8] = compass.pitch();
  data[9] = compass.roll();
  data[10] = compass.heading();
  writeCSVLine(data,11);
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
  myFile.println(",IR940,IR830,RED,YELLOW,GREEN,BLUE,VIOLET400,UV351,Pitch,Roll,Heading");
  myFile.close();
  setTime(11,0,0,10,6,2013);//Set the time, needs to be changed every time!
  //setTime(hr,min,sec,day,month,yr);
}

/**
  * Write a line to the file used to store data
  */
void writeCSVLine(int* readings,int arraySize)
{
  openFile();
  //First print the time.
  //myFile.print(year());
  //myFile.print("-");
  myFile.print(month());
  myFile.print("-");
  myFile.print(day());
  myFile.print("_");
  myFile.print(hour());
  myFile.print(":");
  myFile.print(minute());
  myFile.print(":");
  myFile.print(second());
  myFile.print(",");
  for(int i = 0;i<arraySize-1;i++)
  {
    myFile.print(readings[i]);
    myFile.print(",");
    //Serial.print(readings[i]);
    //Serial.print(",");
  }
  //Serial.println(readings[arraySize-1]);
  myFile.println(readings[arraySize-1]);

  // close the file:
  myFile.close();
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
