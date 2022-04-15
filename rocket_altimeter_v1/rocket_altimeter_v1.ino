#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;


// change this for your particular board of use (pins varry for mega, uno, mini etc)
int chip_select = 53;

//define global variables
File myFile;
String fileName = "accelx5.csv";

// !!!! change this on the flight day
float local_pressure = 1022.0;
unsigned long time;
int falling_count;
//unsigned float current_alt;
//unsigned float previous_alt;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while ( !Serial ) delay(100);   // wait for native usb

  Serial.print("Initializing SD card...");

  if (!SD.begin(chip_select)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  Serial.println("-------------------");

  //Serial.println(F("BMP280 and MPU test"));
  bool status1;
  bool status2;
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status1 = bmp.begin(0x76);
  status2 = mpu.begin(0x68);


  if (!status1 && !status2) {
    Serial.println(F("Could not find a valid BMP280 or MPU sensor"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");

}

void loop() {
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    Serial.print("Opened file, begin writing data...");
  } else {
    Serial.print("error opening file");
    exit(0);
  }
  //Serial.print("Opened file, begin writing data...");
  //time = millis();
  float accel_x;
  float altitude;
  int j = 0;
  //condition in while statement will have to do with the rocket flying, the while loop will exit when the rocket has confirmed landed
  //write functions for different stages of flight, let them return BOOLEAN
  while (j <= 2000) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    accel_x = a.acceleration.x;
    altitude = bmp.readAltitude(local_pressure);
    time = millis();
    writeSD(time,altitude,accel_x);
    //myFile.println((String)time + "," + (String)accel_x + "," + (String)altitude);
    j++;
    delay(10);
  }

  myFile.close();
  Serial.print("-------------");
  Serial.print("file written and closed");
  delay(1000);
  exit(0);

  

}



//returns true if appogee has been detected
//takes 100 consecutive measurements at 10ms per measurement results in
//1 second of consistent falling to trigger appogee

/*
bool checkApogee() {
  if(current_alt < previous_alt){
    falling_count++;
  } else {
    falling_count = 0;
  }
  if(falling_count == 100){
    return true
  }
  
}
*/

/*
  float local_pressure = 1015.24;
  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(local_pressure)); /* Adjusted to local forecast!
  Serial.println(" m");

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values
  Serial.print("AccelX:");
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print("AccelY:");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print("AccelZ:");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print("GyroX:");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print("GyroY:");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print("GyroZ:");
  Serial.print(g.gyro.z);
  Serial.println("");
*/





  void writeSD(unsigned long t, float alt_meas, float accelx) {
    myFile = SD.open(fileName, FILE_WRITE);
    myFile.println((String)t + "," + (String)alt_meas + "," + (String)accelx);
    myFile.close();
  }
