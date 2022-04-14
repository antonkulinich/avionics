#define PYRO 2

void setup() {
  // put your setup code here, to run once:
pinMode(PYRO, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(PYRO, HIGH);
delay(1000);
digitalWrite(PYRO,LOW);
delay(2000);

}
