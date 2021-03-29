/*

  Dieser Sketch ist für die Hausbeleuchtung vorgesehen.

  Geplante Funktionsweise:
  1. Einschalten über Tastendruck
  2. Leuchte für Leuchte gehen die Lampen zufallsbasiert an
  3. Zwischenzeitlich gehen manche Leuchten wieder aus
  4. Auf Tastendruck gehen alle Leuchten wieder aus
  5. Der Tastendruck wird an nachrangige Client-Arduinos weitergereicht

  2021-03-26
  Sven Piller

  Historie:
  - v1: 2021-03-28 Erste Version für die Anlage

*/

// Anzahl der Leuchten
const byte LEUCHTEN_ANZAHL = 16;
unsigned int LEUCHTEN_PATTERN[LEUCHTEN_ANZAHL] = {
  0B0000000000000001, // Bäckerei
  0B0000000000000010, // Biker-Shop
  0B0000000000000100, // Gästehaus
  0B0000000000001000, // Leeres Geschäft (Reihenhaus)
  0B0000000000010000, // Gelbes Reihenhaus
  0B0000000000100000, // Blaues Reihenhaus
  0B0000000001000000, // Spielwarengeschäft (Reihenhaus)
  0B0000000010000000, // Haus mit Turm
  0B0000000100000000, // Torhaus
  0B0000001000000000, // Haus mit Treppenaufgang
  0B0000010000000000, // Siedlungshaus
  0B0000100000000000, // Stadthaus (gelb)
  0B0001000000000000, // Kirche
  0B0010000000000000, // Gerätehaus
  0B0100000000000000, // Feuerwehr
  0B1000000000000000, // Villa

};
unsigned int LEUCHTEN_PATTERN_AUS = 0B0000000000000000; // aus
unsigned int LEUCHTEN_PATTERN_AN = 0B1111111111111111;  // alle

// Anschlüsse des 74HC595
int LATCH_PIN = 4; // ST_CP
int CLOCK_PIN = 5; // SH_CP
int DATA_PIN = 6;  // DS

// Taster zum Ein-/Ausschalten
const byte TASTER_PIN = 2;
const byte TASTER_GEDRUECKT = LOW;
// LED Anzeige, ob Programm aktiv ist
const byte INDICATOR_LED_PIN = 13;
// Ausgang für nachrangige Arduinos.
const byte CLIENT_PIN = 7;

// Aktueller Zustand der Lampen
bool sind_die_lampen_an = false;

// Bootstrapping
void setup()
{
  // Serial.begin(9600);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW);

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  pinMode(CLIENT_PIN, OUTPUT);
  digitalWrite(CLIENT_PIN, LOW);

  lampen_aus();
}

// Programmlogik
void loop()
{
  if (digitalRead(TASTER_PIN) == TASTER_GEDRUECKT)
  {
    // Serial.print("Taste gedrueckt! ");

    if (sind_die_lampen_an == true)
    {
      // Serial.println("An => Aus");
      digitalWrite(CLIENT_PIN, LOW);
      lampen_aus();
      digitalWrite(INDICATOR_LED_PIN, LOW);
    }
    else
    {
      // Serial.println("Aus => An");
      digitalWrite(CLIENT_PIN, HIGH);
      lampen_an();
      digitalWrite(INDICATOR_LED_PIN, HIGH);
    }

    // Taster entprellen
    delay(200);
  }
}

// Schaltet alle Leuchten aus
void lampen_aus()
{
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN_AUS >> 8)); //shift out highbyte
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AUS);        //shift out lowbyte
  digitalWrite(LATCH_PIN, HIGH);
  sind_die_lampen_an = false;
  delay(500);
}

// Schaltet alle Leuchten der Reihe nach ein
void lampen_an()
{
  for (int col = 0; col < LEUCHTEN_ANZAHL; col++)
  {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN[col] >> 8)); //shift out highbyte
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN[col]);        //shift out lowbyte
    digitalWrite(LATCH_PIN, HIGH);
    delay(200);
  }
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN_AN >> 8)); //shift out highbyte
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AN);        //shift out lowbyte
  digitalWrite(LATCH_PIN, HIGH);
  sind_die_lampen_an = true;
  delay(200);
}
