String voice;
int
ledweiss = 2, //Die weiße LED mit Pin2 verbinden
ledrot = 3,  //Die rote LED mit Pin3 verbinden
ledblau = 4, //usw….
ledgelb = 13,
ledgruen = 6;

void allon() { //Die Funktion für den Befehl alle LEDs anzuschalten
  digitalWrite(ledweiss, HIGH);
  digitalWrite(ledrot, HIGH);
  digitalWrite(ledblau, HIGH);
  digitalWrite(ledgelb, HIGH);
  digitalWrite(ledgruen, HIGH);
}
void alloff() { //Die Funktion für den Befehl alle LEDs auszuschalten
  digitalWrite(ledweiss, LOW);
  digitalWrite(ledrot, LOW);
  digitalWrite(ledblau, LOW);
  digitalWrite(ledgelb, LOW);
  digitalWrite(ledgruen, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(ledweiss, OUTPUT); //Die Pins mit den LEDs werden als Ausgänge festgelegt
  pinMode(ledrot, OUTPUT);
  pinMode(ledblau, OUTPUT);
  pinMode(ledgelb, OUTPUT);
  pinMode(ledgruen, OUTPUT);
}

void loop() {
  while (Serial.available()) { //überprüfen ob lesbare Werte vorhanden sind
    delay(10);
    char c = Serial.read();
    if (c == '#') {
      break; //“#“ zeigt das ende eines Befehls an, deshalb soll der Loop verlassen werden wenn ein ''#''vorkommt
    }
    voice += c;
  }
  if (voice.length() > 0) {
    Serial.println(voice);
    //in diesem Abschnitt werden die Befehle den einzelnen Funktionen zugeordnet

    if (voice == "*alle an") {
      allon(); //alle Pins sollen angeschaltet werden (vorher festgelegte Funktion allon)
    }
    else if (voice == "*alle aus") {
      alloff(); //alle Pins sollen ausgeschaltet werden (vorher festgelegte Funktion alloff)
    }

    //In diesem Abschnitt wird das Einschalten der einzelnen LEDs festgelegt

    else if (voice == "*weiß an") {
      digitalWrite(ledweiss, HIGH);
    }
    else if (voice == "*rot an") {
      digitalWrite(ledrot, HIGH);
    }
    else if (voice == "*blau an") {
      digitalWrite(ledblau, HIGH);
    }
    else if (voice == "*gelb an") {
      digitalWrite(ledgelb, HIGH);
    }
    else if (voice == "*grün an") {
      digitalWrite(ledgruen, HIGH);
    }

    //In diesem Abschnitt wird das Ausschalten der einzelnen LEDs festgelegt

    else if (voice == "*weiß aus") {
      digitalWrite(ledweiss, LOW);
    }
    else if (voice == "*rot aus") {
      digitalWrite(ledrot, LOW);
    }
    else if (voice == "*blau aus") {
      digitalWrite(ledblau, LOW);
    }
    else if (voice == "*gelb aus") {
      digitalWrite(ledgelb, LOW);
    }
    else if (voice == "*grün aus") {
      digitalWrite(ledgruen, LOW);
    }
    voice = "";
  }
}
