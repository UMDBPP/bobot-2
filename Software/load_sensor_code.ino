// Arduino as load cell amplifier
// by Christian Liljedahl
// christian.liljedahl.dk

#define REF_OUT 23

// CALIBRATION DATA
#define LOAD_A 0.125f       // kg
#define ANALOG_VAL_A 200    // analog reading taken with load A

#define LOAD_B 0.218f       // kg
#define ANALOG_VAL_B 600    // analog reading taken with load B

float analogValueAverage = 0;
unsigned long lastReadTime = 0; 
int timeBetweenReadings = 200; // Reading every 200 ms

// Custom map function for floats
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float analogToLoad(float analogval) {
  float load = mapfloat(analogval, ANALOG_VAL_A, ANALOG_VAL_B, LOAD_A, LOAD_B);
  return load;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  int analogValue = analogRead(REF_OUT);

  // Running average - smoothing the readings
  analogValueAverage = 0.99 * analogValueAverage + 0.01 * analogValue;

  // Is it time to print?
  if (millis() > lastReadTime + timeBetweenReadings) {
    float load = analogToLoad(analogValueAverage);

    Serial.print("analogValue: "); 
    Serial.println(analogValueAverage);
    Serial.print("       load: "); 
    Serial.println(load, 5);
    
    lastReadTime = millis();
  }
}