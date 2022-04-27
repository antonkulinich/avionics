//////////////////////////////////////////////
// Apogee Detection And Safe Dual-Deployment//
// Pyrotechnic Activation                   //
//                                          //
//      ENGINEERS:                          //
//    Anton Kulinich (M.S. AE)              //
//    Dan Sullivan (B.S. AE)                //
//                                          //
//      DATE:                               //
//    4/13/2022                             //
//                                          //
//      DESCRIPTION:                        //
//   The program is designed to detect the  //
//   pressure and altitude using a BMP280.  //
//   With the altitude detected upon        //
//   program startup, the program will      //
//   determine the minimum safe altitude    //
//   at which the onboard pyrotechnics      //
//   may fire. The pyrotechnics are to be   //
//   fired when (a) apogee is detected      //
//   within 1 meter, (b) the rocket is      //
//   above the minimum safe distance, and   //
//   (c)[main parachute only] when the      //
//   rocket is between the minimum safe     //
//   distance and 100 meters above the      //
//   minimum safe distance.                 //
//                                          //
//       NOTES:                             //
//    Further optimizations are needed.     //
//    Current sketch uses:                  //
//        PROGRAM STORAGE SPACE: 90%        //
//        GLOBAL VARIABLES:      66%        //
//                                          //
//                                          //
//////////////////////////////////////////////



/////////////////////////////////////////////
// Library Declaration                     //
/////////////////////////////////////////////


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>



/////////////////////////////////////////////
// Sensor Declaration                      //
/////////////////////////////////////////////



Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;



/////////////////////////////////////////////
// Variable Declaration                    //
/////////////////////////////////////////////


int chip_select = 8;
int falling_count;
float current_alt;
int previous_alt;                         /////////////////////////////////////
//////////////////////////////////////////// ADJUST LOCAL PRESSURE LAUNCH DAY//
int local_pressure = 1023.00;             /////////////////////////////////////
int ref_alt;
File myFile;
String fileName = "accelx4.csv";
unsigned long time;
bool MarkApge = false;
//int safedist = 500 * (1 / 3.281);
int safedist = 128;
int safealt = ref_alt + safedist;
//int armdist = 300 * (1 / 3.281);
int armdist = 129;
int armalt = ref_alt + armdist;


/////////////////////////////////////////////
// Single-Run & setup                      //
/////////////////////////////////////////////



void setup() {
  //unsigned status1;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  dosh();
  dash();
  dot();
  poweronblink();
  Serial.begin(115200);
  // put your setup code here, to run once:
  while (!Serial) delay(100); // wait for native usb

  Serial.print(F("Initializing SD Card..."));
  if (!SD.begin(chip_select)) {
    Serial.println(F("SD card initialization failed!"));
    while (1);
  }
  Serial.println(F("initialization done."));
  Serial.println(F("--------------------"));
  Serial.print(F("beginning apogee detection test"));
  bool status1 = bmp.begin(0x76);
  bool status2 = mpu.begin(0x68);
  if (!status1 && !status2) {
    Serial.println(F("Could not find a valid BMP280 or MPU sensor"));
    while (1) delay(10);
  }



  /////////////////////////////////////////////
  // Activate BMP Oversampling               //
  /////////////////////////////////////////////



  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */


  /////////////////////////////////////////////
  // Activate MPU                            //
  /////////////////////////////////////////////



  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");


  /////////////////////////////////////////////
  // Determine reference altitude            //
  /////////////////////////////////////////////

  int sum;
  for (byte i = 0; i < 100; i++)
  {
    sum += current_alt;
    current_alt = bmp.readAltitude(local_pressure);
  }
  sum = sum / 100;
  ref_alt = sum;



  /////////////////////////////////////////////
  // Determine safety altitude from current  //
  /////////////////////////////////////////////

  Serial.println(F("\nReference Altitude Completed:"));
  Serial.print(ref_alt);
  Serial.println(F("\nSafety Altitude:"));
  Serial.print(safealt);
}

/////////////////////////////////////////////
// Repeated Functions placed here          //
/////////////////////////////////////////////



void loop() {

  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    Serial.println(F(" Opened file, begin writing data..."));
  } else {
    Serial.print(F("error opening file"));
    exit(0);
  }

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
    myFile.println((String)time + "," + (String)accel_x + "," + (String)altitude);
    j++;
    delay(10);
  }
  myFile.close();
  Serial.print("-------------");
  Serial.print("file written and closed");
 
  current_alt = bmp.readAltitude(local_pressure);
  Serial.print(altitude);
  // Serial.println(current_alt);
  apogeeCheck();
  previous_alt = current_alt;
}



/////////////////////////////////////////////
// Determining when rocket is at apogee    //
/////////////////////////////////////////////



void apogeeCheck() {
  if (current_alt < previous_alt) {
    falling_count++;
    //Serial.print("lower altitude detected");
    //Serial.print("\n");
  } else {
    falling_count = 0;
    //   Serial.println("altitude climbing...");
  }
  if (falling_count == 1 && current_alt > safealt) {
    Serial.println(F("appogee_reached"));
    (MarkApge = true);
    dash();
    dash();
    dash();
  }
  if (falling_count == 1 && current_alt > safealt && current_alt < armalt){
    Serial.println(F("Firing 2nd charge"));
    dot();
    dot();
    dot();
  }
}

//////////////////////////////////////////////////
// Activate onboard LED to signal program start //
//////////////////////////////////////////////////



void poweronblink() {
  dosh();
  dosh();
  dosh();
}



/////////////////////////////////////////////
// Apogee pyro                             //
/////////////////////////////////////////////



//void apogeeignition() {
  
//  if (current_alt > safealt && falling_count >= 1) {
//    Serial.print(F("Apogee deployment alt:"));
//    Serial.print(current_alt);
//    dash();
//    dash();
//    dash();

//  }
//}



/////////////////////////////////////////////
// Main Parachute Pyro                     //
/////////////////////////////////////////////


//void pyroignition() {
    
//  if (current_alt > safealt && falling_count >= 1 && current_alt < armalt) {
//    Serial.print(F("2nd deployment alt:"));
//    Serial.print(current_alt);
//    dot();
//    dot();
//    dot();
//  }
//}


/////////////////////////////////////////////
// Reducing memory usage through functions //
/////////////////////////////////////////////
void dot()
{
  digitalWrite(10, HIGH);
  delay(3000);
  digitalWrite(10, LOW);
  delay(250);
}

void dash()
{
  digitalWrite(4, HIGH);
  delay(3000);
  digitalWrite(4, LOW);
  delay(250);
}

void dosh()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
