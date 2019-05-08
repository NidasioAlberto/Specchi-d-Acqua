/*
La versione corrente del programma prevede l'invio dei dati (sensori, gps e tensione batteria)
a una certa frequenza stabilita.
Inoltre accetta una connessione tcp sulla porta 1234 per il controllo remoto tramite app.
*/

//SEZIONE LIBRERIE

//Wifi
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

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
const size_t capacity = JSON_ARRAY_SIZE(3) + 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(9) + 180;

//SEZIONE VARIABILI

//Wifi e connessioni
#define WIFI_SSID "Team_specchi"
#define WIFI_PSW "Fornaroli"
#define TCP_SERVER_PORT 1234

WiFiServer server(TCP_SERVER_PORT);
WiFiClient client, espClient;
PubSubClient mqtt(espClient);
boolean velocitaC, angoloC, direzioneC; //dati del telecomando
String velocita, angolo, direzione;

//Impostazioni MQTT
#define MQTT_USR "barca4"
#define MQTT_PSW "barca4"
#define MQTT_ADDR "192.168.0.1"
#define MQTT_PORT 1883
#define MQTT_DATA_TOPIC "records"

//Pin componenti
#define STEPPER_1 D0
#define STEPPER_2 D3
#define STEPPER_3 D4
#define STEPPER_4 D5
#define DC_ENABLE D6
#define LED_PIN 13      //GPIO8   - SD1
#define PROBE_PIN 1     //GPIO1   - TX
#define DHT_PIN 3       //GPIO3   - RX
#define GPS_PIN 15      //GPIO15  - D8
#define ADC_PIN A0       //ADC0    - A0

//Motori
#define STEP_TOTALI 4096
int posizioneCorrenteInStep; //si parte da 0 gradi
int posizioneDaRaggiungere;
long ultimoAggiornamentoStepper;
boolean stepperFermo = true;

//Led di stato
int stato = 0; //0: setup in corso, 1: tutto ok (i dati vengono trasmessi), 2: batteria scarica, altro: errore
#define FREQUENZA_STATO0 500
#define FREQUENZA_STATO2 100
long ultimoAggiornamentoLed;
int ultimoStatoLed;

//Impostazioni invio dati
#define ID_BARCHETTA "4"
#define ID_PROBE "1"
#define ID_DHT11_TEMP "2"
#define ID_DHT11_HUM "3"

//Istanze componenti e dati
OneWire oneWire(PROBE_PIN);
DallasTemperature probe(&oneWire);
DHT dht(DHT_PIN, DHT11);
TinyGPSPlus gpsParser;
SoftwareSerial gps(GPS_PIN, GPS_PIN);
float tempAria, umidAria, tempAcqua, tensione;

//gestione frequenza invio dati
long ultimaVolta;
float frequenzaDati = 0.5; //in Hz

void setup() {
  stato = 0;
  
  pinMode(LED_PIN, OUTPUT);

  //imposto il pin del motore DC
  pinMode(DC_ENABLE, OUTPUT);
  
  setupStepper();
  stopStepper();
  
  //mi connetto al wifi
  connettiWifi();

  connettiMqtt();

  setupSensori();
  
  gps.begin(9600);
}

void loop() {
  //aggiorno i dati del GPS
  aggiornaGps();

  //controllo la connessione
  controllaConnessione();

  aggiornaLed();

  //ogni n secondi leggo i dati e li invio
  if(millis() - ultimaVolta > (1 / frequenzaDati) * 1000) {
    //record and send sensors data
    leggiSensori();
    
    digitalWrite(LED_PIN, LOW);
    inviaDati();
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    
    ultimaVolta = millis();
  }
  
  //controlllo se c'è qualcuno che vuole controllare la barchetta
  if(client) {
    while(client.connected()) {   
      //controllo lo stato del wifi
      controllaConnessione();
      aggiornaLed();
      
      if(client.available()) {
        velocita = "";
        angolo = "";
        direzione = "";

        velocitaC = true;
        angoloC = false;
        direzioneC = false;

        while(client.available() > 0) {
          char c = client.read();
          
          if(c == ',') {
            if(velocitaC) {
              velocitaC = false;
              angoloC = true;
            } else if(angoloC) {
              angoloC = false;
              direzioneC = true;
            }
          } else if(c == ';') {
            muoviElica(angolo.toInt() / 2);
            impostaMotoreDC(velocita.toInt());
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
          
          aggiornaStepper();
          aggiornaLed();
        }
      }
          
      aggiornaStepper();
    }
    client.stop();
    muoviElica(0);
    impostaMotoreDC(0);
  } else {
    client = server.available();
  }

  aggiornaStepper();

  delay(1);
  mqtt.loop();
}

void controllaConnessione() {
  //controlla la connessione wifi
  if(WiFi.status() != WL_CONNECTED) {
    stato = 3;
    //c'è un errore
    connettiWifi();
  } else {
    stato = 1;
  }

  //controllo la connessione mqtt
  controllaMqtt();
}

void connettiWifi() {
  //provo a connettermi
  WiFi.mode(WIFI_STA);
  WiFi.hostname(MQTT_USR);
  WiFi.begin(WIFI_SSID, WIFI_PSW);

  //aspetto finchè non siamo connessi al wifi
  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
    aggiornaLed();
  }

  //avvio il server tcp
  server.begin();
}

void setupStepper() {
  //imposto i pin dei motori
  pinMode(STEPPER_1, OUTPUT);
  pinMode(STEPPER_2, OUTPUT);
  pinMode(STEPPER_3, OUTPUT);
  pinMode(STEPPER_4, OUTPUT);
}

void muoviElica(int gradi) {
  //accettiamo da -90 a 90 gradi
  if(gradi > 90) gradi = 90;
  else if(gradi < -90) gradi = -90;
  
  //calcolo la posizione da raggiungere
  posizioneDaRaggiungere = gradi * STEP_TOTALI / 360;
}

void aggiornaStepper() {
  if(micros() - ultimoAggiornamentoStepper > 2000) {
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

void aggiornaLed() {
  switch(stato) {
    case 0:
      if(millis() - ultimoAggiornamentoLed > FREQUENZA_STATO0) {
        if(ultimoStatoLed) {
          ultimoStatoLed = 0;
          digitalWrite(LED_PIN, ultimoStatoLed);
        } else {
          ultimoStatoLed = 1;
          digitalWrite(LED_PIN, ultimoStatoLed);
        }
        ultimoAggiornamentoLed = millis();
      }
      break;
    case 1:
      ultimoStatoLed = 1;
      digitalWrite(LED_PIN, ultimoStatoLed);
      break;
    case 2:
      if(millis() - ultimoAggiornamentoLed > FREQUENZA_STATO2) {
        if(ultimoStatoLed) {
          ultimoStatoLed = 0;
          digitalWrite(LED_PIN, ultimoStatoLed);
        } else {
          ultimoStatoLed = 1;
          digitalWrite(LED_PIN, ultimoStatoLed);
        }
        ultimoAggiornamentoLed = millis();
      }
      break;
    default:
      if(millis() - ultimoAggiornamentoLed > FREQUENZA_STATO2) {
        switch(ultimoStatoLed) {
          case 1:
            digitalWrite(LED_PIN, HIGH);
            break;
          case 2:
            digitalWrite(LED_PIN, LOW);
            break;
          case 3:
            digitalWrite(LED_PIN, HIGH);
            break;
          case 4:
            digitalWrite(LED_PIN, LOW);
            break;
          case 5:
            digitalWrite(LED_PIN, LOW);
            break;
        }
        ultimoStatoLed++;
        if(ultimoStatoLed > 5) ultimoStatoLed = 0;
        ultimoAggiornamentoLed = millis();
      }
      break;
  }
}

void impostaMotoreDC(int vel) {
  if(vel > 10) {
    analogWrite(DC_ENABLE, map(vel, 0, 100, 256, 1023));
  } else {
    digitalWrite(DC_ENABLE, LOW);
  }
}

void connettiMqtt() {
  //imposto l'indirizzo del server e la porta
  mqtt.setServer(MQTT_ADDR, MQTT_PORT);

  //provo a connettermi con username e password, se non funziona aspetto 2 secondi e riprovo
  while(!mqtt.connected()) {
    aggiornaLed();
    if(!mqtt.connect(MQTT_USR, MQTT_USR, MQTT_PSW)) delay(2000);
  }
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
  tensione = (float) 0.0058783*analogRead(ADC_PIN);
}

void aggiornaGps() {
  if(gps.available() > 0) {
    while(gps.available() > 0) {
      gpsParser.encode(gps.read());
    }
  }
}

void inviaDati() {
  DynamicJsonDocument record(capacity);
  
  //JsonObject record = doc.createNestedObject("record");
  record["idBarchetta"] = ID_BARCHETTA;
  record["tensione"].set((float) tensione);
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

void controllaMqtt() {
  if(mqtt.state() != MQTT_CONNECTED) {
    stato = 3;
    //provo a ricollegarmi
    connettiMqtt();
  } else {
    stato = 1;
  }
}
