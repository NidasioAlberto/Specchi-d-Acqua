#define LED_PIN 13

int stato = 0; //0: setup in corso, 1: tutto ok (i dati vengono trasmessi), 2: batteria scarica, altro: errore
#define FREQUENZA_STATO0 500
#define FREQUENZA_STATO2 100
long ultimoAggiornamentoLed;
int ultimoStatoLed;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Serial.print("Stato: "); Serial.println(stato);
  while(millis() < 20000) {
    delay(10);
    aggiornaLed();
  }

  stato = 1;

  Serial.print("Stato: "); Serial.println(stato);
  while(millis() < 30000) {
    delay(10);
    aggiornaLed();
  }
  
  stato = 2;

  Serial.print("Stato: "); Serial.println(stato);
  while(millis() < 40000) {
    delay(10);
    aggiornaLed();
  }

  stato = 3;

  Serial.print("Stato: "); Serial.println(stato);
  while(millis() < 50000) {
    delay(10);
    aggiornaLed();
  }
}

void loop() {
  digitalWrite(LED_PIN, LOW);
  delay(1000);
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
