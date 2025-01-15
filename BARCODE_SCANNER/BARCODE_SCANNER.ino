#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>

class MyParser : public HIDReportParser {
public:
  MyParser();
  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);

protected:
  uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
  virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
  virtual void OnScanFinished();
  
  String scannedCode;  // Variable to store the scanned barcode
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(true, mod, key);
  scannedCode += (char)ascii;  // Append the scanned character to the scannedCode string
}

void MyParser::OnScanFinished() {
  Serial.print("SCANNED: ");  // Print "SCANNED: " before the barcode
  Serial.println(scannedCode);  // Print the complete barcode

  scannedCode.remove(0, scannedCode.length());
}

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser Parser;

void setup() {
  Serial.begin(115200);
  Serial.println("Scanner Started Successfully!");

  bool oscStarted = false;

  while (!oscStarted) {
    if (Usb.Init() == 0) {
      oscStarted = true;
      Serial.println("OSC started successfully!");
    } else {
      Serial.println("OSC did not start. Retrying...");
      delay(1000);
    }
  }

  delay(200);
  Hid.SetReportParser(0, &Parser);
}

void loop() {
  Usb.Task();
}
