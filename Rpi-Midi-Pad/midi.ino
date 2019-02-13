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
  messagecounter++;
  Serial.printf("Receive Midi Note On, NOTE: %d | VEL: %d\n",note, velocity );
  if( note == pass[0]){
    oldmessagecounter = messagecounter;
    passed[0] = true;
  }
  if(passed[0] && oldmessagecounter + 1 == messagecounter){
    passed[1] = true;
  }

  if( passed[0] && passed[1]){
    Serial.println("send midi settings");
    passed[0] = false;
    passed[1] = false;
  }
  
}
  
void OnNoteOff(byte channel, byte note, byte velocity){
  //Serial.printf("Receive Midi Note Off, NOTE: %d | VEL: %d\n",note, velocity ); 
}
