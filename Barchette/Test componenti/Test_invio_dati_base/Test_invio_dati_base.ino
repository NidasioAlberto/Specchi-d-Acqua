#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Impostazioni WIFI
#define WIFI_SSID "Team_specchi"
#define WIFI_PSW "Fornaroli"

//Impostazioni MQTT
#define MQTT_USR "barca1"
#define MQTT_PSW "barca1"
#define MQTT_ADDR "192.168.0.1"
#define MQTT_PORT 1883
#define MQTT_DATA_TOPIC "records"

WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup() {
  Serial.begin(115200);
  
  connettiWifi();

  connettiMqtt();
}

void loop() {
  //controllo la connessione
  controllaConnessione();

  //invio un messaggio di prova
  String messaggio = "Ciao questo è un messaggio di prova";
  char payload[messaggio.length() + 1];
  messaggio.toCharArray(payload, messaggio.length() + 1);
  mqtt.publish(MQTT_DATA_TOPIC, payload);

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

void connettiMqtt() {
  //imposto l'indirizzo del server e la porta
  mqtt.setServer(MQTT_ADDR, MQTT_PORT);

  //provo a connettermi con username e password, se non funziona aspetto 2 secondi e riprovo
  while(!mqtt.connected()) if(!mqtt.connect(MQTT_USR, MQTT_USR, MQTT_PSW)) {
    Serial.println("Connessione mqtt fallita, riprovo tra due secondi");
    delay(2000);
  }
}
