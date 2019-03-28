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

#define LED_PIN 13

#define STEPPER_1 D0
#define STEPPER_2 D3
#define STEPPER_3 D4
#define STEPPER_4 D5

#define DC_ENABLE D6

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

#define STEP_TOTALI 4096
int posizioneCorrenteInStep; //si parte da 0 gradi
int posizioneDaRaggiungere;
long ultimoAggiornamentoStepper;
boolean stepperFermo = true;

WiFiServer server(1234);

boolean velocitaC, angoloC, direzioneC;
String velocita, angolo, direzione;
float speedF;
int angoloI;

//Dati e variabili
float tempAria, umidAria, tempAcqua;

long ultimaVolta;
float frequenzaDati = 0.5; //in Hz

int statoScheda = 0; //0 per impostazione iniziale, 1 per invio dati, 2 per perdita di segnale, 3 batteria scarica

void setup() {

  //imposto i pin dei motori
  pinMode(STEPPER_1, OUTPUT);
  pinMode(STEPPER_2, OUTPUT);
  pinMode(STEPPER_3, OUTPUT);
  pinMode(STEPPER_4, OUTPUT);

  //led
  pinMode(LED_PIN, OUTPUT);

  //imposto i pin del motore DC
  pinMode(DC_ENABLE, OUTPUT);

  stopStepper();
  
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

  WiFiClient client = server.available();
 
  if(client) {
    while(client.connected()) {
      aggiornaStepper();
      
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
            muoviElica(angolo.toInt() / 2);
            impostaMotoreDC(velocita.toInt(), direzione.toInt());
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
  }
  
  aggiornaStepper();
  
  mqtt.loop();
}

void connettiWifi() {
  //provo a connettermi
  WiFi.mode(WIFI_STA); //imposto la modalità di funzionamento
  WiFi.begin(WIFI_SSID, WIFI_PSW); //imposto ssid e password

  //aspetto fino a quando siamo connessi
  while(WiFi.status() != WL_CONNECTED) {
    
  }

  server.begin();
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

void muoviElica(int gradi) {
  Serial.print("Posizione corrente: "); Serial.println(posizioneCorrenteInStep);
  //accettiamo da -90 a 90 gradi
  if(gradi > 90) gradi = 90;
  else if(gradi < -90) gradi = -90;
  
  //calcolo la posizione da raggiungere
  posizioneDaRaggiungere = gradi * STEP_TOTALI / 360;
  
  Serial.print("Posizione da raggiungere: "); Serial.println(posizioneDaRaggiungere);
}

void aggiornaStepper() {
  if(micros() > ultimoAggiornamentoStepper + 2000) {
    ultimoAggiornamentoStepper = micros();
    if(posizioneDaRaggiungere != posizioneCorrenteInStep) {
      stepperFermo = false;
      //controllo la posizione attuale
      if(posizioneDaRaggiungere > posizioneCorrenteInStep) {
        posizioneCorrenteInStep++;
        stepperSequence((posizioneCorrenteInStep + 1000) % 8);
      } else {
        posizioneCorrenteInStep--;
        stepperSequence((posizioneCorrenteInStep + 1000) % 8);
      }
    } else {
      if(!stepperFermo) {
        Serial.println("fermo tutto");
        stepperFermo = true;
        stopStepper();
      }
    }
  }
}

void stepperSequence(int seqNum) {
  int pin1, pin2, pin3, pin4;
  switch(seqNum){
    case 0: {
      pin1 = 1;
      pin2 = 0;
      pin3 = 0;
      pin4 = 0;
      break;
    }
    case 1: {
      pin1 = 1;
      pin2 = 1;
      pin3 = 0;
      pin4 = 0;
      break;
    }
    case 2: {
      pin1 = 0;
      pin2 = 1;
      pin3 = 0;
      pin4 = 0;
      break;
    }
    case 3: {
      pin1 = 0;
      pin2 = 1;
      pin3 = 1;
      pin4 = 0;
      break;
    } 
    case 4: {
      pin1 = 0;
      pin2 = 0;
      pin3 = 1;
      pin4 = 0;
      break;
    }
    case 5: {
      pin1 = 0;
      pin2 = 0;
      pin3 = 1;
      pin4 = 1;
      break;
    }
    case 6: {
      pin1 = 0;
      pin2 = 0;
      pin3 = 0;
      pin4 = 1;
      break;
    }
    case 7: {
      pin1 = 1;
      pin2 = 0;
      pin3 = 0;
      pin4 = 1;
      break;
    }
    default: {
      pin1 = 0;
      pin2 = 0;
      pin3 = 0;
      pin4 = 0;
      break;
    }
  }
  digitalWrite(STEPPER_1, pin1);
  digitalWrite(STEPPER_2, pin2);
  digitalWrite(STEPPER_3, pin3);
  digitalWrite(STEPPER_4, pin4);
}

void stopStepper() {
  digitalWrite(STEPPER_1, LOW);
  digitalWrite(STEPPER_2, LOW);
  digitalWrite(STEPPER_3, LOW);
  digitalWrite(STEPPER_4, LOW);
}

void impostaMotoreDC(int vel, int dir) {
  if(vel > 0) {
    analogWrite(DC_ENABLE, map(vel, 0, 100, 512, 1023));
  } else {
    digitalWrite(DC_ENABLE, LOW);
  }
}
