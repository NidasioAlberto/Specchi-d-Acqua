#include <ESP8266WiFi.h>
#include <Servo.h>

#define RED_LED 12
#define GREEN_LED 14

#define FORWARD_PIN D1
#define BACKWARD_PIN D3
#define ENABLE_PIN D4
#define MOTOR_PIN 2

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

WiFiServer server(1234);
Servo timone;

boolean speedC, directionC;
String speed, direction;
float speedF, directionF;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(FORWARD_PIN, HIGH);
  digitalWrite(BACKWARD_PIN, LOW);
  
  timone.attach(D7);

  //first set the red led on until the esp connect to the wifi
  digitalWrite(RED_LED, HIGH);

  //begin serial communciation for debug
  Serial.begin(115200);

  connectToWifi();
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
 
  if(client) {
    accendiVerde();
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

            if(directionF > 90 || directionF < -90) indietro();
            else avanti();

            if(speedF < 30) {
              analogWrite(ENABLE_PIN, 0);
            } else {
              analogWrite(ENABLE_PIN, map((int) speedF, 0, 100, 0, 1023));
            }

            //nuovo sistema per lo sterzo!
            float angolo = 0;

            if(directionF < 90 && directionF > -90) angolo = map(directionF, -90, 90, 60, 120);
            if(directionF > 90 && directionF < 180) angolo = map(directionF, 90, 180, 60, 90);
            if(directionF < -90 && directionF > -180) angolo = map(directionF, -180, -90, 90, 120);

            timone.write(angolo);
            /*if(directionF > 30) timone.write(130);
            else if(directionF < -30) timone.write(70);
            else timone.write(100);*/
              //timone.write(map(directionF, -180, 180, 60, 120));
            
            Serial.print("Speed: "); Serial.print(speedF);
            Serial.print(" angle: "); Serial.print(angolo);
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
    accendiRosso();
    fermati();
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

void accendiRosso() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

void accendiVerde() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
}

void avanti() {
  digitalWrite(FORWARD_PIN, HIGH);
  digitalWrite(BACKWARD_PIN, LOW);
}

void indietro() {
  digitalWrite(FORWARD_PIN, LOW);
  digitalWrite(BACKWARD_PIN, HIGH);
}

void fermati() {
  digitalWrite(FORWARD_PIN, LOW);
  digitalWrite(BACKWARD_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
  timone.write(100);
}
