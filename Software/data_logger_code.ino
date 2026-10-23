#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_MS8607.h>
#include <SD.h>
#include <SPI.h>
#include "RTClib.h"
#include <Servo.h>

#define REF_OUT 23
#define LOAD_A 0.125f       // kg
#define ANALOG_VAL_A 200    // analog reading taken with load A

#define LOAD_B 0.218f       // kg
#define ANALOG_VAL_B 600    // analog reading taken with load B


RTC_PCF8523 rtc;
Adafruit_MS8607 ms8607;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
Servo s_a; 
Servo s_b;
File myFile;

// Configuration
const int chipSelect = 10; // Use BUILTIN_SDCARD for Teensy 4.0 onboard slot
const uint16_t SAMPLERATE_MS = 500;

float analogValueAverage = analogRead(REF_OUT);
unsigned long lastReadTime = 0; 
int timeBetweenReadings = 200; // Reading every 200 ms
int servo_flag = 0;

// Altitude parameters
float LAUNCH_ALT = 20; 
float LAUNCH_P = 1026;
float LAUNCH_T = 280;


bool done = false;


void setup(void) {
  Serial.begin(115200);
  while (!Serial && millis() < 4000); 
  
  // BNO055 Setup
  if (!bno.begin()) {
    Serial.println("BNO055 NOT detected!");
    while (1);
  }
  
  // RTC Setup
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("RTC is NOT initialized, let's set the time!");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.start();
  
  //MS8607 Setup
  if (!ms8607.begin()){
    Serial.println("Failed to begin MS8607");
    while(1);
  }
  ms8607.setHumidityResolution(MS8607_HUMIDITY_RESOLUTION_OSR_8b);
  
  // SD Card Setup
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Initialization FAILED!");
    while (1);
  }

  //Setting Servos to closed positon
  s_a.attach(3);
  s_b.attach(2);

  s_a.write(70);
  s_b.write(30);


  // File Initialization & Headers
  myFile = SD.open("datalog_ns146_log.txt", FILE_WRITE);
  if (myFile) {
    // Write all headers on the first line
    myFile.print("Date,");
    myFile.print("Time,");
    myFile.print("OrientX,OrientY,OrientZ,");
    myFile.print("GyroX,GyroY,GyroZ,");
    myFile.print("LinearX,LinearY,LinearZ,");
    myFile.print("MagX,MagY,MagZ,");
    myFile.print("AccelX,AccelY,AccelZ,");
    myFile.print("GravX,GravY,GravZ,");
    myFile.print("Temp_C,");
    myFile.print("Pressure_hPa,");
    myFile.print("Humidity_%rH, ");
    myFile.print("Analog_Value, ");
    myFile.print("Load_Value, ");
    myFile.println("Altitude");
    myFile.flush(); 
    Serial.println("Logging started to datalog.txt...");
  } else {
    Serial.println("Could not open file!");
  }

  bno.setExtCrystalUse(true);

}

void loop(void) {
  
  //When Servos open, stop the loop
  if (done) return;
  if (!myFile) return;

  // Fetch all vectors from the sensor
  imu::Vector<3> orient = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> gyro   = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  imu::Vector<3> linAcc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  imu::Vector<3> mag    = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  imu::Vector<3> accel  = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> grav   = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  int8_t temp           = bno.getTemp();
  
  DateTime now = rtc.now();
  sensors_event_t temp1, pressure, humidity;
  ms8607.getEvent(&pressure, &temp1, &humidity);

  // --- Start Writing One Row of Data ---
  myFile.print(now.year(), DEC);
  myFile.print('/');
  myFile.print(now.month(), DEC);
  myFile.print('/');
  myFile.print(now.day(), DEC);
  myFile.print(",");

  myFile.print(now.hour(), DEC);
  myFile.print(':');
  myFile.print(now.minute(), DEC);
  myFile.print(':');
  myFile.print(now.second(), DEC);

  // Helper function calls to print X, Y, Z for each vector
  writeVectorToSD(orient);
  writeVectorToSD(gyro);
  writeVectorToSD(linAcc);
  writeVectorToSD(mag);
  writeVectorToSD(accel);
  writeVectorToSD(grav);

  // Final piece of data (Temp) and a new line
  myFile.print(temp);
  myFile.print(",");
  myFile.print(pressure.pressure); 
  myFile.print(",");
  myFile.print(humidity.relative_humidity);
  myFile.print(",");



  // Load Sensor  
  analogValueAverage = 0.99 * analogValueAverage + 0.01 *analogRead(REF_OUT);
  float load = analogToLoad(analogValueAverage);
  myFile.print(analogValueAverage);
  myFile.print(",");
  myFile.print(load,5);
  myFile.print(",");

  //Altitude
  float alt = altCalc(pressure.pressure);
  myFile.println(alt);
  //Test
  Serial.println(alt);

  //servos open at particular altitude
  float deployment_altitude = 30000;
  if (deployment_altitude <= alt){
    servo_run();
    done = true;
  }
  // Ensure data is saved
  myFile.flush();

  Serial.print("Logged at "); Serial.print(millis()); Serial.println(" ms");
  delay(SAMPLERATE_MS);
}

// Helper function to print a vector to the file with commas
void writeVectorToSD(imu::Vector<3> v) {
  myFile.print(v.x()); myFile.print(",");
  myFile.print(v.y()); myFile.print(",");
  myFile.print(v.z()); myFile.print(",");
}
void servo_run(void){

  //closed
  s_a.write(70);
  s_b.write(30);

  delay(1000);
  //open
  s_a.write(30);
  s_b.write(70);

  //closed
  delay(1000);
  s_a.write(70);
  s_b.write(30);

}

// Custom map function for floats
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float analogToLoad(float analogval) {
  float load = mapfloat(analogval, ANALOG_VAL_A, ANALOG_VAL_B, LOAD_A, LOAD_B);
  return load;
}
float altCalc(float P) {
  // Inputs: P in hPa
  const float g = 9.80665;  // m/s^2
  const float R = 287.05;   // J/(kg·K)
  // Layer 1: Troposphere (0–11 km)
  if (P > 226.32) {
    float T0 = 288.15;  //standard atmospheric temperature
    float P0 = LAUNCH_P * exp((g * LAUNCH_ALT) / (R * LAUNCH_T));
    float L = fitLapseRate(P0, T0);
    float H0 = LAUNCH_ALT;
    float alt = (1 / L) * (T0 * pow(P / P0, (-R * L) / (g)) - T0) + H0;
    return alt;
  }
  // Layer 2: Tropopause (11–20 km)
  else if (P > 54.75) {
    float T0 = 216.65;
    float P0 = 226.32;
    float H0 = 11000.0;
    float alt = H0 + (R * T0 / (g)) * log(P0 / P);
    return alt;
  }
  // Layer 3: Lower Stratosphere (20–32 km)
  else {
    float T0 = 216.65;
    float P0 = 54.75;
    float H0 = 20000.0;
    float L = 0.001;
    float alt = (1 / L) * (T0 * pow(P / P0, (-R * L) / (g)) - T0) + H0;
    return alt;
  }
}

float fitLapseRate(float P0_local, float T0_local) {
  float L = -0.0065;  // initial guess
  float tol = 1e-6;
  int max_iter = 50;
  float P11 = 226.32;
  float h11 = 11000.0;
  const float g = 9.80665;  // m/s^2
  const float R = 287.05;   // J/(kg·K)

  for (int i = 0; i < max_iter; i++) {
    float f = P0_local * pow((T0_local + L * h11) / T0_local, -g / (R * L)) - P11;
    // numerical derivative
    float dL = 1e-5;
    float f_prime = (P0_local * pow((T0_local + (L + dL) * h11) / T0_local, -g / (R * (L + dL))) - P11 - f) / dL;
    L -= f / f_prime;
    if (fabs(f) < tol) break;
  }
  return L;
}
