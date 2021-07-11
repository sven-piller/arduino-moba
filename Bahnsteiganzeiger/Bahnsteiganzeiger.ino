// Zugzielanzeiger für Bahnsteige, KDM Version 14
//
//In Stummiforum (https://stummiforum.de/viewtopic.php?f=21&t=131472) hat Tobias siener Sketch teilt.
//
//Ich hab mehrere änderungen hingeführt und hier ist meiner version "Zugzielanzeiger für den Bahnsteig mit Arduino"
//
//Noten:
//
//
// Umlaute ersetzten laut folgender Chiffre. Dazu koennen Hex Codes (0xE4) oder Octal Codes (\344) verwendet werden
// Die oktalen Codes enthalten immer 3 Ziffern. Dadurch koennen diese besser zusammen mit darauf folgenden
// Ziffern verwendet werden.
//   ae  = \344
//   oe  = \366
//   ue  = \374
//   ss  = \337
//   Ae  = \304
//   Oe  = \326
//   Ue  = \334
//   3/4 = \276
// Wuerzburg ist daher W\374rzburg
// Verspaetung ist daher Versp\344tung
//   ä = \xE4
//   ö = \xF6
//   ü = \xFC
//   ß = \xDF
//   Ä = \xC4
//   Ö = \xD6
//   Ü = \xDC
//Würzburg ist daher W\xFCrzburg
//Verspätung ist daher Versp\xF4tung
//Dänisch:
//   æ = \xE6
//   ø = \xF8
//   å = \xE5
//   Æ = \xC6
//   Ø = \xD8
//   Å = \xC5
//Alle charakteren at: http://www.codetable.net/unicodecharacters

#include <U8glib.h>
#include "Push_Button.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

#define BUTTON0_PIN 3 // Activate the next text message from Flash fo the current OLED

int count = 0;
unsigned long previousMillis = 0; // Speichern letzte zeit der anziege hat sich geändert
const long interval = 31500;      // interval zwischen änderungen (milliseconds)
const int ANZAHL_ANZEIGE = 7;     // anzahl varianten
char gleis[5];

Push_Button Button0(BUTTON0_PIN);

char uhrzeit[ANZAHL_ANZEIGE][6] {
  // [6] ist anzahl ziffern
  "09:34",
  "10:04",
  "12:53",
  "15:21",
  "17:02",
  "18:30",
  "21:45"
};
char zugnummer[ANZAHL_ANZEIGE][8] {
  // [7] ist anzahl charakteren
  "HLB2872",
  "HLB2873",
  "EC 172 ",
  "EC 592 ",
  "IC 60  ",
  "RE 4447",
  "ICE 61 ",
};
char ziel[ANZAHL_ANZEIGE][17] {
  // [17] ist anzahl charakteren
  "Aschaffenburg",
  "Wiesbaden Hbf",
  "Hamburg-Altona",
  "Dortmund",
  "Puttgarden",
  "Kiel / Flensburg",
  "Leipzig Hbf",
};
char zuglauf1[ANZAHL_ANZEIGE][21] {
  //[21] ist anzahl charakteren
  "Gro\337auheim - Kahl,",
  "Darmstadt Hbf",
  "Kassel - Hannover",
  "Limburg - K\366ln",
  "Wuppertal - Dortmund",
  "Elmshorn - ",
  "Fulda - Eisenach"
};
char zuglauf2[ANZAHL_ANZEIGE][21] {
  //[21] ist anzahl charakteren
  "- Gro\337krotzenburg",
  "- Mainz Hbf",
  "- Berlin Hbf",
  " - D\374sseldorf",
  "Bremen - Hamburg",
  "Ne\xFCmunster",
  ""
};
char wagenstand[ANZAHL_ANZEIGE][9] {
  //[9] ist anzahl charakteren
  "",
  "",
  "  222211",
  "11111   ",
  "  22111 ",
  "   2121 ",
  "111211 "
};
char lauftext[ANZAHL_ANZEIGE][85] {
  //[85] ist anzahl charakteren, mindstens 20 leerzeichnen um die lauftext zu haben
  "",
  "                    Versp\xF4tung ca 10 Min",
  "",
  "",
  "",
  "                    Zugteilung in Ne\xFCmunster - Vorne zugteil fahrt bis Flensburg    ",
  ""
};

const unsigned int LAUFTEXT_L = 21;
int offset = 0;
int subset = 0;
char ausgabe[LAUFTEXT_L];

char incomingByte = 0;
unsigned int incomingBytePosition = 0;

enum ReceiveState
{
  ReceiveState_Idle,
  ReceiveState_Hash,
  ReceiveState_Lauftext,
  ReceiveState_Gleis,
  ReceiveState_Wagenstand,
  ReceiveState_Zuglauf1,
  ReceiveState_Zuglauf2,
  ReceiveState_Ziel,
  ReceiveState_Uhrzeit,
  ReceiveState_Zugnummer
};

ReceiveState currentState = ReceiveState_Idle;

enum GleisSeite
{
  GleisSeite_Links,
  GleisSeite_Rechts
};

GleisSeite gleisSeite = GleisSeite_Links; // definiert der layout der zugzielanzieger

void draw()
{
  u8g.setColorIndex(1);
  static int zugdatenSpalte = 0;
  if (gleisSeite == GleisSeite_Links)
  {
    zugdatenSpalte = 100;
  }
  else if (gleisSeite == GleisSeite_Rechts)
  {
    zugdatenSpalte = 0;
  }

  u8g.setFont(u8g_font_5x8); // 5x8
  u8g.drawStr(zugdatenSpalte, u8g.getFontAscent(), uhrzeit[count]);

  u8g.setFont(u8g_font_4x6); // 4x6
  u8g.drawStr(zugdatenSpalte, 7 + u8g.getFontAscent(), zugnummer[count]);

  if (strlen(wagenstand[count]) > 0)
  {
    u8g.setColorIndex(1);
    u8g.setFont(u8g_font_4x6); // 4x6
    u8g.drawStr(zugdatenSpalte, 15 + u8g.getFontAscent(), "ABCDEFG");
    u8g.drawBox(zugdatenSpalte, 21 + u8g.getFontAscent() - 5, strlen(wagenstand[count]) * 4, 6);
    u8g.setColorIndex(0);
    u8g.drawStr(zugdatenSpalte, 21 + u8g.getFontAscent(), wagenstand[count]);
    u8g.setColorIndex(1);
  }

  static int zuglaufSpalte = 30;
  if (gleisSeite == GleisSeite_Links)
  {
    zuglaufSpalte = 15;
  }
  else if (gleisSeite == GleisSeite_Rechts)
  {
    zuglaufSpalte = 30;
  }

  //Verspätungsbox
  if (lauftext[count][20] != '\0')
  {
    u8g.setColorIndex(1);
    u8g.drawBox(zuglaufSpalte, 0, 78, 6);
    u8g.setColorIndex(0);
    u8g.setFont(u8g_font_4x6); // 4x6
    int remaining = strlen(lauftext[count]) - offset;
    if (remaining > (LAUFTEXT_L - 1))
    {
      remaining = LAUFTEXT_L - 1;
    }
    memcpy(ausgabe, &lauftext[count][offset], remaining);
    ausgabe[remaining] = '\0';
    u8g.drawStr(zuglaufSpalte - subset, 6, ausgabe);
    u8g.setColorIndex(1);
  }

  u8g.drawStr(zuglaufSpalte, 12, zuglauf1[count]);

  u8g.drawStr(zuglaufSpalte, 19, zuglauf2[count]);

  u8g.setFont(u8g_font_tpss);
  u8g.drawStr(zuglaufSpalte, 34 + u8g.getFontDescent(), ziel[count]);

  u8g.setFont(u8g_font_9x18B);
  if (gleisSeite == GleisSeite_Links)
  {
    u8g.drawStr(0, 13, gleis);
  }
  else if (gleisSeite == GleisSeite_Rechts)
  {
    u8g.drawStr(128 - (6 * strlen(gleis)), 13, gleis);
  }

  // Nächste Züge
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_4x6); // 4x6
  if (count + 1 < ANZAHL_ANZEIGE)
  {

    // Trennung
    u8g.drawLine(0, 33, 128, 33);
    u8g.drawLine(0, 34, 128, 34);

    //nächster Zug
    char tmpsv[36];
    snprintf(tmpsv, 36, " % s % s % s", uhrzeit[count + 1], zugnummer[count + 1], ziel[count + 1]);
    u8g.drawStr(0, 44 + u8g.getFontDescent(), tmpsv);
  }
  if (count + 2 < ANZAHL_ANZEIGE)
  {
    //übernächster Zug
    char tmpsv2[36];
    snprintf(tmpsv2, 36, " % s % s % s", uhrzeit[count + 2], zugnummer[count + 2], ziel[count + 2]);
    u8g.drawStr(0, 54 + u8g.getFontDescent(), tmpsv2);
  }
  if (count + 3 < ANZAHL_ANZEIGE)
  {
    //überübernächster Zug
    char tmpsv3[36];
    snprintf(tmpsv3, 36, " % s % s % s", uhrzeit[count + 3], zugnummer[count + 3], ziel[count + 3]);
    u8g.drawStr(0, 64 + u8g.getFontDescent(), tmpsv3);
  }
}

void initializeStrings()
{
  String("1").toCharArray(gleis, 5); // Gleisnummer eigeben
}

void setup()
{
  initializeStrings();

  u8g.begin();
  u8g.setContrast(150); // Kontrast eingeben
  u8g.setRot180();      // anzeige 180 grad rotieren.
}

void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis; // Speichern letzte zeit der anziege hat sich geändert
    count = (count + 1);
  }

  // Use a button to change the displayed message
  if (Button0.Pressed())
  {
    previousMillis = currentMillis; // Speichern letzte zeit der anziege hat sich geändert
    count = (count + 1);
  }

  if (count > (ANZAHL_ANZEIGE - 1))
  {
    count = 0;
  }

  else
  {
    u8g.firstPage();
    do
    {

      draw();

    } while (u8g.nextPage());

    subset += 1;
    if (subset > 3)
    {
      offset += 1;
      subset = 0;
    }

    if (offset > strlen(lauftext[count]))
    {
      offset = 0;
    }
  }
}
