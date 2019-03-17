void setup() {
  pinMode(D9, OUTPUT);
  pinMode(D10, OUTPUT);
}

void loop() {
  digitalWrite(D9, HIGH);
  digitalWrite(D10, LOW);
  delay(1000);
  
  digitalWrite(D9, LOW);
  digitalWrite(D10, HIGH);
  delay(1000);
}
