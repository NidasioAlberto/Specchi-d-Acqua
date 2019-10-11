#include "BluetoothSerial.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

BluetoothSerial espBt;

char incoming;

void setup() {
  Serial.begin(115200);

  espBt.begin("ESP32_led_control");
  Serial.println("Bluetooth device is ready to pair");

  pinMode(13, OUTPUT);

  const uint8_t* point = esp_bt_dev_get_address();
 
  for (int i = 0; i < 6; i++) {
 
    char str[3];
 
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
 
    if (i < 5){
      Serial.print(":");
    }
 
  }
}

void loop() {
  if (espBt.available())
  {
    incoming = espBt.read();
    Serial.print("Received: "); Serial.println(incoming);
  }
}
