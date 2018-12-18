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

char ssid[] = "Fabian WiFi";
char pass[] = "azilliza";
char deviceName[] = "ESP-Remote";
bool isConnected = false;
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);

int PlayingNow = 0;

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
  if(Serial.available()>0){
    int readByte = Serial.read();
    switch(readByte){
      case 'q':
          PlayPause(PlayingNow);
        break;
      case 'w':
          AppleMIDI.sendControlChange(32,255,11);
        break;
      default:
        break;
      }
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

void OnAppleMidiConnected(uint32_t ssrc, char* name) {
  isConnected  = true;
  Serial.print(F("Connected to session "));
  Serial.println(name);
}

void OnAppleMidiDisconnected(uint32_t ssrc) {
  isConnected  = false;
  Serial.println(F("Disconnected"));
}
