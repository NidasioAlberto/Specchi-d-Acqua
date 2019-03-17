#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//if the you can't send a packet more than 128 bytes change MQTT_MAX_PACKET_SIZE in the .h library

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//sezione connessioni

#include <ArduinoJson.h>
const size_t capacity = JSON_ARRAY_SIZE(3) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(8) + 260;

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

#define ID_BARCHETTA 1
#define MQTT_USR "barca1"
#define MQTT_PSW "barca1"
#define MQTT_ADDR "192.168.4.100"
#define MQTT_DATA_TOPIC "records"
#define MQTT_CONFIG_TOPIC "barca1/config"

WiFiClient espClient;
PubSubClient mqtt(espClient);

WiFiServer server(1234);

boolean velocitaC, angoloC, direzioneC;
String velocita, angolo, direzione;
float speedF;
int angoloI;

//sezione sensori

#define ID_PROBE 1 //id sonda temperatura acqua
#define ID_DHT11_TEMP 2 //id DHT11 temepratura
#define ID_DHT11_HUM 3 //id DHT11 umidità

//#define PROBE_PIN D7
#define DHT_PIN 3

//OneWire oneWire(PROBE_PIN);

//DallasTemperature probe(&oneWire);
DHT dht(DHT_PIN, DHT11);

float tempAria, umidAria;//, tempAcqua;

TinyGPSPlus gpsParser;
SoftwareSerial gps(D8, D8);

long ultimaVolta;
float frequenzaDati = 1; //in Hz

//sezione motori
#define STEPPER_1 D0
#define STEPPER_2 D3
#define STEPPER_3 D4
#define STEPPER_4 D5

#define STEP_TOTALI 4096

#define DC_ENABLE D6
#define DC_FORWARD D7
#define DC_BACKWARD 1

int posizioneCorrenteInStep; //si parte da 0 gradi
int posizioneDaRaggiungere;
long ultimoAggiornamentoStepper;
boolean stepperFermo = true;

void setup() {
  //Serial.begin(115200);
  
  preparaMotori();
  
  fermaMotori();

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

  //controllo se qualcuno vuole radiocomandare la barchetta
  WiFiClient client = server.available();
 
  if(client) {
    while (client.connected()) {
      aggiornaStepper();
      if(client.available()) {
        velocita = "";
        angolo = "";
        direzione = "";

        velocitaC = true;
        angoloC = false;
        direzioneC = false;

        while(client.available() > 0) {
          char c = client.read();

          //Serial.write(c);
          
          if(c == ',') {
            if(velocitaC) {
              velocitaC = false;
              angoloC = true;
            } else if(angoloC) {
              angoloC = false;
              direzioneC = true;
            }
          } else if(c == ';') {
            //Serial.print("Velovità: "); //Serial.print(velocita.toInt());
            //Serial.print(" angolo: "); //Serial.print(angolo.toInt());
            //Serial.print(" direzione: "); //Serial.println(direzione.toInt());
            muoviElica(angolo.toInt());
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
    //Serial.println("Client disconnected");
  }
  
  aggiornaStepper();
  mqtt.loop();
}

void connettiWifi() {
  //provo a connettermi
  WiFi.mode(WIFI_STA); //imposto la modalità di funzionamento
  WiFi.begin(WIFI_SSID, WIFI_PSW); //imposto ssid e password

  //aspetto fino a quando siamo connessi
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  //Serial.println("Connessione WIFI stabilita");

  server.begin();
}

void connettiMqtt() {
  //imposto il callback per i messaggi
  mqtt.setCallback(gestisciMessaggioMqtt);

  //imposto l'indirizzo del server e la porta
  mqtt.setServer(MQTT_ADDR, 1883);

  //provo a connettermi con username e password
  while(!mqtt.connected()) {
    //Serial.print("Collegamento al server MQTT in corso...");
    if(mqtt.connect(MQTT_USR, MQTT_USR, MQTT_PSW)) {
      //Serial.println("Connessione tramite  MQTT stabilita");

      //mi iscrivo al topic di configurazione
      mqtt.subscribe(MQTT_CONFIG_TOPIC);
      
    } else {
      //Serial.print("Connessione fallita, rc=");
      //Serial.print(mqtt.state());
      
      delay(2000);
    }
  }
}

void gestisciMessaggioMqtt(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Nuovo messaggio MQTT (");
  //Serial.print(topic);
  //Serial.print("):");
  //for(int i = 0; i < length; i++) //Serial.write((char) payload[i]);
  //Serial.println("");
}

void setupSensori() {
  //probe.begin();
  dht.begin();
}

void leggiSensori() {
  //probe.requestTemperatures();
  //tempAcqua = probe.getTempCByIndex(0);
  tempAria = dht.readTemperature();
  umidAria = dht.readHumidity();

  ////Serial.print("Temperatura acqua: "); //Serial.print(tempAcqua);
  ////Serial.print("Temperatura aria: "); //Serial.print(tempAria);
  ////Serial.print(" umidità aria: "); //Serial.println(umidAria);
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
    //Serial.println("Connessione WIFI persa");
    
    //provo a riconnettermi
    connettiWifi();
    connettiMqtt();
  }
}

void controllaMqtt() {
  if(mqtt.state() != MQTT_CONNECTED) {
    //Serial.println("Connessione MQTT persa");

    //provo a ricollegarmi
    connettiMqtt();
  }
}

void inviaDati() {
  DynamicJsonDocument record(capacity);
  
  //JsonObject record = doc.createNestedObject("record");
  record["idBarchetta"] = ID_BARCHETTA;
  record["latitudine"] = String(gpsParser.location.lat(), 6); 
  record["longitudine"] = String(gpsParser.location.lng(), 6);
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

  /*if(!isnan(waterTemp)) {
    JsonObject datiSensori_0 = datiSensori.createNestedObject();
    datiSensori_0["valore"].set((float) waterTemp);
    datiSensori_0["idSensore"] = ID_PROBE;
  }*/
  
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
  //Serial.println(" data sent");
}

void preparaMotori() {
  //imposto i pin dei motori
  pinMode(STEPPER_1, OUTPUT);
  pinMode(STEPPER_2, OUTPUT);
  pinMode(STEPPER_3, OUTPUT);
  pinMode(STEPPER_4, OUTPUT);

  //imposto i pin del motore DC
  pinMode(DC_ENABLE, OUTPUT);
  pinMode(DC_FORWARD, OUTPUT);
  pinMode(DC_BACKWARD, OUTPUT);
}

void fermaMotori() {
  digitalWrite(STEPPER_1, LOW);
  digitalWrite(STEPPER_2, LOW);
  digitalWrite(STEPPER_3, LOW);
  digitalWrite(STEPPER_4, LOW);
  
  digitalWrite(DC_ENABLE, LOW);
  digitalWrite(DC_FORWARD, LOW);
  digitalWrite(DC_FORWARD, LOW);
}

void impostaMotoreDC(int vel, int dir) {
  if(dir > 0) {
    digitalWrite(DC_FORWARD, HIGH);
    digitalWrite(DC_BACKWARD, LOW);
  } else if(dir == 0 || vel == 0) {
    digitalWrite(DC_FORWARD, LOW);
    digitalWrite(DC_BACKWARD, LOW);
  } else {
    digitalWrite(DC_FORWARD, LOW);
    digitalWrite(DC_BACKWARD, HIGH);
  }

  if(vel > 0) {
    analogWrite(DC_ENABLE, map(vel, 0, 100, 512, 768));
  }
}

void muoviElica(int gradi) {
  //Serial.print("Posizione corrente: "); //Serial.println(posizioneCorrenteInStep);
  //accettiamo da -90 a 90 gradi
  if(gradi > 90) gradi = 90;
  else if(gradi < -90) gradi = -90;
  
  //calcolo la posizione da raggiungere
  posizioneDaRaggiungere = gradi * STEP_TOTALI / 360;
  
  //Serial.print("Posizione da raggiungere: "); //Serial.println(posizioneDaRaggiungere);
}

void aggiornaStepper() {
  if(micros() > ultimoAggiornamentoStepper + 2000) {
    ultimoAggiornamentoStepper = micros();
    if(posizioneDaRaggiungere != posizioneCorrenteInStep) {
      stepperFermo = false;
      //controllo la posizione attuale
      if(posizioneDaRaggiungere > posizioneCorrenteInStep) {
        posizioneCorrenteInStep++;
        sequenzaStepper((posizioneCorrenteInStep + 1000) % 8);
      } else {
        posizioneCorrenteInStep--;
        sequenzaStepper((posizioneCorrenteInStep + 1000) % 8);
      }
    } else {
      if(!stepperFermo) {
        //Serial.println("fermo tutto");
        stepperFermo = true;
        fermaMotori();
      }
    }
  }
}

void sequenzaStepper(int seqNum) {
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
