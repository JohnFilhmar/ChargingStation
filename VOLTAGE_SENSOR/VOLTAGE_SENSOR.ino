const int voltageSensorPin = 2;
float vIn;
float vOut;
float voltageSensorVal;
const float factor = 5.128;
const float vCC = 5.00;
int numSamples = 50;  
int numMeans = 30; 
float sumMeanVoltage = 0.0; 

void setup() {
  Serial.begin(9600);
}

void loop() {
  sumMeanVoltage = 0.0; 
 
  for (int i = 0; i < numMeans; i++) {
    float sumVoltage = 0.0;   
    for (int j = 0; j < numSamples; j++) {
      voltageSensorVal = analogRead(voltageSensorPin);
      vOut = (voltageSensorVal / 807.0) * vCC;
      vIn = vOut * factor;
      sumVoltage += vIn;  
    }
    float meanVoltage = sumVoltage / numSamples; 
    sumMeanVoltage += meanVoltage; 
  }

  float finalMean = sumMeanVoltage / numMeans; 

  Serial.print("Final Mean Voltage = ");
  Serial.print(finalMean);
  Serial.println("V");
  delay(1000);
}
