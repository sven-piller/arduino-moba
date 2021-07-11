/*
  Signaldecoder Version 2.4
  geschrieben am 17.03.2017, zuletzt geändert am 3.4.2021
  von Manfred Studier

  In der Zeile "#define ADDRESS  ..." wird die Adresse eingetragen.
  Es werden nur die Adressen 1, 5, 9, 13 usw. erzeugt.
  Die Adressen 6, 7, 8 werden z. B. auf 5 zurückgesetzt.
  In den nächsten drei Zeilen wird die Version gewählt.
  Zwei von den drei Zeilen müssen herauskommentiert sein.
  Die nicht herauskommentierte Zeile bestimmt dann die Version.
  SEPARATE bestimmt den Ablauf des Lichtwechsels.
*/

#define ADDRESS  1
#define SEPARATE  // separater oder überlappter Lichtwechsel

//static unsigned char triAddr[65] = {
//  0xaa, 0xc0, 0x80, 0x30, 0xf0, 0xb0, 0x20, 0xe0, 0xa0, 0x0c,
//  0xcc, 0x8c, 0x3c, 0xfc, 0xbc, 0x2c, 0xec, 0xac, 0x08, 0xc8,
//  0x88, 0x38, 0xf8, 0xb8, 0x28, 0xe8, 0xa8, 0x03, 0xc3, 0x83,
//  0x33, 0xf3, 0xb3, 0x23, 0xe3, 0xa3, 0x0f, 0xcf, 0x8f, 0x3f,
//  0xff, 0xbf, 0x2f, 0xef, 0xaf, 0x0b, 0xcb, 0x8b, 0x3b, 0xfb,
//  0xbb, 0x2b, 0xeb, 0xab, 0x02, 0xc2, 0x82, 0x32, 0xf2, 0xb2,
//  0x22, 0xe2, 0xa2, 0x0e, 0xce
//};

#define ADDRESS1  triAddr[((ADDRESS << 1) + 6) >> 3]

// Pins
#define PIN_RAIL   2
#define PIN_ROT1   3
#define PIN_GRUEN  4
#define PIN_LED   13

// Signal Pictures ///////////////////////////////////////////////
class Picture {
  public:
    Picture(unsigned char ofs) {
      offset = ofs;
    }
    virtual void putLight(bool val) {}
  protected:
    unsigned char offset;
};

class PicHp0 : public Picture {
  public:
    PicHp0(unsigned char ofs) : Picture(ofs) {}
    void putLight(bool val) {
      digitalWrite(PIN_ROT1 + offset, !val);
    }
};

class PicHp1 : public Picture {
  public:
    PicHp1(unsigned char ofs) : Picture(ofs) {}
    void putLight(bool val) {
      digitalWrite(PIN_GRUEN + offset, !val);
    }
};

// Signals ///////////////////////////////////////////////////////
class Signal {
  public:
    Signal() {}
    virtual void switchTo(unsigned char index);
  protected:
    void dimm(Picture* currPic, Picture* newPic);
};

#define DIM 90
#define EXP 2


void Signal::dimm(Picture* currPic, Picture* newPic) // overlapped
{
  for (int dim = DIM; dim > 0; --dim) {
    currPic->putLight(HIGH);
    newPic->putLight(LOW);
    delayMicroseconds(pow(dim, EXP));
    currPic->putLight(dim == DIM);
    newPic->putLight(dim != DIM);
    delayMicroseconds(pow(DIM, EXP) - pow(dim, EXP));
  }
}


//-----------------------------------------------------------------
class BlockSignal : public Signal {
  public:
    BlockSignal(unsigned char ofs);
    void switchTo(unsigned char index);
  private:
    Picture *picture[2];
    unsigned char currIndex;
};

BlockSignal::BlockSignal(unsigned char ofs)
{
  picture[0] = new PicHp0(ofs);
  picture[1] = new PicHp1(ofs);
  picture[0]->putLight(HIGH);
  currIndex = 0;
}

void BlockSignal::switchTo(unsigned char index)
{
  index = index % 2;
  dimm(picture[currIndex], picture[index]);
  currIndex = index;
}

//-----------------------------------------------------------------
#define CNT_GAP   100
#define CHECK_TIME  40  //52

bool b, b2;
unsigned char val = 0;
unsigned char v;
unsigned char address = 0;
unsigned char mapIntAdr[8] = {0, 4, 2, 6, 1, 5, 3, 7};

// Main //////////////////////////////////////////////////////////
#define N_INP       8
Signal* intab[N_INP];

void setup() {
  pinMode(PIN_RAIL, INPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(PIN_LED, LOW);

  intab[0] = new BlockSignal(0);
  intab[1] = intab[0];
  intab[2] = new BlockSignal(2);
  intab[3] = intab[2];
  intab[4] = new BlockSignal(4);
  intab[5] = intab[4];
  intab[6] = new BlockSignal(6);
  intab[7] = intab[6];
  intab[8] = new BlockSignal(8);
  intab[9] = intab[8];

  Serial.begin(115200);
//
//  intab[0]->switchTo(0);
//  intab[2]->switchTo(0);
//  intab[4]->switchTo(0);
//  intab[6]->switchTo(0);
//  intab[8]->switchTo(0);

}

void loop() {


  digitalWrite(PIN_LED, HIGH);
  delay(50);
  digitalWrite(PIN_LED, LOW);

  intab[0]->switchTo(1);
  delay(1000);
  intab[2]->switchTo(1);
  delay(1000);
  intab[4]->switchTo(1);
  delay(1000);
  intab[6]->switchTo(1);
  delay(1000);
  intab[8]->switchTo(1);
  
  delay(5000);
  
  intab[0]->switchTo(0);
  delay(1000);
  intab[2]->switchTo(0);
  delay(1000);
  intab[4]->switchTo(0);
  delay(1000);
  intab[6]->switchTo(0);
  delay(1000);
  intab[8]->switchTo(0);
  
  delay(5000);

}
