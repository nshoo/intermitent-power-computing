/*
Simple program to read in information from pins 7 and 8
prints to computer through serial
(transmitter is MSP430)
*/

#define TRIGGER_PIN 7
#define DATA_PIN 8
#define POWER_PIN 9

bool triggered = false, started = false;
int count, total = 0, bits = 0, expected = 0;

void setup() {
  pinMode(TRIGGER_PIN, INPUT);
  pinMode(DATA_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("Starting!");
}

void loop() {
  if(!started){
    if(!triggered && digitalRead(TRIGGER_PIN) == HIGH){
      triggered = true;
      //Serial.print((digitalRead(DATA_PIN) == HIGH ? 1 : 0));
      //Serial.print(' ');
      expected = !expected;
      if((digitalRead(DATA_PIN) == HIGH ? 1 : 0) == expected)
        bits++;
      else {
        bits = 0;
        expected = 0;
        //Serial.print('\n');
      }
      if(bits == 8){
        bits = 0;
        expected = 0;
        started = true;
        total = 0;
      }
    } else if(triggered && digitalRead(TRIGGER_PIN) == LOW)
      triggered = false;
  } else {
    if(!triggered && digitalRead(TRIGGER_PIN) == HIGH){
      triggered = true;
      total *= 2;
      total += (digitalRead(DATA_PIN) == HIGH ? 1 : 0);
      if(++count >= 8) {
        //Serial.print("Char: ");
        Serial.print((char) total);
        started = false;
        count = 0;
      }
    } else if(triggered && digitalRead(TRIGGER_PIN) == LOW)
      triggered = false;
  }
}
