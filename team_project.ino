#include <MQUnifiedsensor.h>


#define       Board               "Arduino UNO"
const float   V_REF             = 5.0;     
const uint8_t ADC_BITS          = 10;      
const int     MQ6_PIN           = A0;      
const float   RL_VALUE          = 10.0;    

const float   LPG_A             = 39.062;  
const float   LPG_B             = -1.518;


const uint8_t CAL_SAMPLES  = 20;           
const uint16_t CAL_DELAY   = 500;          
const float   RATIO_CLEAN_AIR = 10.0;      

MQUnifiedsensor MQ6(Board, V_REF, ADC_BITS, MQ6_PIN, "MQ‑6");

void setup() {
  Serial.begin(9600);

  MQ6.setRL(RL_VALUE);
  MQ6.setRegressionMethod(1);         
  MQ6.setA(LPG_A);
  MQ6.setB(LPG_B);

 
  Serial.print(F("{\"status\":\"calibrating\",\"samples\":"));
  Serial.print(CAL_SAMPLES);
  Serial.println(F("}"));

  float rs_sum = 0;
  for (uint8_t i = 0; i < CAL_SAMPLES; ++i) {
    int adc = analogRead(MQ6_PIN);
    float vout = adc * (V_REF / 1023.0);
    float rs = (V_REF - vout) * RL_VALUE / vout; 
    rs_sum += rs;
    delay(CAL_DELAY);
  }
  float rs_avg = rs_sum / CAL_SAMPLES;
  float R0 = rs_avg / RATIO_CLEAN_AIR;
  MQ6.setR0(R0);

  Serial.print(F("{\"status\":\"R0\",\"value\":"));
  Serial.print(R0, 2);
  Serial.println(F("}"));
  Serial.println(F("{\"status\":\"ready\"}"));
}

void loop() {
  MQ6.update();                      

  float ppm  = MQ6.readSensor();       
  int   raw  = analogRead(MQ6_PIN);    
  float vout = raw * (V_REF / 1023.0);

  Serial.print('{');
  Serial.print("\"raw\":");     Serial.print(raw);
  Serial.print(",\"ppm\":");     Serial.print(ppm, 1);
  Serial.print(",\"voltage\":"); Serial.print(vout, 2);
  Serial.println('}');

  delay(250); 
}
