#include <SD.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>

const char* ssid = "2F1-SolarCharge";  // SSID (name) of the Wi-Fi network
const char* password = "2F1Solar";  // Password for the Wi-Fi network
const int chipSelect = D8;  // Set SD card chip select pin

ESP8266WebServer server(80);
bool isAdminLoggedIn = false;

void handleRoot();
void handleCustomFile();
void handleLogin();



void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set ESP8266 as Wi-Fi access point (hotspot)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 6, false, 1);  // SSID, password, channel, hidden, max connections

  Serial.println("Wi-Fi access point created");

  // Start the SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card Initialization Failed");
    return;
  }

  // Print the IP address of the access point
  Serial.print("Access point IP address: ");
  Serial.println(WiFi.softAPIP());

  // Route handlers
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/restart", handleRestart);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.onNotFound(handleCustomFile);
  server.handleClient();
}





void handleRestart() {
  server.send(200, "text/plain", "Restarting...");
  delay(1000);
  ESP.restart();
}


void handleRoot() {
  // Check if the admin is logged in
  if (isAdminLoggedIn) {
    String filePath = server.uri();

    if (filePath.endsWith("/"))
      filePath += "home.html";

    if (SD.exists(filePath)) {
      File file = SD.open(filePath, "r");
      if (file) {
        if (filePath.endsWith(".html"))
          server.streamFile(file, "text/html");
        else if (filePath.endsWith(".css"))
          server.streamFile(file, "text/css");
        else if (filePath.endsWith(".js"))
          server.streamFile(file, "application/javascript");
        else {
          server.send(500, "text/plain", "Invalid file type");
          Serial.println("Invalid file type: " + filePath);
        }
        file.close();
        return;
      } else {
        Serial.println("Failed to open file: " + filePath);
      }
    } else {
      Serial.println("File not found: " + filePath);
    }
  } else {
    server.sendHeader("Location", "/login");
    server.send(302, "text/plain", "Redirecting to login page...");
  }
}

void handleCustomFile() {
  String filePath = server.uri();

  if (SD.exists(filePath)) {
    File file = SD.open(filePath, "r");

    if (file) {
      if (filePath.endsWith(".html")) {
        server.streamFile(file, "text/html");
      } else if (filePath.endsWith(".css")) {
        server.streamFile(file, "text/css");
      } else if (filePath.endsWith(".js")) {
        server.streamFile(file, "application/javascript");
      } else {
        server.send(500, "text/plain", "Invalid file type");
        Serial.println("Invalid file type: " + filePath);
      }

      file.close();
      return;
    } else {
      Serial.println("Failed to open file: " + filePath);
    }
  } else {
    Serial.println("File not found: " + filePath);
  }
}

void handleLogin() {
  if (server.method() == HTTP_GET) {
    // Serve the login page
    File loginPage = SD.open("/login.html");
    if (loginPage) {
      server.send(200, "text/html", loginPage.readString());
      loginPage.close();
    } else {
      Serial.println("Failed to open login page");
      server.send(404, "text/plain", "File Not Found");
    }
  } else if (server.method() == HTTP_POST) {
    // Handle login form submission
    if (server.hasArg("username") && server.hasArg("password")) {
      String username = server.arg("username");
      String password = server.arg("password");

      // Perform authentication logic here (e.g., check username and password against a stored value)
      if (username == "admin" && password == "solarcharge2F1") {
        isAdminLoggedIn = true;
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
        return;
      }
    }

    server.send(401, "text/plain", "Authentication Failed");
  }
}
