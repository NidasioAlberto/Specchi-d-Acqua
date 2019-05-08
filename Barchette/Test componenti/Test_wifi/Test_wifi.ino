#include <ESP8266WiFi.h>

#define WIFI_SSID "Team_specchi"
#define WIFI_PSW "Fornaroli"

void setup() {
  //inizio la comunicazione seriale
  Serial.begin(115200);

  //mi connetto al wifi
  connettiWifi();
}

void loop() {
  //controllo la connessione
  controllaConnessione();

  delay(1000);
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
}
