#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//if the you can't send a packet more than 128 bytes change MQTT_MAX_PACKET_SIZE in the .h library

#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <ArduinoJson.h>
const size_t capacity = JSON_ARRAY_SIZE(3) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(8) + 260;

#define RED_LED D5
#define GREEN_LED D6

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

#define ID_BARCHETTA 3
#define MQTT_USR "barca3"
#define MQTT_PSW "barca3"
#define MQTT_ADDR "192.168.4.100"
#define MQTT_DATA_TOPIC "records"
#define MQTT_CONFIG_TOPIC "barca3/config"

WiFiClient espClient;
PubSubClient mqtt(espClient);

long lastTime;

#define ID_PROBE 1
#define ID_DHT11_TEMP 2
#define ID_DHT11_HUM 3

#define ONE_WIRE_BUS D3
#define DHT_PIN D1

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT11);

float waterTemp, airTemp, airHumidity;

int sampleRate = 1;

TinyGPSPlus gpsParser;
SoftwareSerial gps(D8, D7);

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  //first set the red led on until the esp connect to the wifi
  digitalWrite(RED_LED, HIGH);

  //begin serial communciation for debug
  Serial.begin(9600);

  connectToWifi();

  connectMqtt();

  //turn on the green led
  digitalWrite(GREEN_LED, HIGH);

  //prepare the sensors
  setupSensors();
  
  gps.begin(9600);
}

void loop() {
  //update the gps when possible
  updateGps();
  
  //check wifi connection
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");

    //turn off the green led
    digitalWrite(GREEN_LED, LOW);
    
    //there's an error !
    connectToWifi();

    connectMqtt();

    //turn on the green led
    digitalWrite(GREEN_LED, HIGH);
  }

  //check mqtt connection
  if(mqtt.state() != MQTT_CONNECTED) {
    Serial.println("MQTT connection lost");

    //turn off the green led
    digitalWrite(GREEN_LED, LOW);

    connectMqtt();

    //turn on the green led
    digitalWrite(GREEN_LED, HIGH);
  }

  //every n sencods
  if(millis() - lastTime > sampleRate * 1000) {
    //record and send sensors data
    recordSensorsData();
    sendSensorsData();
    
    lastTime = millis();
  }

  mqtt.loop();
}

void connectToWifi() {
  //try to connect
  WiFi.mode(WIFI_STA); //set station mode
  WiFi.begin(WIFI_SSID, WIFI_PSW);

  //wait until the wifi is connected
  while (WiFi.status() != WL_CONNECTED) {
    //blink the red led while connecting
    digitalWrite(RED_LED, HIGH);
    delay(250);
    digitalWrite(RED_LED, LOW);
    delay(250);
  }
  Serial.println("Wifi connection established");
}

void connectMqtt() {
  //set the callback for the mqtt protocol
  mqtt.setCallback(handleMqttMessage);

  //set the server ip and port
  mqtt.setServer(MQTT_ADDR, 1883);

  //try to connect with username and password
  while(!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    if(mqtt.connect(MQTT_USR, MQTT_USR, MQTT_PSW)) {
      Serial.println("Connected through MQTT");

      //subscribe to the config topic
      mqtt.subscribe(MQTT_CONFIG_TOPIC);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");

      //blink the led
      digitalWrite(RED_LED, HIGH);
      delay(2500);
      digitalWrite(RED_LED, LOW);
      delay(2500);
    }
  }
}

void handleMqttMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("New mqtt message (");
  Serial.print(topic);
  Serial.print("):");
  for(int i = 0; i < length; i++) Serial.write((char) payload[i]);
  Serial.println("");
}

void setupSensors() {
  sensors.begin();
  dht.begin();
}

void recordSensorsData() {
  sensors.requestTemperatures();
  waterTemp = sensors.getTempCByIndex(0);
  airTemp = dht.readTemperature();
  airHumidity = dht.readHumidity();

  //Serial.print("Water tmp: "); Serial.print(waterTemp);
  //Serial.print(" air tmp: "); Serial.print(airTemp);
  //Serial.print(" air hum: "); Serial.println(airHumidity);
}

void sendSensorsData() {
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

  if(!isnan(waterTemp)) {
    JsonObject datiSensori_0 = datiSensori.createNestedObject();
    datiSensori_0["valore"].set((float) waterTemp);
    datiSensori_0["idSensore"] = ID_PROBE;
  }
  
  if(!isnan(airTemp)) {
    JsonObject datiSensori_1 = datiSensori.createNestedObject();
    datiSensori_1["valore"].set((float) airTemp);
    datiSensori_1["idSensore"] = ID_DHT11_TEMP;
  }
  
  if(!isnan(airHumidity)) {
    JsonObject datiSensori_2 = datiSensori.createNestedObject();
    datiSensori_2["valore"].set((float) airHumidity);
    datiSensori_2["idSensore"] = ID_DHT11_HUM;
  }

  String message = "";
  serializeJson(record, message);
  //Serial.println(message);
  Serial.println("data sent");
  char payload[message.length() + 1];
  message.toCharArray(payload, message.length() + 1);

  //Serial.println(payload);
  
  //send the actual message
  Serial.println(mqtt.publish(MQTT_DATA_TOPIC, payload));
}

void updateGps() {
  if(gps.available() > 0) {
    while(gps.available() > 0) {
      gpsParser.encode(gps.read());
    }
  }
}
