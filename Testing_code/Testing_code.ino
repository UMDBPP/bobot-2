//Libraries
#include<MS5607.h>
#include<Wire.h>

MbedI2C i2c(6,7);

//Objects
MS5607 P_Sens;

//GPIO Declarations
int SPIO_POCI = 0;
int SPIO_CS_RAD = 1;
int SPIO_SCK = 2;
int SPIO_PICO = 3;
int CROSS_PRIMTX_REDRX = 4;
int CROSS_PRIMRX_REDTX = 5;
int SDA = 6;
int SCL = 7;
int D1 = 8;
int SPIO_CS_IMU = 9;
int LS2 = 10;
int PWM_MC = 11;
int GPS_RX = 12;
int GPS_TX = 13;
int LS1 = 14;
int CH_1A = 15;
int CH_1B = 16;
int CH_2A = 17;
int CH_2B = 18;
int CH_3A = 19;
int CH_3B = 20;
int SPIO_CS_SD = 21;
int SW = 22;
int DIO1 = 23;
int BUSY = 24;
int RST = 25;
int TXEN = 26;
int TIME_PULSE = 27;
int STEP = 28;
int DIRT = 29;

//Variables
float P_val,T_val,H_val;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600); //Start Serial comms
  if(!P_Sens.begin()){
    Serial.println("Error in Communicating with sensor, check your connections!");
  }else{
    Serial.println("MS5607 initialization successful!");
  }
}

// the loop function runs over and over again forever
void loop() {
   if(P_Sens.readDigitalValue()){
    T_val = P_Sens.getTemperature();
    P_val = P_Sens.getPressure();
    H_val = P_Sens.getAltitude();
    Serial.print("Temperature :  ");
    Serial.print(T_val);
    Serial.println(" C");
    Serial.print("Pressure    :  ");
    Serial.print(P_val);
    Serial.println(" mBar");
    Serial.print("Altitude    :  ");
    Serial.print(H_val);
    Serial.println(" meter");
  }else{
    Serial.println("Error in reading digital value in sensor!");
  }
  delay(5000);
}
