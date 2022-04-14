#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>

#include <Wire.h>

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

int falling_count;
float current_alt;
float previous_alt;
float local_pressure = 1023.00;
unsigned status1;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  Serial.print("beginning apogee detection test");
  status1 = bmp.begin(0x76);
  if (!status1) {
    Serial.println(F("Could not find a valid BMP280 or MPU sensor"));
    while (1) delay(10);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */

}



void loop() {
  // put your main code here, to run repeatedly:
  current_alt = bmp.readAltitude(local_pressure);
  //Serial.println(current_alt);
  apogeeCheck();
  previous_alt = current_alt;
  delay(500);
}


void apogeeCheck(){
  if(current_alt < previous_alt){
    falling_count++;
    //Serial.print("lower altitude detected");
    //Serial.print("\n");
  } else {
    falling_count = 0;
    Serial.println("altitude climbing...");
  }
  if(falling_count == 3){
    Serial.println("appogee_reached");
  }
}
