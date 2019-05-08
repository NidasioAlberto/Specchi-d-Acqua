#define ADC_PIN 2

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println((float) 0.0058783*analogRead(A0));
  Serial.println(analogRead(A0));
  delay(1000);
}
