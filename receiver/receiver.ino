#include <EEPROM.h>

int HUMIDITY_ANALOG = 0;
int TEMPERATURE_ANALOG = 1;
int PRESSURE_ANALOG = 2;

int HUMIDITY_LED = 10;
int TEMPERATURE_LED = 11;
int PRESSURE_LED = 12;

int CAMERA_SWITCH = 8;
int PICTURE_SWITCH = 9;

byte HIGHER_BOUND = 0x0F;
byte LOWER_BOUND = 0x00;

int fiveNineCount;

void setup() {
  Serial.begin(38400);
  Serial.println("Started");
  printValidMemory();
  fiveNineCount = 0;
  pinMode(HUMIDITY_LED, OUTPUT);   
  pinMode(PRESSURE_LED, OUTPUT);   
  pinMode(TEMPERATURE_LED, OUTPUT);  
  pinMode(CAMERA_SWITCH,OUTPUT);
  pinMode(PICTURE_SWITCH,OUTPUT);
  pinMode(13,OUTPUT); 
  digitalWrite(CAMERA_SWITCH,HIGH);
  delay(500);
  digitalWrite(CAMERA_SWITCH,LOW);
}

void loop() { 
   if (Serial.available()) {
     byte b = Serial.read();
     if(b == 0x59)
     {
       fiveNineCount++;
       return;
     }
     if(fiveNineCount != 3)
     {
       fiveNineCount = 0;
       return;
     }
     fiveNineCount = 0;
     //The data received must be between the LOWER_BOUND and the HIGHER_BOUND
     if(b < LOWER_BOUND || b > HIGHER_BOUND)
       return;
     int humidityReading = sensorRead(HUMIDITY_ANALOG);
     int tempReading = sensorRead(TEMPERATURE_ANALOG);
     int pressureReading = sensorRead(PRESSURE_ANALOG);
     saveData(b,humidityReading,tempReading,pressureReading);
     // Serial.print("Humidity: ");
     // Serial.print(humidityReading);
     // Serial.print(" Temperature: "); 
     // Serial.print(tempReading);
     // Serial.print(" Pressure: ");
     // Serial.println(pressureReading);
     if(b>>3)
       takePicture();  
     ledOn(b,1000);
   }
}

int sensorRead(int sensorNumber)
{
  int result = analogRead(sensorNumber);
  return result;
}

void takePicture()
{
  digitalWrite(PICTURE_SWITCH,HIGH);
  delay(500);
  digitalWrite(PICTURE_SWITCH,LOW);
}

void ledOn(byte b,int time)
{
  if((b>>2)&1)
     digitalWrite(HUMIDITY_LED, HIGH);;
  if((b>>1)&1)
     digitalWrite(TEMPERATURE_LED, HIGH);
  if(b&1)
     digitalWrite(PRESSURE_LED,HIGH);
  delay(time);
  digitalWrite(HUMIDITY_LED, LOW);
  digitalWrite(TEMPERATURE_LED, LOW);
  digitalWrite(PRESSURE_LED,LOW);
}

//Clear the memory of the Arduino chip.
//It sets all 4096 bytes to 0.
//And then set position 0 of the memory as 0,
//position 1 of the memory as 4.
void clearMemory()
{
  for(int i = 0;i<4096;i++)
    EEPROM.write(i, 0);
  EEPROM.write(0,0);
  EEPROM.write(1,2);
}

void printMemory(int limit)
{
  for(int i = 0;i<limit;i++)
  {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(EEPROM.read(i));
  }
}

void printValidMemory()
{
  int firstDigit = EEPROM.read(0);
  int secondDigit = EEPROM.read(1);
  int nextPosition = firstDigit*64+secondDigit;
  printMemory(nextPosition);
}

//Convert the temperature reading from the AD592 sensor to temperature in Kevin.
int temperatureReadingConversion(int reading)
{
   int K = reading/1024*5*1000;
   return K;
}

//Map n(0 <= n <= 1023) to a number between 0 and 256 to be stored
//as a byte.
int toByte(int n)
{
  return n/4;
}

//Save data to the non-volatile memory.
//The non-volatile memory has 4096 bytes.
//The first two bytes are used as pointer to indicate
//the next position to write.
//The actually data starts from the 5th byte.
//Three adjacent bytes are grouped together as a set of data,
//with the first byte being the humidity reaing, second being the temperatur
//and third being the pressure.
void saveData(byte b, int humidity, int temperature, int pressure)
{
  int firstDigit = EEPROM.read(0);
  int secondDigit = EEPROM.read(1);
  int nextPosition = firstDigit*64+secondDigit;
  if(nextPosition+3 >= 4096)
    return;
  EEPROM.write(nextPosition, b);
  EEPROM.write(nextPosition+1,humidity);
  EEPROM.write(nextPosition+2,temperature);
  EEPROM.write(nextPosition+3,pressure);
  secondDigit += 4;
  if(secondDigit >= 64)
  {
    secondDigit %= 64;
    firstDigit++;
  }
  EEPROM.write(0,firstDigit);
  EEPROM.write(1,secondDigit);
}
 
