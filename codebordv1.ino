#include "SD.h"
#include <SPI.h>
#include "SparkFun_BNO080_Arduino_Library.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>

// SD
#define SD_CS 0

// IMU (I2C)
#define imu_SCL 5
#define imu_SDA 3

// GPS
#define RXD2 7
#define TXD2 6
#define GPS_BAUD 9600
HardwareSerial gpsSerial(2);


BNO080 myIMU;
Adafruit_BMP280 bmp;

void setup() {
  delay(3000);
  Serial.begin(115200);

  // SD 
  if (!SD.begin(SD_CS)) {
    Serial.println("SD non trouvé");
  }

  File f = SD.open("data.txt", FILE_WRITE);
  f.println("------------------New data-------------------");
  f.close();

  // GPS init
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("GPS active");

  // IMU init
  Wire.begin(imu_SDA, imu_SCL);
  if (!myIMU.begin()) {
    Serial.println("IMU non trouvé");
  }
  Wire.setClock(400000);
  myIMU.enableAccelerometer(50);
  myIMU.enableGyro(50);
  myIMU.enableMagnetometer(50);


  // BMP init
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 non trouvé");
  }
}

void loop() {
  File data_file = SD.open("data.txt", FILE_WRITE);

  // GPS
  if (gpsSerial.available() > 0) {
    char gpsData = gpsSerial.read();
    data_file.print("GPS:active ");
    data_file.print(gpsData);
  } else {
    data_file.print("GPS:inactive");
  }
  data_file.println();

  // IMU
  if (myIMU.dataAvailable()) {
    data_file.print("IMU:active ");
    data_file.print("ax:"); data_file.print(myIMU.getRawAccelX());
    data_file.print(" ay:"); data_file.print(myIMU.getRawAccelY());
    data_file.print(" az:"); data_file.print(myIMU.getRawAccelZ());

    data_file.print(" gx:"); data_file.print(myIMU.getRawGyroX());
    data_file.print(" gy:"); data_file.print(myIMU.getRawGyroY());
    data_file.print(" gz:"); data_file.print(myIMU.getRawGyroZ());

    data_file.print(" mx:"); data_file.print(myIMU.getRawMagX());
    data_file.print(" my:"); data_file.print(myIMU.getRawMagY());
    data_file.print(" mz:"); data_file.print(myIMU.getRawMagZ());
  } else {
    data_file.print("IMU:inactive");
  }
  data_file.println();

  // BMP
  if (bmp.begin(0x76)) {
    float temp = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0;
    float altitude = bmp.readAltitude(1013.25);

    data_file.print("BMP:active ");
    data_file.print("temp: "); data_file.print(temp);
    data_file.print(" P: "); data_file.print(pressure);
    data_file.print(" alt: "); data_file.print(altitude);
  } else {
    data_file.print("BMP:inactive");
  }
  data_file.println();
  data_file.close();

  delay(500);
}
