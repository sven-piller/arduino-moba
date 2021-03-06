/**
  Dieser Sketch ist für die Straßenbeleuchtung vorgesehen.

  Geplante Funktionsweise:
  1. Einschalten über Tastendruck
  2. Alle Lampen gehen zufällig mit leichten Schwankungen an.
  3. Alle Lamepen leuchten permanent, eine hat Probleme und flackert ab und zu
  4. Auf Tastendruck werden sie wieder ausgeschaltet

  2021-03-05
  Sven Piller

*/

const byte  taster = 2;
const byte  lampen[] = {6, 8, 9, 10, 13};
const byte anzahl_lampen =  sizeof(lampen); //5

int alle_an = false;

byte  defekte_lampe;
byte idx_defekte_lampe;
int pause_defekte_lampe;

int Pause1;
int EinAus;

// Schaltet alle Lampen auf LOW
void AllesDunkel() {

  for (int i = 0; i < anzahl_lampen; i++) {
    digitalWrite(lampen[i], LOW);
  }

}

// Schaltet alle Lampen ein
void AllesLeuchtet() {

  for (int i = 0; i < anzahl_lampen; i++) {
    digitalWrite(lampen[i], HIGH);
  }

}

// Schaltet alle Lampen ein, inklusive der flackernden Lampe
void BetriebLampen() {

  for (int i = 0; i < anzahl_lampen; i++) {
    if (i == idx_defekte_lampe) {
      BetriebFlackerndeLampe();     
    } else {
      digitalWrite(lampen[i], HIGH);
    }
  }
}

/**
 * Sorgt für das Flackern der defekten Lampe
 * 
 * Wenn die Zählvariable größer als die aktuelle Pause ist,
 * dann wird der Status der Lampe getoggelt und eine neue 
 * Zeitspanne gesetzt.
 */
void BetriebFlackerndeLampe() {

    if (digitalRead(defekte_lampe) == HIGH) {
      digitalWrite(defekte_lampe, LOW);
      pause_defekte_lampe = random(300, 1000);
    } else {
      digitalWrite(defekte_lampe, HIGH);
      pause_defekte_lampe = random(5, 300);
    }

    // Den Flackerzustand eine gewisse Zeit halten
    for (int i = 0; i < pause_defekte_lampe; i++) {
      delay(1);

      // Interupt, falls Taster in dieser Pause gedrückt wird
      if ((digitalRead(taster)) == LOW) {
        i = pause_defekte_lampe;
      }

    }

}

/**
 * Startet den Betrieb der Lampen
 * 
 * Die Lampen sollen mit einer simulierten Flackerschaltung
 * angehen.
 */
void StarteLampen() {

}

// Defekte Lampe bestimmen
void BestimmeDefekteLampe() {
  randomSeed(analogRead(0));
  idx_defekte_lampe = random(0, anzahl_lampen - 1);
  defekte_lampe = lampen[idx_defekte_lampe];
  _printDefekteLampe();
}

// Bootstrapping
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(taster, INPUT_PULLUP);

  for (int i = 0; i < anzahl_lampen; i++) {
    pinMode(lampen[i], OUTPUT);
    digitalWrite(lampen[i], LOW);
  }
  BestimmeDefekteLampe();
}

// Programmlogik
void loop() {

  if (digitalRead(taster) == LOW) {
    // Taste gedrückt
    Serial.print("Taste gedrueckt! ");

    // Status toggeln
    if (alle_an == false)
    {
      Serial.println("Aus => An");
      BestimmeDefekteLampe();
      alle_an = true;
      BetriebLampen();
    } else {
      Serial.println("An => Aus");
      alle_an = false;
      AllesDunkel();
    }

    // Taster entprellen
    delay(400);
  }

  if (alle_an == true) {
    // Lampen leuchten
    // AllesLeuchtet();
    BetriebLampen();
  } else {
    // Lampen sind aus
    AllesDunkel();
  }

}

/** 
 * Debugausgaben für die Defekte Lampe 
 */
void _printDefekteLampe(){
  Serial.print("Defekte Lampe: #");
  Serial.print(idx_defekte_lampe); 
  Serial.print(" => Lampe ");
  Serial.print(lampen[idx_defekte_lampe]);
  Serial.print(": "); 
  Serial.println(defekte_lampe);
}
