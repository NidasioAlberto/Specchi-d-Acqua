#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define RED_LED 14
#define GREEN_LED 12

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

#define MQTT_USR "barca1"
#define MQTT_PSW "barca1"
#define MQTT_ADDR "192.168.4.100"
#define MQTT_DATA_TOPIC "barca1/data"
#define MQTT_CONFIG_TOPIC "barca1/config"

WiFiClient espClient;
PubSubClient mqtt(espClient);

long lastTime;

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
}

void loop() {
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

  //send some data
  if(millis() - lastTime > 1000) {
    mqtt.publish(MQTT_DATA_TOPIC, "hello world");
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
    //blink the red led while co nnecting
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
