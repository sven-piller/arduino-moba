/*  Simulation "belebtes Haus für die Modellbahn" */
/*  Version "Flimmern nur wenn die Pin-13 LED an ist */
byte pwmFlimmerPin=6; // PWM Pin für das "analoge" Flimmern
long flimmerStart,flimmerEnd; // Anfangs- und Endzeiten für das Flimmern
// LED Pins für das normale Blinken deklarieren
byte leds[]   ={9,   11,   8,   13, 10};
// LED Blinktakt in Millisekunden für diese Pins
long ledtakt[]={22000, 30333, 50000, 92100, 11000};
// Variablen zum Merken von millis()-Zeiten beim Schalten/Blinken
long ledtime[sizeof(leds)];

void setup() {
//  Serial.begin(9600);
  // Alle verwendeten Pins auf OUTPUT setzen
  pinMode(pwmFlimmerPin, OUTPUT);  
  for (int i=0;i<sizeof(leds);i++)
    pinMode(leds[i], OUTPUT);
}

boolean milliSekundenTakt(int dauer, long &alterWert) {
// Parameter "dauer": Dauer einer Blinkphase (an bzw. aus)
// Parameter "alterWert": Variable zum Speichern des millis() Timers
// Rückgabewert: true wenn die Zeit bis zum nächsten Umschalten abgelaufen ist, sonst false
  if (millis() - alterWert < dauer) return false;
  while (millis() - alterWert >= dauer) alterWert+=dauer;
  return true;  
}

void blinkenImTakt() {
  
// Alle gleichmäßig blinkenden LEDs in ihrem eigenen Takt blinken lassen  
  for (int i=0;i<sizeof(leds);i++) // alle LEDs in einer Schleife durchgehen
  {
    if (milliSekundenTakt(ledtakt[i],ledtime[i])) // Takt für diese LED abgelaufen?
    {
      digitalWrite(leds[i],!digitalRead(leds[i]));  // wenn ja ==> Umschalten
      if (leds[i]==13 && digitalRead(leds[i])) // Pin-13 LED wurde gerade eingeschaltet
      {
       flimmerStart=millis()+2000; // Anfangszeit für das Flimmern setzen
       flimmerEnd =millis()+25000;  // Endzeit für das Flimmern setzen
      }  
//      Serial.print("Pin ");Serial.print(leds[i]);Serial.print(" = ");Serial.println(digitalRead(leds[i]));
    }
  }
}

void flimmernPwmPin() {
// Die flimmernde LED im Zufallsmodus flimmern lassen
  static long alterWert;
  static int flimmerDauer=200;
  static byte flimmerHelligkeit;
  if (milliSekundenTakt(flimmerDauer,alterWert)) // Takt abgelaufen?
  {
    flimmerDauer=1+random(200); // neue Flimmerdauer als Zufallswert
    if (millis()>flimmerStart && millis()<flimmerEnd)
      flimmerHelligkeit=random(256); // neue Flimmerhelligkeit als Zufallswert
    else  
      flimmerHelligkeit=0; // Flimmern aus wenn falsche Zeit zum Flimmern
    analogWrite(pwmFlimmerPin, flimmerHelligkeit); 
  }  
}

void loop() {
  blinkenImTakt();
  flimmernPwmPin();
}
