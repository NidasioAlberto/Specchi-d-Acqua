#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>

#define DHT_PIN 4 //GPIO4

#define FREQUENZA_DATI 1 // Frequenza in Hz

#define TEMPERATURE_SERVICE_UUID "46100001-358C-4B41-9A2B-A2ACE9730461"
#define TEMPERATURE_CHARACTERISTIC_UUID "46100002-358C-4B41-9A2B-A2ACE9730461"

// Sezione sensori

DHT dht(DHT_PIN, DHT11);

float tempAria, umidAria;

// Sezione BLE

BLEServer *bleServer;
BLECharacteristic *temperatureCharacteristic;

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Dispositivo connesso");

      bleServer->startAdvertising(); // Riavvio l'advertising per permetter anche ad altri dispositivi di connettersi, rimuovere per permettere un solo dispositivo alla volta
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Dispositivo disconnesso");
    }
};

class TemperatureCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
      // Il client cerca di leggere il valore della temperatura
      Serial.println("Il client ha letto la temperatura");
    }
};

void setup() {
  Serial.begin(115200);

  // Preparo il DHT11
  dht.begin();

  // Avvio il bluetooth
  BLEDevice::init("Barchetta 1");

  // Creo il server BLE
  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new ServerCallbacks());

  // Creo il servizio temperatura
  BLEService *temperatureService = bleServer->createService(TEMPERATURE_SERVICE_UUID);

  // Creo una caratteristica BLE
  temperatureCharacteristic = temperatureService->createCharacteristic(
    TEMPERATURE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  temperatureCharacteristic->setCallbacks(new TemperatureCharacteristicCallbacks()); // Imposto i callback per controllare quando il client richiede un valore

  // Avvio il servizio
  temperatureService->start();

  // Avvio l'advertising
  bleServer->getAdvertising()->start();

  Serial.println("In attesa di un client...");
}

void loop() {
  tempAria = dht.readTemperature();
  umidAria = dht.readHumidity();

  Serial.print("Temperatura aria 1: "); Serial.print(tempAria);
  Serial.print(" umidità aria: "); Serial.print(umidAria);
  Serial.println("");

  //Preparo il dato e lo pubblico sul servizio
  char stringaTempAria[5] = {'\0'};
  sprintf(stringaTempAria, "%3.1f", tempAria);
  temperatureCharacteristic->setValue(stringaTempAria);
  temperatureCharacteristic->notify();

  delay(1000 / FREQUENZA_DATI);
}
