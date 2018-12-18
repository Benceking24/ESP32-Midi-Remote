#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "AppleMidi.h"

#define PIN_Btn_1 33
#define PIN_Btn_2 32
#define PIN_Btn_3 35
#define PIN_Btn_4 34
#define PIN_LED_Blt 4
#define PIN_LED_Com 0
#define PIN_LED_Oth 2

char ssid[] = "ssid";
char pass[] = "pwd";
char deviceName[] = "ESP-Remote";
unsigned long t0 = millis();
bool isConnected = false;
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);
unsigned long debounceDelay = 50;

int metronomeCount = 0;
int PlayingNow = 0;

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
   pinMode(PIN_LED_Blt, OUTPUT);
   pinMode(PIN_LED_Com, OUTPUT);
   pinMode(PIN_LED_Oth, OUTPUT);
  
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
    MidiCC(36,1);
  }
    if(ButtonDebounceRead(PIN_Btn_4, &Btn_4State, &Btn_4LastState, &Btn_4lastDebounceTime)==HIGH){
    MidiCC(37,1);
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
  Serial.println("Start");
  PlayingNow = 1;
}

void Continue(){
  Serial.println("Continue");
  PlayingNow = 1;
}

void Stop(){
  Serial.println("Stop");
  PlayingNow = 0;
}
