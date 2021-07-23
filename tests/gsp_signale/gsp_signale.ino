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

  Mit Ideen von:
  - http://ediy.com.my/blog/item/87-using-74hc595-shift-registers-with-arduino

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
unsigned int LEUCHTEN_PATTERN_ACTIVE;

// Anschlüsse des 74HC595
int IC1_LATCH_PIN = 4; // ST_CP
int IC1_CLOCK_PIN = 5; // SH_CP
int IC1_DATA_PIN = 6;  // DS

int counter = 0;

// Taster zum Ein-/Ausschalten
const byte TASTER1_PIN = 2;
const byte TASTER_GEDRUECKT = LOW;
// LED Anzeige, ob Programm aktiv ist
const byte TASTER1_LED_PIN = 13;
// Ausgang für nachrangige Arduinos
const byte CONNEX_N2_PIN = 7;

// Aktueller Zustand der Lampen
bool sind_die_lampen_an = false;

// Bootstrapping
void setup()
{
  Serial.begin(9600);
  pinMode(TASTER1_PIN, INPUT_PULLUP);
  pinMode(TASTER1_LED_PIN, OUTPUT);
  digitalWrite(TASTER1_LED_PIN, LOW);

  pinMode(IC1_LATCH_PIN, OUTPUT);
  pinMode(IC1_CLOCK_PIN, OUTPUT);
  pinMode(IC1_DATA_PIN, OUTPUT);

  pinMode(CONNEX_N2_PIN, OUTPUT);
  digitalWrite(CONNEX_N2_PIN, LOW);

  lampen_aus();
}

// Programmlogik
void loop()
{
  if (digitalRead(TASTER1_PIN) == TASTER_GEDRUECKT)
  {
    // Serial.println("Taste gedrueckt! ");
    Serial.print("Counter: ");
    Serial.println(counter);

    if (counter == 0)
    {
      lampen_an();
      counter++;
    }
    else if (counter == 16)
    {
      counter = 0;
      lampen_aus();
    }
    else
    {
      if (counter < 16) {
        bitWrite(LEUCHTEN_PATTERN_ACTIVE, counter, 0);
      }
      Serial.println(LEUCHTEN_PATTERN_ACTIVE, BIN);
      digitalWrite(IC1_LATCH_PIN, LOW);
      shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN_ACTIVE >> 8)); //shift out highbyte
      shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_ACTIVE);        //shift out lowbyte
      digitalWrite(IC1_LATCH_PIN, HIGH);
      counter++;
    }

    // Taster entprellen
    delay(250);
  }
}

// Schaltet alle Leuchten aus
void lampen_aus()
{
  digitalWrite(IC1_LATCH_PIN, LOW);
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN_AUS >> 8)); //shift out highbyte
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AUS);        //shift out lowbyte
  digitalWrite(IC1_LATCH_PIN, HIGH);
  sind_die_lampen_an = false;
  LEUCHTEN_PATTERN_ACTIVE = LEUCHTEN_PATTERN_AUS;
  delay(500);
}

// Schaltet alle Leuchten der Reihe nach ein
void lampen_an()
{
  for (int i = 0; i < LEUCHTEN_ANZAHL; i++)
  {
    digitalWrite(IC1_LATCH_PIN, LOW);
    shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN[i] >> 8)); //shift out highbyte
    shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN[i]);        //shift out lowbyte
    digitalWrite(IC1_LATCH_PIN, HIGH);
    delay(200);
  }
  digitalWrite(IC1_LATCH_PIN, LOW);
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN_AN >> 8)); //shift out highbyte
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AN);        //shift out lowbyte
  digitalWrite(IC1_LATCH_PIN, HIGH);
  sind_die_lampen_an = true;
  LEUCHTEN_PATTERN_ACTIVE = LEUCHTEN_PATTERN_AN;
  delay(200);
}
