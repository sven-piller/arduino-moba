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

// Ausgänge mit angeschlossenen LED-Straßenlampen
const byte  LAMPEN_PINS[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
// Anzahl der Lampen
const byte LAMPEN_ANZAHL =  sizeof(LAMPEN_PINS); //10
const byte LAMPE_AN = HIGH;
const byte LAMPE_AUS = LOW;

const byte TASTER1_LED_PIN = 13;

// Taster zum Ein-/Ausschalten
const byte  TASTER1_PIN = 2;
// Kosmetische Konstante
const byte TASTER_GEDRUECKT = LOW;

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

// Bootstrapping
void setup() {
  Serial.begin(9600);
  pinMode(TASTER1_PIN, INPUT_PULLUP);
  pinMode(TASTER1_LED_PIN, OUTPUT);
  digitalWrite(TASTER1_LED_PIN, LOW);

  for (int i = 0; i < LAMPEN_ANZAHL; i++) {
    pinMode(LAMPEN_PINS[i], OUTPUT);
    digitalWrite(LAMPEN_PINS[i], LAMPE_AUS);
  }

}

// Programmlogik
void loop() {

  if (sind_die_lampen_an == false) {

    digitalWrite(TASTER1_LED_PIN, HIGH);
    Serial.println("Lampe eingeschaltet: D13");
    delay(500);
    for (int i = 0; i < LAMPEN_ANZAHL; i++) {
      digitalWrite(LAMPEN_PINS[i], LAMPE_AN);
      Serial.print("Lampe eingeschaltet: D");
      Serial.println(LAMPEN_PINS[i]);
      delay(500);
    }
    sind_die_lampen_an = true;
    Serial.println("Alle Ausgänge sind an.");
  } 
}
