#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "AppleMidi.h"

#define PIN_Btn_1 33
#define PIN_Btn_2 32
#define PIN_Btn_3 35
#define PIN_Btn_4 34
#define PIN_DISPLAY_Data 16
#define PIN_DISPLAY_Clock 4
#define PIN_DISPLAY_Enable 0

char ssid[] = "ssid";
char pass[] = "pwd";
char deviceName[] = "ESP-Remote";
unsigned long t0 = millis();
bool isConnected = false;
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);
unsigned long debounceDelay = 50;

int metronomeCount = 0;
int PlayingNow = 0;

int MinLimit = 1;
int MaxLimit = 99;
int location = MinLimit;
int channel = 4;
int bpmCount = 0;
int Display_Output[3] = {0,0,0};

int Btn_1State;
int Btn_1LastState = LOW;
unsigned long Btn_1lastDebounceTime = 0;
int Btn_2State;
int Btn_2LastState = LOW;
unsigned long Btn_2lastDebounceTime = 0;
int Btn_3State;
int Btn_3LastState = LOW;
unsigned long Btn_3lastDebounceTime = 0;
int Btn_4State;
int Btn_4LastState = LOW;
unsigned long Btn_4lastDebounceTime = 0;



void setup() {
   Serial.begin(115200);
   pinMode(PIN_Btn_1, INPUT);
   pinMode(PIN_Btn_2, INPUT);
   pinMode(PIN_Btn_3, INPUT);
   pinMode(PIN_Btn_4, INPUT);
   pinMode(PIN_DISPLAY_Data, OUTPUT);
   pinMode(PIN_DISPLAY_Clock, OUTPUT);
   pinMode(PIN_DISPLAY_Enable, OUTPUT);
  
   Serial.print(F("Getting IP address..."));
   WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(F("."));
    }
    Serial.println(F(""));
    Serial.println(F("WiFi connected"));
    Serial.println();
    Serial.print(F("IP address is "));
    Serial.println(WiFi.localIP());
    
    AppleMIDI.begin(deviceName);
    AppleMIDI.OnConnected(OnAppleMidiConnected);
    AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);
    AppleMIDI.OnReceiveStart(Start);
    AppleMIDI.OnReceiveContinue(Continue);
    AppleMIDI.OnReceiveStop(Stop);
    AppleMIDI.OnReceiveClock(tick);

    DisplayOn();
    DisplayNumber(1);
}

void loop() {
  AppleMIDI.run();
  if(ButtonDebounceRead(PIN_Btn_1, &Btn_1State, &Btn_1LastState, &Btn_1lastDebounceTime)==HIGH){
    PlayPause(PlayingNow);
  }
  if(ButtonDebounceRead(PIN_Btn_2, &Btn_2State, &Btn_2LastState, &Btn_2lastDebounceTime)==HIGH){
    MidiCC(35,1);
  }
    if(ButtonDebounceRead(PIN_Btn_3, &Btn_3State, &Btn_3LastState, &Btn_3lastDebounceTime)==HIGH){
    decreaseBank(location, MinLimit, channel);
  }
    if(ButtonDebounceRead(PIN_Btn_4, &Btn_4State, &Btn_4LastState, &Btn_4lastDebounceTime)==HIGH){
    increaseBank(location, MaxLimit, channel);
  }
}

void tick(){
  if(bpmCount==23){
    FlipDp(0);
    FlipDp(0);
    bpmCount=0;
    }
    else{
      bpmCount++;
      }
}

void PlayPause(int IsItPlayingNow){
  if(IsItPlayingNow){
      AppleMIDI.sendControlChange(30,255,11);
  }
  else{
      AppleMIDI.sendControlChange(31,255,11); 
  }
}

void MidiCC(byte inControlNumber, byte inChannel){
        AppleMIDI.sendControlChange(inControlNumber,255,inChannel);
  }
  

int ButtonDebounceRead(int PinNum, int *SavedState, int *LastState, unsigned long *LastDebounce){
  int reading = digitalRead(PinNum);
  int state = LOW;
  if (reading != *LastState) {
    *LastDebounce = millis();
  }
  if ((millis() - *LastDebounce) > debounceDelay) {
    if (reading != *SavedState) {
      *SavedState = reading;
      if (*SavedState == HIGH) {
        state = !state;
      }
    }
  }
  *LastState = reading;
  return state;
}

void increaseBank(int actualLocation, int limitMax, int outputChannel){
    if(location!=limitMax){
        location++;
        DisplayNumber(location);
        AppleMIDI.sendControlChange(location,255,outputChannel);
    }
    else{
      AppleMIDI.sendControlChange(location,255,outputChannel);
      FlashScreen(2);
    }
}

void decreaseBank(int actualLocation, int limitMin, int outputChannel){
    if(location!=limitMin){
        location--;
        DisplayNumber(location);
        AppleMIDI.sendControlChange(location,255,outputChannel);
    }
    else{
      AppleMIDI.sendControlChange(location,255,outputChannel);
      FlashScreen(2);
    }
}

void OnAppleMidiConnected(uint32_t ssrc, char* name) {
  isConnected  = true;
  Serial.print(F("Connected to session "));
  Serial.println(name);
}

void OnAppleMidiDisconnected(uint32_t ssrc) {
  isConnected  = false;
  Serial.println(F("Disconnected"));
}


void Start(){
  PlayingNow = 1;
}

void Continue(){
  PlayingNow = 1;
}

void Stop(){
  PlayingNow = 0;
  bpmCount = 0;
}

void DisplayOn()
// turns on display
{
 digitalWrite(PIN_DISPLAY_Enable, LOW);
 shiftOut(PIN_DISPLAY_Data, PIN_DISPLAY_Clock, MSBFIRST, B00000001);
 digitalWrite(PIN_DISPLAY_Enable, HIGH);
 delay(10);
}

void DisplayOff()
{
 digitalWrite(PIN_DISPLAY_Enable, LOW);
 shiftOut(PIN_DISPLAY_Data, PIN_DISPLAY_Clock, MSBFIRST, B00000000);
 digitalWrite(PIN_DISPLAY_Enable, HIGH);
 delay(10);
}

void DisplayPrintOutput(){
  digitalWrite(PIN_DISPLAY_Enable, LOW);
  shiftOut(PIN_DISPLAY_Data, PIN_DISPLAY_Clock, MSBFIRST, Display_Output[0]); // D23~D16
  shiftOut(PIN_DISPLAY_Data, PIN_DISPLAY_Clock, MSBFIRST, Display_Output[1]); // D15~D8
  shiftOut(PIN_DISPLAY_Data, PIN_DISPLAY_Clock, MSBFIRST, Display_Output[2]); // D7~D0
  digitalWrite(PIN_DISPLAY_Enable, HIGH);
  delay(10);
  }

void DisplayNumber(int n){
  if(n>99){
    n=n%100;
  }
  int firstDigit = n%10;
  int secondDigit = n/10;
  int result = (secondDigit<<4)|firstDigit;
  Display_Output[0]=128;
  Display_Output[1]=0;
  Display_Output[2]=result;
  DisplayPrintOutput();
}

void FlipDp(int nthDP){
  int prefix=0; //9 = first digit, 10 = second digit
  if(nthDP==0){
    prefix = 9;
    }
  else if(nthDP==1){
    prefix = 10;
    }
  int Hi_Nibble=(Display_Output[0]>>4)&0x0F;
  int Lo_Nibble=Display_Output[0] & 0x0F;
  if(Hi_Nibble==prefix){ //Be van kapcsolva az első
      Display_Output[0]&=~(1<<4); //1001xxxx-ről 1000xxxx-re
    }
    else{                 //yyyyxxxx-ről 1001xxxx-re
      Display_Output[0] = (prefix << 4) | Lo_Nibble; //Összefűzi a két felet
    }
   DisplayPrintOutput();
}

void FlashScreen(int numTimes){
  int timeout = 50;
  for(int i=0;i<numTimes;i++){;
      DisplayOff();
      delay(timeout);
      DisplayOn();
      delay(timeout);
  }
}
