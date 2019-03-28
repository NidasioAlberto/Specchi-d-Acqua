#include <ESP8266WiFi.h>

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

#define TCP_SERVER_PORT 1234

WiFiServer server(TCP_SERVER_PORT);
WiFiClient client;

boolean velocitaC, angoloC, direzioneC;
String velocita, angolo, direzione;

void setup() {
  //inizio la comunicazione seriale
  Serial.begin(115200);
  
  //mi connetto al wifi
  connettiWifi();
}

void loop() {
  //controllo la connessione
  controllaConnessione();


  //controlllo se c'è qualcuno che vuole controllare la barchetta
  if(client) {
    while(client.connected()) {    
      //controllo lo stato del wifi
      controllaConnessione();
      
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
            Serial.print("Velovità: "); Serial.print(velocita);
            Serial.print(" angolo: "); Serial.print(angolo);
            Serial.print(" direzione: "); Serial.println(direzione);
            break;
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
  } else {
    client = server.available();
  }
}

void controllaConnessione() {
  //controlla la connessione wifi
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connessione persa");
    
    //c'è un errore
    connettiWifi();
  }
}

void connettiWifi() {
  //provo a connettermi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSW);

  //aspetto finchè non siamo connessi al wifi
  while(WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("Connessione wifi stabilita");
  Serial.println("Indirizzo IP: ");
  Serial.println(WiFi.localIP());

  //avvio il server tcp
  server.begin();
}
