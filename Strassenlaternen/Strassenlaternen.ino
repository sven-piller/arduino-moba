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

int alle_an = false;

int taster = 2;
byte lampen[] = {6, 8, 9, 10, 13};

// Schaltet alle Lampen auf LOW
void AllesDunkel() {
  for (int i = 0; i < sizeof(lampen); i++)
  {
    digitalWrite(lampen[i], LOW);
  }
}


// Schaltet alle Lampen auf HIGH
void AllesLeuchtet() {
  for (int i = 0; i < sizeof(lampen); i++)
  {
    digitalWrite(lampen[i], HIGH);
  }
}

void StarteLampen() {

}

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(taster, INPUT_PULLUP);
  for (int i = 0; i < sizeof(lampen); i++)
  {
    pinMode(lampen[i], OUTPUT);
    digitalWrite(lampen[i], LOW);
  }
}

void loop() {
  if (digitalRead(taster) == LOW)
  {
    // Taste gedrückt
    Serial.print("Taste gedrueckt! Alter Status: ") ; Serial.print(alle_an);
    // Status toggeln
    alle_an = !alle_an;
    //    if (alle_an == false)
    //    {
    //      alle_an = true;
    //    }
    //    else
    //    {
    //      alle_an = false;
    //    }
    Serial.print(" => Neuer Status:  "); Serial.println(alle_an);
    // Taster entprellen
    delay(200);
  }

  if (alle_an == true)
  {
    // Lampen leuchten
    AllesLeuchtet();
  }
  else
  {
    // Lampen sind aus
    AllesDunkel();
  }
}
