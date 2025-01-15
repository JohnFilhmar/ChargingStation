#include <ArduinoJson.h>
#include <SD.h>

String globalData; // Declare a global variable to store the received data
const int chipSelect = D8; // Pin connected to the SD card module's chip select (D8)

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.println("Connection established.");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed. Data will not be saved.");
    return;
  }
  Serial.println("SD card initialized.");
}

void loop() {
  if (Serial.available()) {
    globalData = Serial.readStringUntil('\n'); // Read the received data until '\n'
    globalData.trim(); // Remove leading/trailing whitespaces
    if (globalData.length() > 0) {
      Serial.println(globalData);
      saveDataToSDCard(globalData);
    }
    delay(1000);
  }
}

void saveDataToSDCard(const String& data) {
  if (data.startsWith("Temperature") || data.startsWith("Battery Voltage") || data.startsWith("Solar Panel Voltage")) {
    saveSensorData(data);
  } else if (data.startsWith("SCANNED")) {
    saveUserData(data);
  }
}

void saveSensorData(const String& data) {
  StaticJsonDocument<256> doc;
  JsonObject sensorObj = doc.to<JsonObject>();

  sensorObj["timestamp"] = getFormattedTimestamp(); // Add timestamp

  if (data.startsWith("Temperature")) {
    sensorObj["temperature"] = data.substring(data.indexOf(":") + 1);
  } else if (data.startsWith("Battery Voltage")) {
    sensorObj["battery_voltage"] = data.substring(data.indexOf(":") + 1);
  } else if (data.startsWith("Solar Panel Voltage")) {
    sensorObj["solar_panel_voltage"] = data.substring(data.indexOf(":") + 1);
  }

  File file = SD.open("sensors.json", FILE_WRITE);
  if (!file) {
    Serial.println("Error opening sensors file. Sensor data will not be saved.");
    return;
  }

  serializeJson(sensorObj, file);
  file.close();
  Serial.println("Sensor data saved to sensors.json.");
}

void saveUserData(const String& data) {
  StaticJsonDocument<256> doc;
  JsonObject userObj = doc.to<JsonObject>();

  userObj["timestamp"] = getFormattedTimestamp(); // Add timestamp

  String scannedData = data.substring(data.indexOf(":") + 1);
  scannedData.trim(); // Remove leading/trailing whitespaces
  scannedData.replace("\u0000", "-");
  userObj["scanned"] = scannedData;

  File file = SD.open("users.json", FILE_WRITE);
  if (!file) {
    Serial.println("Error opening users file. User data will not be saved.");
    return;
  }

  serializeJson(userObj, file);
  file.close();
  Serial.println("User data saved to users.json.");
}

String getFormattedTimestamp() {
  char timestamp[20];
  time_t now = time(nullptr);
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return String(timestamp);
}