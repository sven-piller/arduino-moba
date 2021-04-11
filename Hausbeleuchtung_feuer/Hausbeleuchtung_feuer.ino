/*

  Dieser Sketch ist für die Hausbeleuchtung und das brennende Haus vorgesehen.

  Geplante Funktionsweise:
  1. Einschalten des brennenden Hauses über Tastendruck
  2. Das Relais für Rauchtopf und Brandflackern wird geschaltet
  3. Nach Verzögerung geht die normale Beleuchtung aus, falls an
  4. Auf Tastendruck gehen alle Leuchten wieder aus
  5. Nach Verzögerung geht die normale Beleuchtung wieder an, falls allgemein an
  6. Über einen PIN wird die normale Hausbeleuchtung geschaltet (siehe Hausbeleuchtung_main)
  7. Die Fabrik wird über einen gesonderten Schalter betrieben

  2021-03-26
  Sven Piller

  Mit Ideen von:
  - http://ediy.com.my/blog/item/87-using-74hc595-shift-registers-with-arduino

  Historie:
  - v1: 2021-03-28 Erste Version für die Anlage

*/

// Anzahl der Leuchten
const byte LEUCHTEN_ANZAHL = 6;
unsigned char LEUCHTEN_PATTERN[LEUCHTEN_ANZAHL] = {
  B00000001, // Fabrik Anbau
  B00000010, // Fabrik EG
  B00000100, // Fabrik OG
  B00001000, // Bauernhaus
  B00010000, // Brennendes Haus
  B00100000, // Gasthaus
  // B01000000 // leer
  // B10000000 // leer
};
unsigned char LEUCHTEN_PATTERN_AUS = B00000000;      // aus
unsigned char LEUCHTEN_PATTERN_AN = B00111111;       // alle
unsigned char LEUCHTEN_PATTERN_AN_FEUER = B00111111; // alle ohne brennendes Haus

// Anschlüsse des 74HC595
int IC2_LATCH_PIN = 4; // ST_CP
int IC2_CLOCK_PIN = 5; // SH_CP
int IC2_DATA_PIN = 6;  // DS

// Taster zum Ein-/Ausschalten
const byte TASTER_DEAD_PIN = 2;
const byte TASTER_GEDRUECKT = LOW;
// Eingang für vorangigen Arduino bzgl. Hausbeleuchtung
const byte CONNEX_N2_PIN = 7;

// Taster zum Ein-/Ausschalten
const byte TASTER2_PIN = 8;
// LED Anzeige, ob Programm aktiv ist
const byte TASTER2_LED_PIN = 13;
// Pins für das Relais, welches den Rauchtopf und das Brandflackern steuert
const byte RELAIS_ZUSTAND1_PIN = 9;
const byte RELAIS_ZUSTAND2_PIN = 10;

// Aktueller Zustand der Lampen
bool sind_die_lampen_an = false;
// Aktueller Zustand des Feuers
bool ist_das_feuer_an = false;

// Bootstrapping
void setup()
{
  // Serial.begin(9600);
  pinMode(TASTER_DEAD_PIN, INPUT_PULLUP);
  pinMode(CONNEX_N2_PIN, INPUT);
  pinMode(IC2_LATCH_PIN, OUTPUT);
  pinMode(IC2_CLOCK_PIN, OUTPUT);
  pinMode(IC2_DATA_PIN, OUTPUT);

  pinMode(TASTER2_PIN, INPUT_PULLUP);
  pinMode(RELAIS_ZUSTAND1_PIN, OUTPUT);
  digitalWrite(RELAIS_ZUSTAND1_PIN, LOW);
  pinMode(RELAIS_ZUSTAND2_PIN, OUTPUT);
  digitalWrite(RELAIS_ZUSTAND2_PIN, LOW);
  pinMode(TASTER2_LED_PIN, OUTPUT);
  digitalWrite(TASTER2_LED_PIN, LOW);

  lampen_aus();
  feuer_aus();
}

void loop()
{
  // Logik für die normale Hausbeleuchtung
  if (digitalRead(CONNEX_N2_PIN) == HIGH && sind_die_lampen_an == false)
  {
    lampen_an();
  }

  if (digitalRead(CONNEX_N2_PIN) == LOW && sind_die_lampen_an == true)
  {
    lampen_aus();
  }

  // Logik für die lokale Hausbeleuchtung
  // TODO: Eventuell ausbauen
  if (digitalRead(TASTER_DEAD_PIN) == TASTER_GEDRUECKT)
  {

    if (sind_die_lampen_an == true)
    {
      lampen_aus();
    }
    else
    {
      lampen_an();
    }

    // Taster entprellen
    delay(200);
  }

  // Logik für das brennende Haus
  if (digitalRead(TASTER2_PIN) == TASTER_GEDRUECKT)
  {

    if (ist_das_feuer_an == true)
    {
      feuer_aus();
    }
    else
    {
      feuer_an();
    }

    // Taster entprellen
    delay(200);
  }
}

/*
  Schaltet das simulierte Feuer an

  Dabei wird erst das Relais geschaltet und im
  Anschluss dann die reguläre Beleuchtung
*/
void feuer_an()
{

  relais_ein();
  ist_das_feuer_an = true;
  delay(200);

  if (sind_die_lampen_an == true)
  {
    delay(3000);
    digitalWrite(IC2_LATCH_PIN, LOW);
    shiftOut(IC2_DATA_PIN, IC2_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AN_FEUER);
    digitalWrite(IC2_LATCH_PIN, HIGH);
  }
}

/*
  Schaltet das simulierte Feuer aus

  Dabei wird erst das Relais geschaltet und im
  Anschluss dann die reguläre Beleuchtung
*/
void feuer_aus()
{

  relais_aus();
  ist_das_feuer_an = false;
  delay(200);

  if (sind_die_lampen_an == true)
  {
    delay(3000);
    digitalWrite(IC2_LATCH_PIN, LOW);
    shiftOut(IC2_DATA_PIN, IC2_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AN);
    digitalWrite(IC2_LATCH_PIN, HIGH);
  }
}

// Schaltet alle Leuchten aus
void lampen_aus()
{
  digitalWrite(IC2_LATCH_PIN, LOW);
  shiftOut(IC2_DATA_PIN, IC2_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AUS);
  digitalWrite(IC2_LATCH_PIN, HIGH);
  sind_die_lampen_an = false;
  delay(500);
}

// Schaltet alle Leuchten der Reihe nach ein
void lampen_an()
{
  for (int i = 0; i < LEUCHTEN_ANZAHL; i++)
  {
    digitalWrite(IC2_LATCH_PIN, LOW);
    shiftOut(IC2_DATA_PIN, IC2_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN[i]);
    digitalWrite(IC2_LATCH_PIN, HIGH);
    delay(200);
  }
  digitalWrite(IC2_LATCH_PIN, LOW);
  shiftOut(IC2_DATA_PIN, IC2_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_AN);
  digitalWrite(IC2_LATCH_PIN, HIGH);
  sind_die_lampen_an = true;
  delay(200);
}

// Schaltet das Relais in Zustand 1 (Strom)
void relais_ein()
{
  digitalWrite(RELAIS_ZUSTAND1_PIN, HIGH);
  delay(100);
  digitalWrite(RELAIS_ZUSTAND1_PIN, LOW);
}

// schaltet das Relais in Zustand 2 (Leer)
void relais_aus()
{
  digitalWrite(RELAIS_ZUSTAND2_PIN, HIGH);
  delay(100);
  digitalWrite(RELAIS_ZUSTAND2_PIN, LOW);
}
