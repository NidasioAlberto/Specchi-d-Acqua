#include <DHT.h>

#define DHT_PIN D6

DHT dht(DHT_PIN, DHT11);

float tempAria, umidAria;

void setup() {
  Serial.begin(115200);
  
  setupSensori();
}

void loop() {
  leggiSensori();
  delay(1000);
}

void setupSensori() {
  dht.begin();
}

void leggiSensori() {
  tempAria = dht.readTemperature();
  umidAria = dht.readHumidity();

  Serial.print("Temperatura aria: "); Serial.print(tempAria);
  Serial.print(" umidità aria: "); Serial.println(umidAria);
}
