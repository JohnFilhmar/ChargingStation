int i = 0;

void setup() {
  Serial.begin(115200); // Initialize serial communication
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.println("Connection established.");
}

void loop() {
  if(i == 0){
    Serial.println("Hi Wemos!");
  }

  // Wait for a response from the WeMos ESP8266
  if (Serial.available()) {
    String receivedData = Serial.readString(); // Read the received data
    Serial.print(receivedData);
  }

  delay(1000);
}
