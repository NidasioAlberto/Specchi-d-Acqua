#define FORWARD_PIN D2
#define BACKWARD_PIN D3
#define ENABLE_PIN D4

void setup() {
  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(FORWARD_PIN, HIGH);
  digitalWrite(BACKWARD_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH);
}

void loop() {
}
