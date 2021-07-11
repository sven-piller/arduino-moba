

#define In 2
int AlleAn = false;
int out;
int outDefekt;
int Pause1;
int defekteLampeFlackerzeit;
int einschaltWahrscheinlichkeit;



void setup() {
  pinMode(2,INPUT_PULLUP);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  Serial.begin(9600);
}

void loop() {
//-----------------------------------------------------------
//__________Alle Lampen an mit Zufälligem Flackern___________ 
//-----------------------------------------------------------  
  if ((digitalRead(In)) == HIGH && AlleAn == false) {
    for (int i=0; i<100; i++){
      randomSeed(analogRead(0)); 
      out = random(3, 12);
      Pause1 = random(5, 50);
      einschaltWahrscheinlichkeit = random(0, 8);
      if (einschaltWahrscheinlichkeit > 0){
        digitalWrite(out,HIGH);
      }
      else{
        digitalWrite(out,LOW);
      }
      delay(Pause1);
      if (i==99){
        for(out=3;out<12;out++){
          digitalWrite(out,HIGH);
          AlleAn = true;
        }
      }
    } 
//-----------------------------------------------------------
//___________Welche Lampe soll später flackern_______________
//-----------------------------------------------------------
    randomSeed(analogRead(0)+millis());    
    outDefekt = random(3, 9); 
    Serial.println(outDefekt);
  }
//-----------------------------------------------------------  
//______________________Alle Leuchten aus____________________
//-----------------------------------------------------------   
  if ((digitalRead(In)) == LOW){
    for(out=3;out<12;out++){
      digitalWrite(out,LOW);
      AlleAn = false;
    }
  }
//-----------------------------------------------------------  
//___________Zufälliges Flackern einer Lampe_________________
//-----------------------------------------------------------   
  if (AlleAn == true){
    if (digitalRead(outDefekt) == HIGH){
      digitalWrite(outDefekt, LOW);
      defekteLampeFlackerzeit = random(300, 1000);
    }
    else{
      digitalWrite(outDefekt, HIGH);
      defekteLampeFlackerzeit = random(5, 300);
    }
    for(int i=0; i<defekteLampeFlackerzeit; i++){          //hier wird eine Pause erzeugt
      delay(1);                           //welche unterbrochen werden
      if ((digitalRead(In)) == LOW) {     //kann.
        i = defekteLampeFlackerzeit;
      }
    }
  }
}
