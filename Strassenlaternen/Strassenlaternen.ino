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
const byte  TASTER_PIN = 2;
// Ausgänge mit angeschlossenen LED-Straßenlampen
const byte  LAMPEN_PINS[] = {6, 8, 9, 10, 13};
// Anzahl der Lampen
const byte LAMPEN_ANZAHL =  sizeof(LAMPEN_PINS); //5

// Aktueller Zustand der Lampen
bool sind_die_lampen_an = false;

// Gibt es im aktuellen Zustand eine defekte Lampe
bool defekteLampeVorhanden  = false;
// Pin der defekten Lampe
byte defekteLampePin;
// Index der defekten Lampe im lampen-Array
byte defekteLampeIndex;
// Flackerzeiten der defekten Lampe
int defekteLampeFlackerzeit;

int Pause1;
int EinAus;

// Bootstrapping
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(TASTER_PIN, INPUT_PULLUP);

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    pinMode(LAMPEN_PINS[i], OUTPUT);
    digitalWrite(LAMPEN_PINS[i], LOW);
  }
}

// Programmlogik
void loop() {

  if (digitalRead(TASTER_PIN) == LOW) {
    // Taste gedrückt
    Serial.print("Taste gedrueckt! ");

    // Status toggeln
    if (sind_die_lampen_an == true) {
      Serial.println("An => Aus");
      sind_die_lampen_an = false;
      schalteAllesDunkel();
    } else {
      Serial.println("Aus => An");

      randomSeed(analogRead(0) + millis());
      int zufall = random(0, 100);
      Serial.print("Defekte Lampe wenn > 50: "); Serial.println(zufall);

      if (zufall > 50) {
        defekteLampeVorhanden = true;
        bestimmeDefekteLampe();
        _printDefekteLampe();
      } else {
        defekteLampeVorhanden = false;
      }

      sind_die_lampen_an = true;
      betreibeLampen();
    }

    // Taster entprellen
    delay(400);
  }

  if (sind_die_lampen_an == true) {
    betreibeLampen();
  } else {
    schalteAllesDunkel();
  }

}

// Schaltet alle Lampen auf LOW
void schalteAllesDunkel() {

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    digitalWrite(LAMPEN_PINS[i], LOW);
  }

}

// Schaltet alle Lampen ein
void schalteAllesAn() {

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    digitalWrite(LAMPEN_PINS[i], HIGH);
  }

}

// Schaltet alle Lampen ein, inklusive der flackernden Lampe
void betreibeLampen() {

  if (defekteLampeVorhanden == false) {
    schalteAllesAn();
  } else {

    for (int i = 0; i < LAMPEN_ANZAHL; i++) {
      if (i == defekteLampeIndex) {
        betreibeFlackerndeLampe();
      } else {
        digitalWrite(LAMPEN_PINS[i], HIGH);
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
void betreibeFlackerndeLampe() {

  // aktuellen Zustand toggeln
  if (digitalRead(defekteLampePin) == HIGH) {
    digitalWrite(defekteLampePin, LOW);
    defekteLampeFlackerzeit = random(300, 1000);
  } else {
    digitalWrite(defekteLampePin, HIGH);
    defekteLampeFlackerzeit = random(5, 300);
  }

  // Den Flackerzustand eine gewisse Zeit halten
  for (int i = 0; i < defekteLampeFlackerzeit; i++) {
    delay(1);

    // Interupt, falls Taster in dieser Pause gedrückt wird
    if ((digitalRead(TASTER_PIN)) == LOW) {
      i = defekteLampeFlackerzeit;
    }

  }

}

/*
  Startet den Betrieb der Lampen

  Die Lampen sollen mit einer simulierten Flackerschaltung
  angehen.
*/
void starteLampen() {

}

// Defekte Lampe bestimmen
void bestimmeDefekteLampe() {
  randomSeed(analogRead(0));
  defekteLampeIndex = random(0, LAMPEN_ANZAHL);
  defekteLampePin = LAMPEN_PINS[defekteLampeIndex];
}

/*
  Debugausgaben für die Defekte Lampe
*/
void _printDefekteLampe() {
  Serial.print("Defekte Lampe: #");
  Serial.print(defekteLampeIndex);
  Serial.print(" => Lampe ");
  Serial.print(LAMPEN_PINS[defekteLampeIndex]);
  Serial.print(": ");
  Serial.println(defekteLampePin);
}
