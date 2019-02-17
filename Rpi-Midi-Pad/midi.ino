char pass[3] = {0,119};
bool passed[3] = {false,false};
unsigned long messagecounter = 0;
unsigned long oldmessagecounter = 0;

//################# init
void initMidiReceiving(){
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleControlChange(OnControlChange);
} 


//################ implementation

void OnControlChange(byte channel, byte control, byte value){
  //Serial.printf("Receive Midi CC: %d | VAL: %d | CHANNEL: %d \n",control, value, channel );
}

void OnNoteOn(byte channel, byte note, byte velocity){
  Serial.printf("Receive Midi Note On, NOTE: %d | VEL: %d\n",note, velocity );

  
}
  
void OnNoteOff(byte channel, byte note, byte velocity){
  Serial.printf("Receive Midi Note Off, NOTE: %d | VEL: %d\n",note, velocity );
  messagecounter++;
  if( note == pass[0] && velocity == 0){
    oldmessagecounter = messagecounter;
    passed[0] = true;
  }
  if(passed[0] && note == pass[1] && velocity ==0 && oldmessagecounter + 1 == messagecounter){
    passed[1] = true;
  }

  if( passed[0] && passed[1]){
    Serial.println("send midi settings");
    usbMIDI.sendAfterTouchPoly(1, 36, 1);
    passed[0] = false;
    passed[1] = false;
  }
}
