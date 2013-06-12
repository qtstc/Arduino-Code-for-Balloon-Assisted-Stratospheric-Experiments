#include <SdFat.h>
#include <Time.h>  

/**
  * This is the code to be run on the Arduino Mega used for BASE 2013 photometer project.
  * 
  * Author: Tao Qian, DePauw University 
  */

//Analog pins for LED readings.
const int RED_ANALOG = 0;
const int UV_ANALOG = 1;
const int GREEN_ANALOG = 2;
const int BLUE_ANALOG = 3;
const int VIOLET_ANALOG = 4;
const int IR_ANALOG = 5;
const int YELLOW_ANALOG_1 = 6;
const int YELLOW_ANALOG_2 = 7;
//Analog pins for X,Y,Z of the tile-axis sensor.
const int X_ANALOG = 8;
const int Y_ANALOG = 9;
const int Z_ANALOG = 10;

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
  int data[12];
  data[RED_ANALOG] = analogRead(RED_ANALOG);
  data[UV_ANALOG]= analogRead(UV_ANALOG);
  data[GREEN_ANALOG]= analogRead(GREEN_ANALOG);
  data[BLUE_ANALOG]= analogRead(BLUE_ANALOG);
  data[VIOLET_ANALOG]= analogRead(VIOLET_ANALOG);
  data[IR_ANALOG]= analogRead(IR_ANALOG);
  data[YELLOW_ANALOG_1]= analogRead(YELLOW_ANALOG_1);
  data[YELLOW_ANALOG_2]= analogRead(YELLOW_ANALOG_2);
  data[X_ANALOG]= analogRead(X_ANALOG);
  data[Y_ANALOG]= analogRead(Y_ANALOG);
  data[Z_ANALOG]= analogRead(Z_ANALOG);
  data[11] = getCompassData();
  //debugCompassAndTilt(data);
  writeCSVLine(data,12);
}

/**
  * Converts the tilt-axis reading to values in g.
  */
float toG(int data)
{
  return (((float)data)*5/1024-1.65)/1.1;
}

/**
   * Method that prints the acceleration in g along different axis.
   *  Only used for debugging.
   */
void debugCompassAndTilt(int* data)
{
  Serial.print(" X ");
  Serial.print(toG(data[X_ANALOG]));
  Serial.print(" Y ");
  Serial.print(toG(data[Y_ANALOG]));
  Serial.print(" Z ");
  Serial.print(toG(data[Z_ANALOG]));
  Serial.print("C ");
  Serial.println(data[11]);
}

/** 
  * Get the reading of the compass.
  * This function assumes that the pins used by the compass are digital 22 to 29(both inclusive),
  * with 22 as the lowest bit.
  */
int getCompassData()
{
  int base = 1;
  int sum = 0;
  for(int i = 22;i<30;i++)
  {
    if(digitalRead(i))
      sum += base;
    base *= 2;
  }
  return sum;
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
  myFile.print(",LED_red,LED_UV,LED_green,LED_blue,LED_violet,LED_IR,LED_yellow_1,LED_yellow_2");
  myFile.print(",X,Y,Z");
  myFile.println(",Compass,");
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
