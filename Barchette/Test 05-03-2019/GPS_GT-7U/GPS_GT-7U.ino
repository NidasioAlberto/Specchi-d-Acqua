#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gpsParser;
SoftwareSerial gps(D8, D7);

void setup() {
  Serial.begin(9600);
  gps.begin(9600);
}

void loop() {
  if(gps.available() > 0) {
    while(gps.available() > 0) {
      gpsParser.encode(gps.read());
    }
    
    //show the data if updated
    
    if(gpsParser.satellites.isUpdated()) {
      Serial.print("staellites: "); Serial.println(gpsParser.satellites.value());
    }
    
    if(gpsParser.location.isUpdated()) {
      Serial.print("lat: "); Serial.print(String(gpsParser.location.lat(), 6));
      Serial.print(" lng: "); Serial.println(String(gpsParser.location.lng(), 6));
    }
    
    if(gpsParser.altitude.isUpdated()) {
      Serial.print("altitude: "); Serial.println(gpsParser.altitude.meters());
    }
    
    if(gpsParser.speed.isUpdated()) {
      Serial.print("speed: "); Serial.println(gpsParser.speed.mps());
    }
    
    if(gpsParser.hdop.isUpdated()) {
      Serial.print("hdop: "); Serial.println((float) gpsParser.hdop.value()/100);
    }
    
    if(gpsParser.course.isUpdated()) {
      Serial.print("course: "); Serial.println(gpsParser.course.value());
    }
  }
}
