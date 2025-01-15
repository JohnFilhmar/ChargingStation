#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
#define OLED_ADDR 0x3C
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int voltageSensorPin1 = 4;
int voltageSensorPin2 = 36;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
unsigned long prev = 1;
bool isDelay(int num)
{
  unsigned long curr = millis();
  if (curr - prev >= num)
  {
    prev = curr;
    return true;
  }
  else
  {
    return false;
  }
}

// Battery logo bitmap
const unsigned char battery_logo[] PROGMEM = {
  0b00000000, 0b00000000,
  0b00000111, 0b11111100,
  0b00001111, 0b11111000,
  0b00011111, 0b11100000,
  0b00111111, 0b10000000,
  0b01111111, 0b11001000,
  0b11111111, 0b11110000,
  0b11011111, 0b11100000,
  0b10001111, 0b11000000,
  0b00001111, 0b10000000,
  0b00011111, 0b00000000,
  0b00011110, 0b00000000,
  0b00111100, 0b00000000,
  0b00111000, 0b00000000,
  0b0110000, 0b00000000,
  0b01000000, 0b00000000
};

// Solar panel logo bitmap
const unsigned char solar_logo[] PROGMEM = {0b00000000, 0b00000000,
                                           0b00000001, 0b10000000,
                                           0b10000000, 0b00000001,
                                           0b01000001, 0b10000010,
                                           0b00001111, 0b11110000,
                                           0b00011111, 0b11111000,
                                           0b00011111, 0b11111000,
                                           0b11011111, 0b11111011,
                                           0b00011111, 0b11111000,
                                           0b00001111, 0b11110000,
                                           0b01000011, 0b11000010,
                                           0b10000000, 0b00000001,
                                           0b00000001, 0b10000000,
                                           0b00000000, 0b00000000,
                                           0b00000000, 0b00000000,
                                           0b00000000, 0b00000000};
// Temperature logo bitmap
const unsigned char temperature_logo[] PROGMEM = {
  0b00000011, 0b11000000,
  0b00000100, 0b00100000,
  0b00001100, 0b00110000,
  0b00011000, 0b00011000,
  0b00010000, 0b00001000,
  0b00010000, 0b00101000,
  0b00010000, 0b00101000,
  0b00010000, 0b00101000,
  0b00010000, 0b00101000,
  0b00010000, 0b00101000,
  0b00010000, 0b00101000,
  0b00110000, 0b00100100,
  0b00100000, 0b00100100,
  0b00111111, 0b11111100,
  0b00011111, 0b11111000,
  0b00001111, 0b11110000
};

float vIn, vOut, voltageSensorVal;
const float factor = 5.128, vCC = 5.00;
int numSamples = 50;
float sumMeanVoltage = 0.0;

float voltage(int pin)
{
  sumMeanVoltage = 0.0;

  for (int j = 0; j < numSamples; j++)
  {
    voltageSensorVal = analogRead(pin);
    vOut = (voltageSensorVal / 807.0) * vCC;
    vIn = vOut * factor;
    sumMeanVoltage += vOut;
  }
  float meanVoltage = sumMeanVoltage / numSamples;
  return meanVoltage;
}

void ledDisplay(float batV, float solV, float temp)
{
  display.clearDisplay();
  // Draw battery logo
  display.drawBitmap(20, 0, battery_logo, 16, 16, WHITE);
  display.setTextSize(2); // Increase the text size
  display.setCursor(40, 0);
  display.print(batV);
  display.println("V");
  // Draw solar panel logo
  display.drawBitmap(20, 25, solar_logo, 16, 16, WHITE);
  display.setTextSize(2); // Increase the text size
  display.setCursor(40, 25);
  display.print(solV);
  display.println("V");
  // Draw temperature logo
  display.drawBitmap(20, 45, temperature_logo, 16, 16, WHITE);
  display.setTextSize(2); // Increase the text size
  display.setCursor(40, 50);
  display.print(temp);
  display.println("C");
  // Display the text
  display.display();
}
String globalData;

void setup()
{
  Serial.begin(115200);   // Initialize serial communication
  while (!Serial)
  {
    ;  // Wait for serial port to connect
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  sensors.begin();

  pinMode(2, OUTPUT); // 12V RELAY MODULE
  digitalWrite(2, LOW);
  pinMode(4, OUTPUT); // 12V EXTERNAL FAN
  digitalWrite(4, LOW);
  pinMode(7, OUTPUT); // BUZZER MODULE
  digitalWrite(7, LOW);
}

void serials(float temp, float batvolt, float solvolt)
{
  Serial.print("Temperature");
  Serial.print(": ");
  Serial.print(temp);
  Serial.println("°C");
  Serial.print("Battery Voltage");
  Serial.print(": ");
  Serial.print(batvolt);
  Serial.println("V");
  Serial.print("Solar Panel Voltage");
  Serial.print(": ");
  Serial.print(solvolt);
  Serial.println("V");
  ledDisplay(batvolt, solvolt, temp);
  if (batvolt <= 12.5 && batvolt <= 14.4)
  {
    digitalWrite(2, LOW); // TURN OFF THE INVERTER IF THE BATTERY REACHES A CERTAIN TRESHOLD TO SUPPLY POWER
  }
  else if (batvolt > 13.0 && batvolt <= 18.0)
  {
    digitalWrite(2, HIGH); // TURN ON THE INVERTER IF THE BATTERY CAN SUPPLY SUFFICIENT POWER
  }
  if (temp >= 40.0)
  {
    digitalWrite(4, HIGH); // TURN ON THE FAN IF THE TEMPERATURE GETS WARM
  }
  else
  {
    digitalWrite(4, LOW); // TURN THE FAN OFF IF THE TEMPERATURE IS AVERAGE OR LOW
  }
  if (temp >= 50)
  {
    // TURN ON AND OFF THE BUZZER TO SIGNAL THAT THE TEMPERATURE IS TOO HOT
    if (isDelay(100))
    {
      digitalWrite(7, HIGH);
    }
    if (isDelay(900))
    {
      digitalWrite(7, LOW);
    }
    digitalWrite(2, LOW); // TURNS OFF THE INVERTER IF IT GETS WAY TOO HOT THAT MAKES THE GLUE STICKS THAT HOLDS SOME PART TOGETHER MAY LOSE VISCOSITY THUS MAKE IT FALL APART
  }
}

void loop()
{
    if (Serial.available()) {
      globalData = Serial.readStringUntil('\n'); // Read the received data until '\n'
      globalData.trim(); // Remove leading/trailing whitespaces
      if (globalData.length() > 0) { // Check if the string is not empty
        Serial.println(globalData);
      }
      delay(1000);
    }



    
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  float batvolt1 = voltage(voltageSensorPin1);
  float solvolt1 = voltage(voltageSensorPin2);
  serials(temp, batvolt1, solvolt1);

  // Additional sensors here
  // Example:
  // float temp2 = sensors.getTempCByIndex(1);
  // float batvolt2 = voltage(anotherVoltageSensorPin);
  // float solvolt2 = voltage(yetAnotherVoltageSensorPin);
  // serials(temp2, batvolt2, solvolt2);

  delay(5000);
}
