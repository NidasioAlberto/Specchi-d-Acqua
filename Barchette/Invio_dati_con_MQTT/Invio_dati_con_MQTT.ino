#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//se non è possibile inviare dei pacchetti di dimensioni maggiori di 128 bye cambiare MQTT_MAX_PACKET_SIZE nel file .h della libreria

//Probe
#include <OneWire.h>
#include <DallasTemperature.h>

//DHT11
#include <DHT.h>

//GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//JSON
#include <ArduinoJson.h>
const size_t capacity = JSON_ARRAY_SIZE(3) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(8) + 260;

//Impostazioni WIFI
#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

//Impostazioni MQTT
#define MQTT_USR "barca1"
#define MQTT_PSW "barca1"
#define MQTT_ADDR "192.168.4.100"
#define MQTT_PORT 1883
#define MQTT_DATA_TOPIC "records"

//Pin componenti
#define PROBE_PIN 1 //GPIO1 - TX
#define DHT_PIN 3 //GPIO3 - RX
#define GPS_PIN 15 //GPIO15 - D8

//Impostazioni invio dati
#define ID_BARCHETTA "1"
#define ID_PROBE "1"
#define ID_DHT11_TEMP "2"
#define ID_DHT11_HUM "3"

//Istanze dei componenti
WiFiClient espClient;
PubSubClient mqtt(espClient);
OneWire oneWire(PROBE_PIN);
DallasTemperature probe(&oneWire);
DHT dht(DHT_PIN, DHT11);
TinyGPSPlus gpsParser;
SoftwareSerial gps(GPS_PIN, GPS_PIN);

//Dati e variabili
float tempAria, umidAria, tempAcqua;

long ultimaVolta;
float frequenzaDati = 1; //in Hz

void setup() {
  connettiWifi();

  connettiMqtt();

  setupSensori();
  
  gps.begin(9600);
}

void loop() {
  //aggiorno i dati del GPS
  aggiornaGps();

  //controllo lo stato del wifi
  controllaWifi();

  //controllo la connessione mqtt
  controllaMqtt();

  //ogni n secondi leggo i dati e li invio
  if(millis() - ultimaVolta > (1 / frequenzaDati) * 1000) {
    //record and send sensors data
    leggiSensori();
    inviaDati();
    
    ultimaVolta = millis();
  }
  
  mqtt.loop();
}

void connettiWifi() {
  //provo a connettermi
  WiFi.mode(WIFI_STA); //imposto la modalità di funzionamento
  WiFi.begin(WIFI_SSID, WIFI_PSW); //imposto ssid e password

  //aspetto fino a quando siamo connessi
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

void connettiMqtt() {
  //imposto l'indirizzo del server e la porta
  mqtt.setServer(MQTT_ADDR, MQTT_PORT);

  //provo a connettermi con username e password, se non funziona aspetto 2 secondi e riprovo
  while(!mqtt.connected()) if(!mqtt.connect(MQTT_USR, MQTT_USR, MQTT_PSW)) delay(2000);
}

void setupSensori() {
  probe.begin();
  dht.begin();
}

void leggiSensori() {
  probe.requestTemperatures();
  tempAcqua = probe.getTempCByIndex(0);
  tempAria = dht.readTemperature();
  umidAria = dht.readHumidity();
}

void aggiornaGps() {
  if(gps.available() > 0) {
    while(gps.available() > 0) {
      gpsParser.encode(gps.read());
    }
  }
}

void controllaWifi() {
  if(WiFi.status() != WL_CONNECTED) {
    //provo a riconnettermi
    connettiWifi();
    connettiMqtt();
  }
}

void controllaMqtt() {
  if(mqtt.state() != MQTT_CONNECTED) {
    //provo a ricollegarmi
    connettiMqtt();
  }
}

void inviaDati() {
  DynamicJsonDocument record(capacity);
  
  //JsonObject record = doc.createNestedObject("record");
  record["idBarchetta"] = ID_BARCHETTA;
  record["latitudine"] = gpsParser.location.lat(); //String(gpsParser.location.lat(), 6); 
  record["longitudine"] = gpsParser.location.lng(); //String(gpsParser.location.lng(), 6);
  record["satelliti"] = gpsParser.satellites.value();
  record["altitudine"] = gpsParser.altitude.meters();
  record["velocita"] = gpsParser.speed.mps();
  record["hdop"] = (float) gpsParser.hdop.value()/100;
  String dateTime = "";
  dateTime += gpsParser.date.year();
  dateTime += "-";
  dateTime += gpsParser.date.month();
  dateTime += "-";
  dateTime += gpsParser.date.day();
  dateTime += " ";
  dateTime += gpsParser.time.hour();
  dateTime += ":";
  dateTime += gpsParser.time.minute();
  dateTime += ":";
  dateTime += gpsParser.time.second(); //2019-02-28 10:10:00
  record["dateTime"] = dateTime;
  
  JsonArray datiSensori = record.createNestedArray("datiSensori");

  if(!isnan(tempAcqua) && tempAcqua != -127) {
    JsonObject datiSensori_0 = datiSensori.createNestedObject();
    datiSensori_0["valore"].set((float) tempAcqua);
    datiSensori_0["idSensore"] = ID_PROBE;
  }
  
  if(!isnan(tempAria)) {
    JsonObject datiSensori_1 = datiSensori.createNestedObject();
    datiSensori_1["valore"].set((float) tempAria);
    datiSensori_1["idSensore"] = ID_DHT11_TEMP;
  }
  
  if(!isnan(umidAria)) {
    JsonObject datiSensori_2 = datiSensori.createNestedObject();
    datiSensori_2["valore"].set((float) umidAria);
    datiSensori_2["idSensore"] = ID_DHT11_HUM;
  }

  String messaggio = "";
  serializeJson(record, messaggio);
  char payload[messaggio.length() + 1];
  messaggio.toCharArray(payload, messaggio.length() + 1);
  
  //send the actual message
  mqtt.publish(MQTT_DATA_TOPIC, payload);
}
