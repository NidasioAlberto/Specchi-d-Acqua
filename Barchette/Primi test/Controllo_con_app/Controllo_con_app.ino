#include <ESP8266WiFi.h>

#define RED_LED 12
#define GREEN_LED 14

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

WiFiServer server(1234);

boolean velocitaC, angoloC, direzioneC;
String velocita, angolo, direzione;
float speedF;
int angoloI;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

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
        velocita = "";
        angolo = "";
        direzione = "";

        velocitaC = true;
        angoloC = false;
        direzioneC = false;

        while(client.available() > 0) {
          char c = client.read();

          Serial.write(c);
          
          if(c == ',') {
            if(velocitaC) {
              velocitaC = false;
              angoloC = true;
            } else if(angoloC) {
              angoloC = false;
              direzioneC = true;
            }
          } else if(c == ';') {
            Serial.print("Velovità: "); Serial.print(velocita.toInt());
            Serial.print(" angolo: "); Serial.print(angolo.toInt());
            Serial.print(" direzione: "); Serial.println(direzione.toInt());
          } else {
            if(velocitaC) {
              velocita += c;
            } else if(angoloC) {
              angolo += c;
            } else if(direzioneC) {
              direzione += c;
            }
          }
        }
      }
    }
 
    client.stop();
    Serial.println("Client disconnected");
    accendiRosso();
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
