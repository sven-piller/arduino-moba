
const byte GSP1_T = A0;
const byte GSP1_R = 2;
const byte GSP1_W = 3;
boolean GSP1_Z = 0;

const byte GSP2_T = A1;
const byte GSP2_R = 4;
const byte GSP2_W = 5;
boolean GSP2_Z = 0;

const byte GSP3_T = A2;
const byte GSP3_R = 6;
const byte GSP3_W = 7;
boolean GSP3_Z = 0;

const byte GSP4_T = A3;
const byte GSP4_R = 8;
const byte GSP4_W = 9;
boolean GSP4_Z = 0;

const byte LED_AN = LOW;
const byte LED_AUS = HIGH;
const byte GEDRUECKT = LOW;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(GSP1_T, INPUT_PULLUP);
  pinMode(GSP1_R, OUTPUT);
  pinMode(GSP1_W, OUTPUT);

  pinMode(GSP2_T, INPUT_PULLUP);
  pinMode(GSP2_R, OUTPUT);
  pinMode(GSP2_W, OUTPUT);

  pinMode(GSP3_T, INPUT_PULLUP);
  pinMode(GSP3_R, OUTPUT);
  pinMode(GSP3_W, OUTPUT);

  pinMode(GSP4_T, INPUT_PULLUP);
  pinMode(GSP4_R, OUTPUT);
  pinMode(GSP4_W, OUTPUT);

  grundstellung();
}

void grundstellung() {

  digitalWrite(GSP1_R, LED_AN);
  digitalWrite(GSP1_W, LED_AUS);
  digitalWrite(GSP2_R, LED_AN);
  digitalWrite(GSP2_W, LED_AUS);
  digitalWrite(GSP3_R, LED_AN);
  digitalWrite(GSP3_W, LED_AUS);
  digitalWrite(GSP4_R, LED_AN);
  digitalWrite(GSP4_W, LED_AUS);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(GSP1_T) == GEDRUECKT) {
    delay(50);
    Serial.println("Signal 1");
    if (GSP1_Z == 0) {
      Serial.println("Signal 1 -> Weiß");
      GSP1_Z = 1;
      digitalWrite(GSP1_R, LED_AUS);
      digitalWrite(GSP1_W, LED_AN);
      delay(250);
    }
    else {
      Serial.println("Signal 1 -> Rot");
      GSP1_Z = 0;
      digitalWrite(GSP1_R, LED_AN);
      digitalWrite(GSP1_W, LED_AUS);
      delay(250);
    }
  }


  if (digitalRead(GSP2_T) == GEDRUECKT) {
    delay(50);
    Serial.println("Signal 2");
    if (GSP2_Z == 0) {
      Serial.println("Signal 2 -> Weiß");
      GSP2_Z = 1;
      digitalWrite(GSP2_R, LED_AUS);
      digitalWrite(GSP2_W, LED_AN);
      delay(250);
    } else {
      Serial.println("Signal 2 -> Rot");
      GSP2_Z = 1;
      digitalWrite(GSP2_R, LED_AN);
      digitalWrite(GSP2_W, LED_AUS);
      delay(250);
    }
  }


  if (digitalRead(GSP3_T) == GEDRUECKT) {
    delay(50);
    Serial.println("Signal 3");
    if (GSP3_Z == 0) {
      Serial.println("Signal 3 -> Weiß");
      GSP3_Z = 1;
      digitalWrite(GSP3_R, LED_AUS);
      digitalWrite(GSP3_W, LED_AN);
      delay(250);
    } else {
      Serial.println("Signal 3 -> Rot");
      GSP3_Z = 1;
      digitalWrite(GSP3_R, LED_AN);
      digitalWrite(GSP3_W, LED_AUS);
      delay(250);
    }
  }


  if (digitalRead(GSP4_T) == GEDRUECKT) {
    delay(50);
    Serial.println("Signal 4");
    if (GSP4_Z == 0) {
      Serial.println("Signal 4 -> Weiß");
      GSP4_Z = 1;
      digitalWrite(GSP4_R, LED_AUS);
      digitalWrite(GSP4_W, LED_AN);
      delay(250);
    } else {
      Serial.println("Signal 4 -> Rot");
      GSP4_Z = 1;
      digitalWrite(GSP4_R, LED_AN);
      digitalWrite(GSP4_W, LED_AUS);
      delay(250);
    }
  }


}
