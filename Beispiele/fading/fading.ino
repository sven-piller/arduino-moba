int LED = 9; //Das Wort „LED“ steht jetzt für den Wert 9.

int helligkeit = 0; //Das Wort „helligkeit“ steht nun für den Wert, der bei der PWM ausgegeben wird. Die Zahl 0 ist dabei nur ein beliebiger Startwert.

int fadeschritte = 5; //fadeschritte: bestimmt die Geschwindigkeit des „fadens“.


void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(LED, OUTPUT);//Der Pin mit der LED (Pin9) ist ein Ausgang
}

void loop() {
  analogWrite(LED, helligkeit);
  helligkeit = helligkeit + fadeschritte;
  delay(25);
  if (helligkeit == 0 || helligkeit == 255)
  {
    fadeschritte = -fadeschritte ;
  }
}
