#include <ESP8266WiFi.h>
#include <Servo.h>

#define RED_LED 12
#define GREEN_LED 13

#define MOTOR_PIN 2

#define WIFI_SSID "Specchi d'Acqua"
#define WIFI_PSW "Fornaroli"

WiFiServer server(1234);
Servo timone;

boolean speedC, directionC;
String speed, direction;
float speedF, directionF;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(MOTOR_PIN, OUTPUT);
  timone.attach(4);

  //first set the red led on until the esp connect to the wifi
  digitalWrite(RED_LED, HIGH);

  //begin serial communciation for debug
  Serial.begin(115200);

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

  WiFiClient client = server.available();
 
  if (client) {
    while (client.connected()) {
      if(client.available()) {
        speed = "";
        direction = "";
        while(client.available() > 0) {
          char c = client.read();

          if(c == 'S') {
            speedC = true;
            directionC = false;
          } else if(c == 'D') {
            speedC = false;
            directionC = true;
          } else if(c == '\n') {
            speedF = speed.toFloat();
            directionF = direction.toFloat() * 57296 / 1000;

            if(speedF < 30) {
              analogWrite(MOTOR_PIN, 0);
            } else {
              analogWrite(MOTOR_PIN, map((int) speedF, 0, 100, 128, 1023));
            }
            timone.write(map(directionF, 180, -180, 60, 120) );
            
            Serial.print("Speed: "); Serial.print(speedF);
            Serial.print(" direction: "); Serial.println(directionF);
            speed = "";
            direction = "";
          } else {
            if(speedC) {
              speed += c;
            } else if(directionC) {
              direction += c;
            }
          }
        }
      }
    }
 
    client.stop();
    Serial.println("Client disconnected");
 
  }
}

void connectToWifi() {
  //try to connect
  WiFi.mode(WIFI_STA); //set station mode
  WiFi.begin(WIFI_SSID, WIFI_PSW);
  server.setNoDelay(true);

  //wait until the wifi is connected
  while (WiFi.status() != WL_CONNECTED) {
    //blink the red led while connecting
    digitalWrite(RED_LED, HIGH);
    delay(250);
    digitalWrite(RED_LED, LOW);
    delay(250);
  }
  Serial.println("Wifi connection established");
  Serial.println(WiFi.localIP());

  server.begin();
}
