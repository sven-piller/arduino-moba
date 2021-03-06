/*
  Dieser Sketch ist für die Straßenbeleuchtung vorgesehen.

  Geplante Funktionsweise:
  1. Einschalten über Tastendruck
  2. Alle Lampen gehen zufällig mit leichten Schwankungen an.
  3. Alle Lamepen leuchten permanent, eine hat Probleme und flackert ab und zu
  4. Auf Tastendruck werden sie wieder ausgeschaltet

  2021-03-05
  Sven Piller

  Mit Ideen von:
  - https://www.domsmoba.de/arduino/8-leucht-stofflampen/

*/

// Taster zum Ein-/Ausschalten
const byte  taster = 2;
// Ausgänge mit angeschlossenen LED-Straßenlampen
const byte  lampen[] = {6, 8, 9, 10, 13};
// Anzahl der Lampen
const byte anzahl_lampen =  sizeof(lampen); //5

// Aktueller Zustand der Lampen
bool sind_lampen_an = false;

// Gibt es im aktuellen Zustand eine defekte Lampe
bool defekteLampeVorhanden  = false;
// Pin der defekten Lampe
byte defekte_lampe;
// Index der defekten Lampe im lampen-Array
byte idx_defekte_lampe;
// Flackerzeiten der defekten Lampe
int flackerzeit_defekte_lampe;

int Pause1;
int EinAus;

// Bootstrapping
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(taster, INPUT_PULLUP);

  for (int i = 0; i < anzahl_lampen; i++) {
    pinMode(lampen[i], OUTPUT);
    digitalWrite(lampen[i], LOW);
  }
}

// Programmlogik
void loop() {

  if (digitalRead(taster) == LOW) {
    // Taste gedrückt
    Serial.print("Taste gedrueckt! ");

    // Status toggeln
    if (sind_lampen_an == true) {
      Serial.println("An => Aus");
      sind_lampen_an = false;
      AllesDunkel();
    } else {
      Serial.println("Aus => An");

      randomSeed(analogRead(0) + millis());
      int zufall = random(0, 100);
      Serial.print("Defekte Lampe wenn > 50: "); Serial.println(zufall);

      if (zufall > 50) {
        defekteLampeVorhanden = true;
        BestimmeDefekteLampe();
        _printDefekteLampe();
      } else {
        defekteLampeVorhanden = false;
      }

      sind_lampen_an = true;
      BetriebLampen();
    }

    // Taster entprellen
    delay(400);
  }

  if (sind_lampen_an == true) {
    BetriebLampen();
  } else {
    AllesDunkel();
  }

}

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

  if (defekteLampeVorhanden == false) {
    AllesLeuchtet();
  } else {

    for (int i = 0; i < anzahl_lampen; i++) {
      if (i == idx_defekte_lampe) {
        BetriebFlackerndeLampe();
      } else {
        digitalWrite(lampen[i], HIGH);
      }
    }

  }

}

/*
   Sorgt für das Flackern der defekten Lampe

   Wenn die Zählvariable größer als die aktuelle Pause ist,
   dann wird der Status der Lampe getoggelt und eine neue
   Zeitspanne gesetzt.
*/
void BetriebFlackerndeLampe() {

  // aktuellen Zustand toggeln
  if (digitalRead(defekte_lampe) == HIGH) {
    digitalWrite(defekte_lampe, LOW);
    flackerzeit_defekte_lampe = random(300, 1000);
  } else {
    digitalWrite(defekte_lampe, HIGH);
    flackerzeit_defekte_lampe = random(5, 300);
  }

  // Den Flackerzustand eine gewisse Zeit halten
  for (int i = 0; i < flackerzeit_defekte_lampe; i++) {
    delay(1);

    // Interupt, falls Taster in dieser Pause gedrückt wird
    if ((digitalRead(taster)) == LOW) {
      i = flackerzeit_defekte_lampe;
    }

  }

}

/*
   Startet den Betrieb der Lampen

   Die Lampen sollen mit einer simulierten Flackerschaltung
   angehen.
*/
void StarteLampen() {

}

// Defekte Lampe bestimmen
void BestimmeDefekteLampe() {
  randomSeed(analogRead(0));
  idx_defekte_lampe = random(0, anzahl_lampen);
  defekte_lampe = lampen[idx_defekte_lampe];
}

/*
   Debugausgaben für die Defekte Lampe
*/
void _printDefekteLampe() {
  Serial.print("Defekte Lampe: #");
  Serial.print(idx_defekte_lampe);
  Serial.print(" => Lampe ");
  Serial.print(lampen[idx_defekte_lampe]);
  Serial.print(": ");
  Serial.println(defekte_lampe);
}
