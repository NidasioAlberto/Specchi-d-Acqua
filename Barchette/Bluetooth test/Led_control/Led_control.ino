#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>

#define DHT_PIN 4 //GPIO4
#define DS18B20_PIN 5 // GPIO5

#define FREQUENZA_DATI .5 // Frequenza in Hz
#define MAX_CLIENT_COUNT 2

#define SENSORS_SERVICE_UUID                      "00100001-358C-4B41-9A2B-A2ACE9730461"
#define AIR_TEMPERATURE_CHARACTERISTIC_UUID       "00100002-358C-4B41-9A2B-A2ACE9730461"
#define AIR_HUMIDITY_CHARACTERISTIC_UUID          "00100003-358C-4B41-9A2B-A2ACE9730461"
#define WATER_TEMPERATURE_CHARACTERISTIC_UUID     "00100004-358C-4B41-9A2B-A2ACE9730461"

#define GPS_LOCATION_SERVICE_UUID                 "00200001-358C-4B41-9A2B-A2ACE9730461"
#define GPS_LATITUDE_CHARACTERISTIC_UUID          "00200002-358C-4B41-9A2B-A2ACE9730461"
#define GPS_LONGITUDE_CHARACTERISTIC_UUID         "00200003-358C-4B41-9A2B-A2ACE9730461"
#define GPS_SATELLITES_CHARACTERISTIC_UUID        "00200004-358C-4B41-9A2B-A2ACE9730461"
#define GPS_ALTITUDE_CHARACTERISTIC_UUID          "00200005-358C-4B41-9A2B-A2ACE9730461"
#define GPS_SPEED_CHARACTERISTIC_UUID             "00200006-358C-4B41-9A2B-A2ACE9730461"
#define GPS_HDOP_CHARACTERISTIC_UUID              "00200007-358C-4B41-9A2B-A2ACE9730461"

#define CONTROL_SERVICE_UUID                      "00300001-358C-4B41-9A2B-A2ACE9730461"
#define SPEED_CHARACTERISTIC_UUID                 "00300002-358C-4B41-9A2B-A2ACE9730461"
#define DIRECTION_CHARACTERISTIC_UUID             "00300003-358C-4B41-9A2B-A2ACE9730461"

// Sezione sensori

DHT dht(DHT_PIN, DHT11);
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
TinyGPSPlus gpsParser;

float tempAria, tempAcqua;
int umidAria;
float gpsLatitude, gpsLongitude, gpsSpeed, gpsHdop;
int gpsSatellites, gpsAltitude;

// Sezione BLE

BLEServer         *bleServer;
BLEService        *sensorsService;
BLECharacteristic *airTemperatureCharacteristic;
BLECharacteristic *airHumidityCharacteristic;
BLECharacteristic *waterTemperatureCharacteristic;

BLEService        *gpsLocationService;
BLECharacteristic *gpsLatitudeCharacteristic;
BLECharacteristic *gpsLongitudeCharacteristic;
BLECharacteristic *gpsSatellitesCharacteristic;
BLECharacteristic *gpsAltitudeCharacteristic;
BLECharacteristic *gpsSpeedCharacteristic;
BLECharacteristic *gpsHdopCharacteristic;

BLEService        *controlService;
BLECharacteristic *speedCharacteristic;
class SpeedCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("New speed: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println();
      }
    }
};
BLECharacteristic *directionCharacteristic;
class DirectionCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("New direction: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println();
      }
    }
};

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Dispositivo connesso");
      Serial.print("Dispositivi connessi: "); Serial.println(bleServer->getConnectedCount());
      if(bleServer->getConnectedCount() < MAX_CLIENT_COUNT) bleServer->startAdvertising(); // Riavvio l'advertising per permetter anche ad altri dispositivi di connettersi, rimuovere per permettere un solo dispositivo alla volta
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Dispositivo disconnesso");
      Serial.print("Dispositivi connessi: "); Serial.println(bleServer->getConnectedCount());
      if(bleServer->getConnectedCount() < MAX_CLIENT_COUNT) bleServer->startAdvertising();
    }
};

void setup() {
  Serial.begin(115200);

  // Preparo i sensori
  dht.begin();
  ds18b20.begin();
  Serial2.begin(9600);
  
  // Avvio il bluetooth
  BLEDevice::init("Barchetta 1");

  // Creo il server BLE
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new ServerCallbacks());

  // Servizio e caratteristiche per i sensori
  sensorsService = bleServer->createService(BLEUUID(SENSORS_SERVICE_UUID), 11);
  
  airTemperatureCharacteristic = sensorsService->createCharacteristic(
    AIR_TEMPERATURE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  airTemperatureCharacteristic->addDescriptor(new BLE2902());
  
  airHumidityCharacteristic = sensorsService->createCharacteristic(
    AIR_HUMIDITY_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  airHumidityCharacteristic->addDescriptor(new BLE2902());

  waterTemperatureCharacteristic = sensorsService->createCharacteristic(
    WATER_TEMPERATURE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  waterTemperatureCharacteristic->addDescriptor(new BLE2902());

  // Servizio e caratteristiche per GPS
  gpsLocationService = bleServer->createService(BLEUUID(GPS_LOCATION_SERVICE_UUID), 20);
  
  gpsLatitudeCharacteristic = gpsLocationService->createCharacteristic(
    GPS_LATITUDE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  gpsLatitudeCharacteristic->addDescriptor(new BLE2902());
  
  gpsLongitudeCharacteristic = gpsLocationService->createCharacteristic(
    GPS_LONGITUDE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  gpsLongitudeCharacteristic->addDescriptor(new BLE2902());
  
  gpsSatellitesCharacteristic = gpsLocationService->createCharacteristic(
    GPS_SATELLITES_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  gpsSatellitesCharacteristic->addDescriptor(new BLE2902());
  
  gpsAltitudeCharacteristic = gpsLocationService->createCharacteristic(
    GPS_ALTITUDE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  gpsAltitudeCharacteristic->addDescriptor(new BLE2902());
  
  gpsSpeedCharacteristic = gpsLocationService->createCharacteristic(
    GPS_SPEED_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  gpsSpeedCharacteristic->addDescriptor(new BLE2902());
  
  gpsHdopCharacteristic = gpsLocationService->createCharacteristic(
    GPS_HDOP_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  gpsHdopCharacteristic->addDescriptor(new BLE2902());

  // Control service
  controlService = bleServer->createService(BLEUUID(CONTROL_SERVICE_UUID), 8);
  
  speedCharacteristic = controlService->createCharacteristic(
    SPEED_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE
  );
  speedCharacteristic->setCallbacks(new SpeedCallbacks());
  
  directionCharacteristic = controlService->createCharacteristic(
    DIRECTION_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE
  );
  directionCharacteristic->setCallbacks(new DirectionCallbacks());

  // Avvio i servizi
  sensorsService->start();
  gpsLocationService->start();
  controlService->start();

  // Avvio l'advertising
  bleServer->getAdvertising()->start();

  Serial.println("In attesa di un client...");
}

void loop() {
  //aggiorno i dati del GPS
  aggiornaGps();

  gpsLatitude = gpsParser.location.lat();
  gpsLongitude = gpsParser.location.lng();
  gpsSatellites = gpsParser.satellites.value();
  gpsAltitude = gpsParser.altitude.meters();
  gpsSpeed = gpsParser.speed.mps();
  gpsHdop = (float) gpsParser.hdop.value()/100;
  
  tempAria = dht.readTemperature();
  umidAria = dht.readHumidity();
  ds18b20.requestTemperatures();
  tempAcqua = ds18b20.getTempCByIndex(0);


  Serial.print("Lat: "); Serial.print(gpsLatitude);
  Serial.print(" lon: "); Serial.print(gpsLongitude);
  Serial.print(" satellites number: "); Serial.print(gpsSatellites);
  Serial.print(" altitude: "); Serial.print(gpsAltitude);
  Serial.print(" speed: "); Serial.print(gpsSpeed);
  Serial.print(" hdop: "); Serial.print(gpsHdop);
  Serial.print(" temperatura aria: "); Serial.print(tempAria);
  Serial.print(" umidità aria: "); Serial.print(umidAria);
  Serial.print(" temperatura acqua: "); Serial.print(tempAcqua);
  Serial.println("");

  //Preparo i dati e li pubblico sul servizio
  char stringaTempAria[4] = {'\0'};
  sprintf(stringaTempAria, "%2.1f", tempAria);
  airTemperatureCharacteristic->setValue(stringaTempAria);
  airTemperatureCharacteristic->notify();
  
  char stringaUmidAria[3] = {'\0'};
  sprintf(stringaUmidAria, "%2d", umidAria);
  airHumidityCharacteristic->setValue(stringaUmidAria);
  airHumidityCharacteristic->notify();
  
  char stringaTempAcqua[5] = {'\0'};
  sprintf(stringaTempAcqua, "%4.2f", tempAcqua);
  waterTemperatureCharacteristic->setValue(stringaTempAcqua);
  waterTemperatureCharacteristic->notify();
  
  char latitudeString[10] = {'\0'};
  sprintf(latitudeString, "%8.6f", gpsLatitude);
  gpsLatitudeCharacteristic->setValue(latitudeString);
  gpsLatitudeCharacteristic->notify();
  
  char longitudeString[10] = {'\0'};
  sprintf(longitudeString, "%8.6f", gpsLongitude);
  gpsLongitudeCharacteristic->setValue(longitudeString);
  gpsLongitudeCharacteristic->notify();
  
  char satellitesString[3] = {'\0'};
  sprintf(satellitesString, "%d", gpsSatellites);
  gpsSatellitesCharacteristic->setValue(satellitesString);
  gpsSatellitesCharacteristic->notify();
  
  char altitudeString[5] = {'\0'};
  sprintf(altitudeString, "%d", gpsAltitude);
  gpsAltitudeCharacteristic->setValue(altitudeString);
  gpsAltitudeCharacteristic->notify();
  
  char speedString[5] = {'\0'};
  sprintf(speedString, "%3.1f", gpsSpeed);
  gpsSpeedCharacteristic->setValue(speedString);
  gpsSpeedCharacteristic->notify();
  
  char hdopString[7] = {'\0'};
  sprintf(hdopString, "%4.2f", gpsHdop);
  gpsHdopCharacteristic->setValue(hdopString);
  gpsHdopCharacteristic->notify();


  delay(1000 / FREQUENZA_DATI);
}

void aggiornaGps() {
  while(Serial2.available() > 0) {
    gpsParser.encode(Serial2.read());
  }
}
