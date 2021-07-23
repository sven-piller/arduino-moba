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
#include <MaerklinMotorola.h>

// Anzahl der Leuchten
const byte LEUCHTEN_ANZAHL = 16;

unsigned int LEUCHTEN_PATTERN_AUS = 0B0000000000000000; // aus
unsigned int LEUCHTEN_PATTERN_AN = 0B1111111111111111;  // alle
unsigned int LEUCHTEN_PATTERN_ACTIVE;
unsigned int ALLE_SIGNALE_ROT = 0B0101010101010101;  // alle rot

// Taster zum Ein-/Ausschalten
const byte TASTER1_PIN = 3;
const byte TASTER_GEDRUECKT = LOW;

// Anschlüsse des 74HC595
int IC1_LATCH_PIN = 4; // ST_CP
int IC1_CLOCK_PIN = 5; // SH_CP
int IC1_DATA_PIN = 6;  // DS


#define INPUT_PIN 2
volatile MaerklinMotorola mm(INPUT_PIN);

#define PIN_ROT_OFS    0
#define PIN_GRUEN_OFS  1

#define S1_ADDRESS 25
#define S2_ADDRESS 26
#define S3_ADDRESS 27
#define S4_ADDRESS 28
#define S5_ADDRESS 29
#define S6_ADDRESS 30
#define S7_ADDRESS 31
#define S8_ADDRESS 32



int counter = 0;


// Signal Pictures ///////////////////////////////////////////////
class Picture {
  public:
    Picture(unsigned char ofs) {
      offset = ofs;
    }
    virtual void setLight(bool val) {}
    virtual void putLight(bool val) {}
  protected:
    unsigned char offset;
};

class PicHp0 : public Picture {
  public:
    PicHp0(unsigned char ofs) : Picture(ofs) {}
    void setLight(bool val) {
      bitWrite(LEUCHTEN_PATTERN_ACTIVE, PIN_ROT_OFS + offset, val);
    }
    void putLight(bool val) {
      bitWrite(LEUCHTEN_PATTERN_ACTIVE, PIN_ROT_OFS + offset, !val);
    }
};

class PicHp1 : public Picture {
  public:
    PicHp1(unsigned char ofs) : Picture(ofs) {}
    void setLight(bool val) {
      bitWrite(LEUCHTEN_PATTERN_ACTIVE, PIN_GRUEN_OFS + offset, val);
    }
    void putLight(bool val) {
      bitWrite(LEUCHTEN_PATTERN_ACTIVE, PIN_GRUEN_OFS + offset, !val);
    }
};

// Signals ///////////////////////////////////////////////////////
class Signal {
  public:
    Signal() {}
    virtual void setTo(unsigned char index);
    virtual void switchTo(unsigned char index);
  protected:
    void writePattern(unsigned int pattern);
    void dimm(Picture* currPic, Picture* newPic);
    void change(Picture* currPic, Picture* newPic);
};

#define DIM 60
#define EXP 2

void Signal::writePattern(unsigned int pattern) {
  //   Serial.println(pattern, BIN);
  digitalWrite(IC1_LATCH_PIN, LOW);
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (pattern >> 8)); //shift out highbyte
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, pattern);        //shift out lowbyte
  digitalWrite(IC1_LATCH_PIN, HIGH);
}


void Signal::dimm(Picture* currPic, Picture* newPic) // overlapped
{
  for (int dim = DIM; dim > 0; --dim) {
    currPic->putLight(LOW);
    newPic->putLight(HIGH);
    writePattern(LEUCHTEN_PATTERN_ACTIVE);
    delayMicroseconds(pow(dim, EXP));
    currPic->putLight(dim != DIM);
    newPic->putLight(dim == DIM);
    writePattern(LEUCHTEN_PATTERN_ACTIVE);
    delayMicroseconds(pow(DIM, EXP) - pow(dim, EXP));
  }
}

void Signal::change(Picture* currPic, Picture* newPic) // overlapped
{
  currPic->setLight(LOW);
  newPic->setLight(HIGH);
  writePattern(LEUCHTEN_PATTERN_ACTIVE);
}


//-----------------------------------------------------------------
class BlockSignal : public Signal {
  public:
    BlockSignal(unsigned char ofs);
    void setTo(unsigned char index);
    void switchTo(unsigned char index);
  private:
    Picture *picture[2];
    unsigned char currIndex;
};

BlockSignal::BlockSignal(unsigned char ofs)
{
  picture[0] = new PicHp0(ofs);
  picture[1] = new PicHp1(ofs);
  picture[0]->setLight(HIGH);
  picture[1]->setLight(LOW);
  currIndex = 0;
}

void BlockSignal::switchTo(unsigned char index)
{
  if (currIndex != index) {
    index = index % 2;
    dimm(picture[currIndex], picture[index]);
    currIndex = index;
  }
}

void BlockSignal::setTo(unsigned char index)
{
  if (currIndex != index) {
    change(picture[currIndex], picture[index]);
    currIndex = index;
  }
}

Signal* S1 = new BlockSignal(0);
Signal* S2 = new BlockSignal(2);
Signal* S3 = new BlockSignal(4);
Signal* S4 = new BlockSignal(6);
Signal* S5 = new BlockSignal(8);
Signal* S6 = new BlockSignal(10);
Signal* S7 = new BlockSignal(12);
Signal* S8 = new BlockSignal(14);


int HP0 = 0;
int HP1 = 1;

// Bootstrapping
void setup()
{
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), isr, CHANGE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  TCCR1A = 0;
  TCCR1B = 0;
  bitSet(TCCR1B, CS12);  // 256 prescaler
  bitSet(TIMSK1, TOIE1); // timer overflow interrupt

  pinMode(IC1_LATCH_PIN, OUTPUT);
  pinMode(IC1_CLOCK_PIN, OUTPUT);
  pinMode(IC1_DATA_PIN, OUTPUT);

  alles_halt();
}

// Programmlogik
void loop()
{
  mm.Parse();
  MaerklinMotorolaData* Data = mm.GetData();
  if (Data) {
    if (Data->IsMagnet && Data->MagnetState) {
      DEBUG("Decoder # " + String(Data->PortAddress) + " Switch " + (Data->DecoderState));
      if (Data->PortAddress == S1_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S1->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S1->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S2_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S2->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S2->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S3_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S3->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S3->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S4_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S4->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S4->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S5_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S5->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S5->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S6_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S6->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S6->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S7_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S7->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S7->switchTo(HP1);
        }
      }
      if (Data->PortAddress == S8_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          S8->switchTo(HP0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          S8->switchTo(HP1);
        }
      }
    }
    delay(50);
  }
}


void DEBUG(String message) {
  Serial.println( String(millis()) + " [DEB]: " + message);
}


// Schaltet alle Signale auf Rot
void alles_halt()
{
  LEUCHTEN_PATTERN_ACTIVE = ALLE_SIGNALE_ROT;
  digitalWrite(IC1_LATCH_PIN, LOW);
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (LEUCHTEN_PATTERN_ACTIVE >> 8)); //shift out highbyte
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, LEUCHTEN_PATTERN_ACTIVE);        //shift out lowbyte
  digitalWrite(IC1_LATCH_PIN, HIGH);

  Serial.println(LEUCHTEN_PATTERN_ACTIVE, BIN);
}

void isr() {
  mm.PinChange();
}

ISR(TIMER1_OVF_vect) {
  /*if (hasNewData) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    hasNewData = false;
    } else {
    digitalWrite(LED_BUILTIN, LOW);
    }*/
}
