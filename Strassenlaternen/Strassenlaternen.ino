/*

  Dieser Sketch ist für die Straßenbeleuchtung vorgesehen.

  Geplante Funktionsweise:
  1. Einschalten über Tastendruck
  2. Alle Lampen gehen zufällig mit leichten Schwankungen an.
  3. Alle Lamepen leuchten permanent, eine hat Probleme und flackert ab und zu
  4. Auf Tastendruck werden sie wieder ausgeschaltet
  5. Wenn das Programm aktiv ist, wird das per LED angezeigt

  2021-03-05
  Sven Piller

  Mit Ideen von:
  - https://www.domsmoba.de/arduino/8-leucht-stofflampen/

  Historie:
  - v1: 2021-03-28 Erste Version für die Anlage

*/

// Ausgänge mit angeschlossenen LED-Straßenlampen
const byte LAMPEN_PINS[10] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
// Anzahl der Lampen
const byte LAMPEN_ANZAHL = 10;
const byte LAMPE_AN = HIGH;
const byte LAMPE_AUS = LOW;

// Taster zum Ein-/Ausschalten
const byte TASTER_PIN = 2;
// Kosmetische Konstante
const byte TASTER_GEDRUECKT = LOW;
// LED Anzeige, ob Programm aktiv ist
const byte INDICATOR_LED_PIN = 13;

// Aktueller Zustand der Lampen
bool sind_die_lampen_an = false;

// Gibt es im aktuellen Zustand eine defekte Lampe
bool defekteLampeVorhanden = false;
// Pin der defekten Lampe
byte defekteLampePin;
// Index der defekten Lampe im lampen-Array
byte defekteLampeIndex;
// Flackerzeiten der defekten Lampe
int defekteLampeFlackerzeit;

// Bootstrapping
void setup() {
  // Serial.begin(9600);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW);

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    pinMode(LAMPEN_PINS[i], OUTPUT);
    digitalWrite(LAMPEN_PINS[i], LAMPE_AUS);
  }

}

// Programmlogik
void loop() {

  if (digitalRead(TASTER_PIN) == TASTER_GEDRUECKT) {
    // Serial.print("Taste gedrueckt! ");

    if (sind_die_lampen_an == true) {
      // Serial.println("An => Aus");
      loescheLampen();
      digitalWrite(INDICATOR_LED_PIN, LOW);
    } else {
      // Serial.println("Aus => An");
      digitalWrite(INDICATOR_LED_PIN, HIGH);

      randomSeed(analogRead(0) + millis());
      int defektWahrscheinlichkeit = random(0, 100);
      // Serial.print("Defekte Lampe wenn > 50: ");
      // Serial.println(defektWahrscheinlichkeit);

      if (defektWahrscheinlichkeit > 50) {
        defekteLampeVorhanden = true;
        bestimmeDefekteLampe();
        _printDefekteLampe();
      } else {
        defekteLampeVorhanden = false;
      }

      starteLampen();
    }

    // Taster entprellen
    delay(50);
  }

  if (sind_die_lampen_an == true) {
    digitalWrite(INDICATOR_LED_PIN, HIGH);
    betreibeLampen();
  } else {
    schalteAllesDunkel();
    digitalWrite(INDICATOR_LED_PIN, LOW);
  }

}

// Schaltet alle Lampen auf LOW
void schalteAllesDunkel() {

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    digitalWrite(LAMPEN_PINS[i], LAMPE_AUS);
  }

}

// Schaltet alle Lampen ein
void schalteAllesAn() {

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    digitalWrite(LAMPEN_PINS[i], LAMPE_AN);
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
        digitalWrite(LAMPEN_PINS[i], LAMPE_AN);
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
  if (digitalRead(defekteLampePin) == LAMPE_AN) {
    digitalWrite(defekteLampePin, LAMPE_AUS);
    defekteLampeFlackerzeit = random(300, 1000);
  } else {
    digitalWrite(defekteLampePin, LAMPE_AN);
    defekteLampeFlackerzeit = random(5, 300);
  }

  // Den Flackerzustand eine gewisse Zeit halten
  for (int i = 0; i < defekteLampeFlackerzeit; i++) {
    delay(1);

    // Interupt, falls Taster in dieser Pause gedrückt wird
    if ((digitalRead(TASTER_PIN)) == TASTER_GEDRUECKT) {
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
  // Serial.println("Anschalten beginnt");

  for (int i = 0; i < 100; i++) {
    randomSeed(analogRead(0) + millis());
    int einschaltIndex = random(0, LAMPEN_ANZAHL);
    int einschaltDelay = random(10, 50);
    delay(einschaltDelay);
    byte einschaltWahrscheinlichkeit = random(0, 8);

    if (einschaltWahrscheinlichkeit > 0) {
      digitalWrite(LAMPEN_PINS[einschaltIndex], LAMPE_AN);
      // Serial.print("Lampe eingeschaltet: ");
      // Serial.println(LAMPEN_PINS[einschaltIndex]);
    }
    else {
      digitalWrite(LAMPEN_PINS[einschaltIndex], LAMPE_AUS);
    }


  }

  sind_die_lampen_an = true;
  // Serial.println("Anschalten fertig");
  betreibeLampen();
}

/*
  Lösche die Lampen aus

  Die Lampen sollen etwas versetzt in zufälliger
  Reihenfolge ausgehen.
*/
void loescheLampen() {
  // Serial.println("Abschalten beginnt");

  bool neueZahl;
  int ausschaltReihenfolge[LAMPEN_ANZAHL];

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {

    do {
      ausschaltReihenfolge[i] = int(random(0, LAMPEN_ANZAHL));
      neueZahl = true;

      for (int j = 0; j < i; j++) {

        if (ausschaltReihenfolge[j] == ausschaltReihenfolge[i]) {
          neueZahl = false;
        }

      }

    } while (!neueZahl);

  }


  for (int ausschaltIndex = 0; ausschaltIndex < LAMPEN_ANZAHL; ausschaltIndex++) {
    randomSeed(analogRead(0) + millis());
    int ausschaltDelay = random(100, 200);
    delay(ausschaltDelay);
    // Serial.println(ausschaltDelay);
    digitalWrite(LAMPEN_PINS[ausschaltReihenfolge[ausschaltIndex]], LAMPE_AUS);
    // Serial.print(ausschaltReihenfolge[ausschaltIndex]);
    // Serial.print(":");
    // Serial.print(LAMPEN_PINS[ausschaltReihenfolge[ausschaltIndex]]);
    // Serial.print("Lampe ausgeschaltet: ");
    // Serial.println(LAMPEN_PINS[ausschaltReihenfolge[ausschaltIndex]]);
  }

  sind_die_lampen_an = false;
  // Serial.println("Abschalten fertig");
}

// Defekte Lampe bestimmen
void bestimmeDefekteLampe() {
  randomSeed(analogRead(0));
  defekteLampeIndex = random(0, LAMPEN_ANZAHL);
  defekteLampePin = LAMPEN_PINS[defekteLampeIndex];
}

// Debugausgaben für die Defekte Lampe
void _printDefekteLampe() {
  // Serial.print("Defekte Lampe: #");
  // Serial.print(defekteLampeIndex);
  // Serial.print(" => Lampe ");
  // Serial.print(LAMPEN_PINS[defekteLampeIndex]);
  // Serial.print(": ");
  // Serial.println(defekteLampePin);
}
