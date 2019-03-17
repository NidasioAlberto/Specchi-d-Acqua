#define STEPPER_1 D1
#define STEPPER_2 D2
#define STEPPER_3 D3
#define STEPPER_4 D4

#define STEP_TOTALI 4096

int posizioneCorrenteInStep; //si parte da 0 gradi
int posizioneDaRaggiungere;
long ultimoAggiornamentoStepper;
boolean stepperFermo = true;

void setup() {
  Serial.begin(115200);

  //imposto i pin dei motori
  pinMode(STEPPER_1, OUTPUT);
  pinMode(STEPPER_2, OUTPUT);
  pinMode(STEPPER_3, OUTPUT);
  pinMode(STEPPER_4, OUTPUT);

  stopStepper();
}

void loop() {
  if(Serial.available()) {
    String dati = Serial.readString();

    int gradi = dati.toInt();
    Serial.print("Gradi: "); Serial.println(gradi);

    muoviElica(gradi);
  }
  aggiornaStepper();
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
