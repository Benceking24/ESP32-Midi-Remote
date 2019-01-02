#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

byte patchNum = 0;
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  if (patchNum < 99) {
    patchNum++;
    MIDI.sendNoteOn(patchNum,127, 7);
    delay(200);
  }
  else{
    patchNum=0;
  }
}
