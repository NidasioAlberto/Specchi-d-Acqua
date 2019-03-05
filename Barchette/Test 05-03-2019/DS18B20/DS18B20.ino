#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

#define ONE_WIRE_BUS D3
#define DHT_PIN D1

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DHT dht(DHT_PIN, DHT11);

void setup() {
 Serial.begin(9600);
 Serial.println("Dallas Temperature IC Control Library Demo");
 sensors.begin();
 dht.begin();
} 
void loop() {
 sensors.requestTemperatures();

 Serial.print("Water tmp: "); Serial.print(sensors.getTempCByIndex(0));
 Serial.print(" air tmp: "); Serial.print(dht.readHumidity());
 Serial.print(" air humidity: "); Serial.println(dht.readTemperature());
 
 delay(1000); 
}
