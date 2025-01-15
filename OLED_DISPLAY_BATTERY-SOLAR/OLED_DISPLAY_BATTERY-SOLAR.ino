#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_ADDR 0x3C // OLED display address (may vary)

Adafruit_SSD1306 display(128, 64, &Wire, -1); // Create an instance of the display object


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
  0b00000000, 0b00000000
};



void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  display.clearDisplay();
  // Draw battery logo
  display.drawBitmap(20, 10, battery_logo, 16, 16, WHITE);
  display.setTextSize(2); // Increase the text size
  display.setCursor(60, 10);
  display.println("99.9V");

  // Draw solar panel logo
  display.drawBitmap(20, 40, solar_logo, 16, 16, WHITE);
  display.setTextSize(2); // Increase the text size
  display.setCursor(60, 40);
  display.println("99.9V");

  
  // Display the text
  display.display();

  // You can add a delay here if needed
  // delay(1000);
}
