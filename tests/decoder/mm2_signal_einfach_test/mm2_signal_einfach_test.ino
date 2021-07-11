#include <MaerklinMotorola.h>
#include <Servo.h>

#define INPUT_PIN 2

Servo myservo;

int posServo;
int DelayServo=50;
int ServoMin=30;
int ServoMax=150;
int PortAddress;
int StatusRG;
int Counter;
int LedBlink;
int DelayLed=500;
int DelayCoil=1000;
int Aux_Output;
int Aux_Output_old;

volatile MaerklinMotorola mm(INPUT_PIN);
volatile bool hasNewData;


void DEBUG(String message) {
  Serial.println( String(millis()) + " [DEB]: " + message);
}

void SetPin() {
  if ((PortAddress==1)and(StatusRG==1)) {Aux_Output=7; Aux_Output_old=8;}
  if ((PortAddress==1)and(StatusRG==2)) {Aux_Output=8; Aux_Output_old=7;}
  if ((PortAddress==2)and(StatusRG==1)) {Aux_Output=7; Aux_Output_old=8;}
  if ((PortAddress==2)and(StatusRG==2)) {Aux_Output=8; Aux_Output_old=7;}
  if ((PortAddress==3)and(StatusRG==1)) {Aux_Output=7; Aux_Output_old=8;}
  if ((PortAddress==3)and(StatusRG==2)) {Aux_Output=8; Aux_Output_old=7;}
  if ((PortAddress==4)and(StatusRG==1)) {Aux_Output=9; Aux_Output_old=9;}
  if ((PortAddress==4)and(StatusRG==2)) {Aux_Output=9; Aux_Output_old=9;}
  if ((PortAddress==5)and(StatusRG==1)) {Aux_Output=10; Aux_Output_old=10;}
  if ((PortAddress==5)and(StatusRG==2)) {Aux_Output=10; Aux_Output_old=10;}
}


void TaskRed() {
  StatusRG = 1; SetPin();
  if (PortAddress==1) {ledBlink();}
  if (PortAddress==2) {ledOn();}
  if (PortAddress==3) {Coil();}
  if (PortAddress==4) {ServoSet();}
  if (PortAddress==5) {ServoSet();}
}


void TaskGreen() {
  StatusRG = 2; SetPin(); 
  if (PortAddress==1) {ledBlink();}
  if (PortAddress==2) {ledOn();}
  if (PortAddress==3) {Coil();}
  if (PortAddress==4) {ServoSet();}
  if (PortAddress==5) {ServoSet();}
}


void ledBlink() {
  if (StatusRG == 1) {LedBlink = 2;}
  if (StatusRG == 2) {LedBlink = 4;}
  digitalWrite(Aux_Output, HIGH);
  digitalWrite(Aux_Output_old, HIGH);
  delay(DelayLed);
  for (Counter=1;Counter<=LedBlink;Counter=Counter+1) {
  digitalWrite(Aux_Output, LOW);
  delay(DelayLed);
  digitalWrite(Aux_Output, HIGH);
  delay(DelayLed); 
  }
}

void Coil() {
  if (Aux_Output_old!=Aux_Output) {
  digitalWrite(Aux_Output_old, HIGH);
  digitalWrite(Aux_Output, HIGH);
  delay(DelayCoil);}
  digitalWrite(Aux_Output, LOW);
  delay(DelayCoil);
  digitalWrite(Aux_Output, HIGH);
  delay(DelayCoil); 
}

void ledOn() {
  if (Aux_Output_old!=Aux_Output) {
  digitalWrite(Aux_Output_old, HIGH);
  delay(DelayLed);}
  digitalWrite(Aux_Output, LOW);
  delay(DelayLed);
}



void ServoSet() {
  myservo.attach(Aux_Output);
  if (StatusRG == 1) {
    for (posServo >= ServoMin; posServo <= ServoMax; posServo += 1){
    myservo.write(posServo);
    delay(DelayServo);
  }}
  if (StatusRG == 2) {
    for (posServo <= ServoMax; posServo >= ServoMin; posServo -= 1){
    myservo.write(posServo);
    delay(DelayServo);
  }}
  myservo.detach();
}


void setup() {
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), isr, CHANGE);

  // use timer1 to blink onboard LED when there is data on the bus
  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);  
  TCCR1A = 0;
  TCCR1B = 0;
  bitSet(TCCR1B, CS12);  // 256 prescaler
  bitSet(TIMSK1, TOIE1); // timer overflow interrupt
  
  
  Serial.begin(115200);
  Serial.flush();
  Serial.println("Starting up");

}

void loop() {
  mm.Parse(); 
  MaerklinMotorolaData* Data = mm.GetData();
  if(Data) {
    // Check for aux decoder / Data->IsMagnet

    if(Data->IsMagnet && Data->MagnetState) {
      hasNewData = true;
      DEBUG("Decoder # " + String(Data->PortAddress) + " Switch " + (Data->DecoderState));
      PortAddress=String(Data->PortAddress).toInt(); {
          if (Data->DecoderState == MM2DecoderState_Green) {
            TaskGreen();
          } else if (Data->DecoderState == MM2DecoderState_Red) {
            TaskRed();
            
          }
        }
     }
  }
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
