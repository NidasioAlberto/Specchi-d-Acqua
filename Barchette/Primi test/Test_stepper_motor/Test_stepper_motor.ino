#define STEPPER_1 D1
#define STEPPER_2 D2
#define STEPPER_3 D3
#define STEPPER_4 D4

#define STEP_TOTALI 4096

int stepCount;
int posizioneCorrenteInStep; //si parte da 0 gradi

void setup() {
  Serial.begin(9600);
  
  pinMode(STEPPER_1, OUTPUT);
  pinMode(STEPPER_2, OUTPUT);
  pinMode(STEPPER_3, OUTPUT);
  pinMode(STEPPER_4, OUTPUT);
}

void loop() {
  if(Serial.available()) {
    String dati = Serial.readString();

    int gradi = dati.toInt();
    Serial.print("Gradi: "); Serial.println(gradi);

    muoviElica(gradi);
  }
}

void muoviElica(int gradi) {
  Serial.println(posizioneCorrenteInStep);
  //accettiamo da -90 a 90 gradi
  if(gradi > 90) gradi = 90;
  else if(gradi < -90) gradi = -90;
  
  //trasformo i gradi in step
  int stepDaGradi = gradi * STEP_TOTALI / 360;

  //controllo la posizione attuale e calcolo gli step da fare
  int stepDaFare = stepDaGradi - posizioneCorrenteInStep;
  Serial.print("Gradi da fare: "); Serial.println(gradi);
  Serial.print("Step da fare: "); Serial.println(stepDaFare);

  //performo gli step
  if(stepDaFare > 0) {
    for(int i = 0; i < stepDaFare; i++) {
      posizioneCorrenteInStep++;
      stepperSequence((posizioneCorrenteInStep + 1000) % 8);
      delay(2);
    }
  } else {
    for(int i = 0; i < -stepDaFare; i++) {
      posizioneCorrenteInStep--;
      stepperSequence((posizioneCorrenteInStep + 1000) % 8);
      delay(2);
    }
  }
  stopStepper();
  Serial.println(posizioneCorrenteInStep);
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
