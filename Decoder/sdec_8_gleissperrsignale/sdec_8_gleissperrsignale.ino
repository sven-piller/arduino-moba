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
unsigned int ALLE_SIGNALE_ROT = 0B0101010101010101;  // alle rot
unsigned int SIGNALZUSTAND = ALLE_SIGNALE_ROT;

// Anschlüsse des 74HC595
int IC1_LATCH_PIN = 4; // ST_CP
int IC1_CLOCK_PIN = 5; // SH_CP
int IC1_DATA_PIN = 6;  // DS

#define INPUT_PIN 2
volatile MaerklinMotorola mm(INPUT_PIN);

#define PIN_ROT_OFS    0
#define PIN_GRUEN_OFS  1

#define GSP1_ADDRESS 25
#define GSP2_ADDRESS 26
#define GSP3_ADDRESS 27
#define GSP4_ADDRESS 28
#define GSP5_ADDRESS 29
#define GSP6_ADDRESS 30
#define GSP7_ADDRESS 31
#define GSP8_ADDRESS 32

#define DIM 60
#define EXP 2

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

class PicSh0 : public Picture {
  public:
    PicSh0(unsigned char ofs) : Picture(ofs) {}
    void setLight(bool val) {
      bitWrite(SIGNALZUSTAND, PIN_ROT_OFS + offset, val);
    }
    void putLight(bool val) {
      bitWrite(SIGNALZUSTAND, PIN_ROT_OFS + offset, !val);
    }
};

class PicSh1 : public Picture {
  public:
    PicSh1(unsigned char ofs) : Picture(ofs) {}
    void setLight(bool val) {
      bitWrite(SIGNALZUSTAND, PIN_GRUEN_OFS + offset, val);
    }
    void putLight(bool val) {
      bitWrite(SIGNALZUSTAND, PIN_GRUEN_OFS + offset, !val);
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
    writePattern(SIGNALZUSTAND);
    delayMicroseconds(pow(dim, EXP));
    currPic->putLight(dim != DIM);
    newPic->putLight(dim == DIM);
    writePattern(SIGNALZUSTAND);
    delayMicroseconds(pow(DIM, EXP) - pow(dim, EXP));
  }
}

void Signal::change(Picture* currPic, Picture* newPic) // overlapped
{
  currPic->setLight(LOW);
  newPic->setLight(HIGH);
  writePattern(SIGNALZUSTAND);
}


//-----------------------------------------------------------------
class SperrSignal : public Signal {
  public:
    SperrSignal(unsigned char ofs);
    void setTo(unsigned char index);
    void switchTo(unsigned char index);
  private:
    Picture *picture[2];
    unsigned char currIndex;
};

SperrSignal::SperrSignal(unsigned char ofs)
{
  picture[0] = new PicSh0(ofs);
  picture[1] = new PicSh1(ofs);
  picture[0]->setLight(HIGH);
  picture[1]->setLight(LOW);
  currIndex = 0;
}

void SperrSignal::switchTo(unsigned char index)
{
  if (currIndex != index) {
    index = index % 2;
    dimm(picture[currIndex], picture[index]);
    currIndex = index;
  }
}

void SperrSignal::setTo(unsigned char index)
{
  if (currIndex != index) {
    change(picture[currIndex], picture[index]);
    currIndex = index;
  }
}

Signal* GSP1 = new SperrSignal(0);
Signal* GSP2 = new SperrSignal(2);
Signal* GSP3 = new SperrSignal(4);
Signal* GSP4 = new SperrSignal(6);
Signal* GSP5 = new SperrSignal(8);
Signal* GSP6 = new SperrSignal(10);
Signal* GSP7 = new SperrSignal(12);
Signal* GSP8 = new SperrSignal(14);


int SH0 = 0;
int SH1 = 1;

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

  init_Signale_Auf_Halt();
}

// Programmlogik
void loop()
{
  mm.Parse();
  MaerklinMotorolaData* Data = mm.GetData();
  if (Data) {
    if (Data->IsMagnet && Data->MagnetState) {
      DEBUG("Decoder # " + String(Data->PortAddress) + " Switch " + (Data->DecoderState));
      if (Data->PortAddress == GSP1_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP1->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP1->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP2_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP2->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP2->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP3_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP3->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP3->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP4_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP4->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP4->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP5_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP5->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP5->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP6_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP6->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP6->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP7_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP7->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP7->switchTo(SH1);
        }
      }
      if (Data->PortAddress == GSP8_ADDRESS) {
        if (Data->DecoderState == MM2DecoderState_Red) {
          GSP8->switchTo(SH0);
        }
        else if (Data->DecoderState == MM2DecoderState_Green) {
          GSP8->switchTo(SH1);
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
void init_Signale_Auf_Halt()
{
  SIGNALZUSTAND = ALLE_SIGNALE_ROT;
  digitalWrite(IC1_LATCH_PIN, LOW);
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, (SIGNALZUSTAND >> 8)); //shift out highbyte
  shiftOut(IC1_DATA_PIN, IC1_CLOCK_PIN, MSBFIRST, SIGNALZUSTAND);        //shift out lowbyte
  digitalWrite(IC1_LATCH_PIN, HIGH);

  Serial.println(SIGNALZUSTAND, BIN);
}

void isr() {
  mm.PinChange();
}

ISR(TIMER1_OVF_vect) {}
