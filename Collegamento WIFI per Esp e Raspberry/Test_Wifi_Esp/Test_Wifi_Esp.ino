#include <ESP8266WiFi.h>

#define RED_LED 14
#define GREEN_LED 12

#define WIFI_SSID "Specchi d'Acqua"
#define WIFI_PSW "Fornaroli"

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  //first set the red led on until the esp connect to the wifi
  digitalWrite(RED_LED, HIGH);

  //begin serial communciation for debug
  Serial.begin(9600);

  connectToWifi();

  //turn on the green led
  digitalWrite(GREEN_LED, HIGH);
}

void loop() {
  //check wifi connection
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost");

    //turn off the green led
    digitalWrite(GREEN_LED, LOW);
    
    //there's an error !
    connectToWifi();

    //turn on the green led
    digitalWrite(GREEN_LED, HIGH);
  }
}

void connectToWifi() {
  //try to connect
  WiFi.mode(WIFI_STA); //set station mode
  WiFi.begin(WIFI_SSID, WIFI_PSW);

  //wait until the wifi is connected
  while (WiFi.status() != WL_CONNECTED) {
    //blink the red led while connecting
    digitalWrite(RED_LED, HIGH);
    delay(250);
    digitalWrite(RED_LED, LOW);
    delay(250);
  }
  Serial.println("Wifi connection established");
}
