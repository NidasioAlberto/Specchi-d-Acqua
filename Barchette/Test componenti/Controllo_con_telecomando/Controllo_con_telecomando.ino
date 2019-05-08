#include <ESP8266WiFi.h>

#define WIFI_SSID "Team specchi"
#define WIFI_PSW "Fornaroli"

#define STEPPER_1 D0
#define STEPPER_2 D3
#define STEPPER_3 D4
#define STEPPER_4 D5

#define STEP_TOTALI 4096

#define DC_ENABLE D6

int posizioneCorrenteInStep; //si parte da 0 gradi
int posizioneDaRaggiungere;
long ultimoAggiornamentoStepper;
boolean stepperFermo = true;

WiFiServer server(1234);

boolean velocitaC, angoloC, direzioneC;
String velocita, angolo, direzione;
float speedF;
int angoloI;

void setup() {
  //begin serial communciation for debug
  Serial.begin(115200);

  //imposto i pin dei motori
  pinMode(STEPPER_1, OUTPUT);
  pinMode(STEPPER_2, OUTPUT);
  pinMode(STEPPER_3, OUTPUT);
  pinMode(STEPPER_4, OUTPUT);

  //imposto i pin del motore DC
  pinMode(DC_ENABLE, OUTPUT);

  stopStepper();

  connectToWifi();
}

void loop() {
  //check wifi connection
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost");
    
    //there's an error !
    connectToWifi();
  }

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
    Serial.println("Client disconnected");
  }
  
  aggiornaStepper();
}

void connectToWifi() {
  //try to connect
  WiFi.mode(WIFI_STA); //set station mode
  WiFi.begin(WIFI_SSID, WIFI_PSW);
  server.setNoDelay(true);

  //wait until the wifi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Wifi connection established");
  Serial.println(WiFi.localIP());

  server.begin();
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
