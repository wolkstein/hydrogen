char pass[3] = {0,119};
bool passed[3] = {false,false};
unsigned long messagecounter = 0;
unsigned long oldmessagecounter = 0;


char n_pass[3] = {1,118};
bool n_passed[3] = {false,false};
unsigned long n_messagecounter = 0;
unsigned long n_oldmessagecounter = 0;

int info_for_next_settings_value = 1000;

//################# init
void initMidiReceiving(){
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandlePitchChange(OnPitchBend);
} 


//################ implementation

void OnControlChange(byte channel, byte control, byte value){
  //Serial.printf("Receive Midi CC: %d | VAL: %d | CHANNEL: %d \n",control, value, channel );
}

void OnNoteOn(byte channel, byte note, byte velocity){
<<<<<<< HEAD
<<<<<<< HEAD
  //Serial.printf("Receive Midi Note On, NOTE: %d | VEL: %d\n",note, velocity );
=======
  Serial.printf("Receive Midi Note On, NOTE: %d | VEL: %d\n",note, velocity );
>>>>>>> d7fa066... add rpi keyevents
=======
  Serial.printf("Receive Midi Note On, NOTE: %d | VEL: %d\n",note, velocity );
>>>>>>> d7fa066... add rpi keyevents

  
}
  
void OnNoteOff(byte channel, byte note, byte velocity){
<<<<<<< HEAD
<<<<<<< HEAD
  //Serial.printf("Receive Midi Note Off, NOTE: %d | VEL: %d\n",note, velocity );

  // midi settings question
  messagecounter++;
  n_messagecounter++;
  
=======
  Serial.printf("Receive Midi Note Off, NOTE: %d | VEL: %d\n",note, velocity );
  messagecounter++;
>>>>>>> d7fa066... add rpi keyevents
=======
  Serial.printf("Receive Midi Note Off, NOTE: %d | VEL: %d\n",note, velocity );
  messagecounter++;
>>>>>>> d7fa066... add rpi keyevents
  if( note == pass[0] && velocity == 0){
    oldmessagecounter = messagecounter;
    passed[0] = true;
  }
  if(passed[0] && note == pass[1] && velocity ==0 && oldmessagecounter + 1 == messagecounter){
    passed[1] = true;
  }

  if( passed[0] && passed[1]){
<<<<<<< HEAD
<<<<<<< HEAD
    //Serial.println("send midi settings");
    for(int i = 0 ; i<8;i++){
      usbMIDI.sendPitchBend( settings.padthreshold[i], settings.midichannel);
      usbMIDI.sendPitchBend( settings.piezoSensing[i], settings.midichannel);
      usbMIDI.sendPitchBend( settings.controlerloockup[i], settings.midichannel);
    }

    //midichannel
    usbMIDI.sendPitchBend( settings.midichannel, settings.midichannel);
    // padretriggerwait
    usbMIDI.sendPitchBend( settings.padRetriggerWait, settings.midichannel);
    // endmessage
    usbMIDI.sendPitchBend(8190, settings.midichannel);// endmessage 8190    
    passed[0] = false;
    passed[1] = false;
  }

  // midi new value
  if( note == n_pass[0] && velocity == 0){
    n_oldmessagecounter = n_messagecounter;
    n_passed[0] = true;
  }
  if(n_passed[0] && note == n_pass[1] && n_oldmessagecounter + 1 == n_messagecounter){
    n_passed[1] = true;
    info_for_next_settings_value = velocity;
  }


}

void OnPitchBend(byte channel, int pitch){
  if( n_passed[0] && n_passed[1]){
    //Serial.printf("pad position %d, set value to %d\n",info_for_next_settings_value,pitch);  

    //pad 1
    if(info_for_next_settings_value == 1) settings.padthreshold[0] = pitch;
    if(info_for_next_settings_value == 2) settings.piezoSensing[0] = pitch;
    if(info_for_next_settings_value == 3) settings.controlerloockup[0] = pitch;
    //pad 2
    if(info_for_next_settings_value == 4) settings.padthreshold[1] = pitch;
    if(info_for_next_settings_value == 5) settings.piezoSensing[1] = pitch;
    if(info_for_next_settings_value == 6) settings.controlerloockup[1] = pitch;
     //pad 3
    if(info_for_next_settings_value == 7) settings.padthreshold[2] = pitch;
    if(info_for_next_settings_value == 8) settings.piezoSensing[2] = pitch;
    if(info_for_next_settings_value == 9) settings.controlerloockup[2] = pitch;
    //pad 4
    if(info_for_next_settings_value == 10) settings.padthreshold[3] = pitch;
    if(info_for_next_settings_value == 11) settings.piezoSensing[3] = pitch;
    if(info_for_next_settings_value == 12) settings.controlerloockup[3] = pitch;

    
    //pad 5
    if(info_for_next_settings_value == 13) settings.padthreshold[4] = pitch;
    if(info_for_next_settings_value == 14) settings.piezoSensing[4] = pitch;
    if(info_for_next_settings_value == 15) settings.controlerloockup[4] = pitch;
    //pad 6
    if(info_for_next_settings_value == 16) settings.padthreshold[5] = pitch;
    if(info_for_next_settings_value == 17) settings.piezoSensing[5] = pitch;
    if(info_for_next_settings_value == 18) settings.controlerloockup[5] = pitch;
     //pad 7
    if(info_for_next_settings_value == 19) settings.padthreshold[6] = pitch;
    if(info_for_next_settings_value == 20) settings.piezoSensing[6] = pitch;
    if(info_for_next_settings_value == 21) settings.controlerloockup[6] = pitch;
    //pad 8
    if(info_for_next_settings_value == 22) settings.padthreshold[7] = pitch;
    if(info_for_next_settings_value == 23) settings.piezoSensing[7] = pitch;
    if(info_for_next_settings_value == 24) settings.controlerloockup[7] = pitch;

    if(info_for_next_settings_value == 25) settings.padRetriggerWait = pitch;

    if(info_for_next_settings_value == 26) saveConfig();
    
    n_passed[0] = false;
    n_passed[1] = false;
    info_for_next_settings_value = 1000;
  }   
=======
    Serial.println("send midi settings");
    usbMIDI.sendAfterTouchPoly(1, 36, 1);
    passed[0] = false;
    passed[1] = false;
  }
>>>>>>> d7fa066... add rpi keyevents
=======
    Serial.println("send midi settings");
    usbMIDI.sendAfterTouchPoly(1, 36, 1);
    passed[0] = false;
    passed[1] = false;
  }
>>>>>>> d7fa066... add rpi keyevents
}
