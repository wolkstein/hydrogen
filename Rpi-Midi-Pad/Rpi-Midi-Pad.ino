/*
 * Rpi-Pad Midi implementation 
 * 
 * drum Pad 1 - 8 Midi Note 
 * Rotary 1 - 2 Midi CC
 * Switch 1 - 2 Midi CC
 * 
 */


// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// settings
#define CONFIG_VERSION "ls1"

struct StoreSettingsStruct {
  // The variables of your settings
  int piezoSensing;
  int padRetriggerWait;
  int controlerloockup[8];
  int padthreshold[8];
    
  // This is for mere detection if they are your settings
  char version_of_program[4]; // it is the last variable of the struct
  // so when settings are saved, they will only be validated if
  // they are stored completely.
} settings = {
  // The default settings
  200,                         // piezoSensing
  50,                          // padRetriggerWait
  {36,37,38,39,39,40,41,36},   // controlerloockup[8]
  {40,40,40,40,40,40,40,40},   // padthreshold[8];
  CONFIG_VERSION
};

int cVal[8] = {0,0,0,0,0,0,0,0};

int hoester_wert[8] = {0,0,0,0,0,0,0,0};

int padSamplingState[8] = {0,0,0,0,0,0,0,0}; // 0 = note off,2 = sampling, 1 = wait for retrigger

unsigned long noteOnTimecVal[8] = {0,0,0,0,0,0,0,0};

int sampleLenghtInMSeconds = 3; // we sample 3ms to catch highest velocity


void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  Serial.begin(56000); 
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

  //EEPROM
  loadConfig();
  // ###### MIDI Receiving Messages
  initMidiReceiving();
}


void loop() {
  usbMIDI.read();
  unsigned long thisLooptime = millis();
  //Serial.printf("loop %i\n",thisLooptime);
  //Serial.printf("padstate %i\n",padSamplingState[0]);

  for(int i = 0;i<8;i++){
    cVal[i] = analogRead(i);
    if(i<7) cVal[i] = 0; // nicht angeschlossene pads floaten rum
  }
  //Serial.println(cVal[7]);
  // find firdt trigger moment from pad
  for(int i = 0;i<8;i++){
    if(cVal[i] > settings.padthreshold[i] && padSamplingState[i] < 1){
      //Serial.printf("%i on pad %i, start Sampling\n",cVal[i],i);
      noteOnTimecVal[i] = thisLooptime;
      padSamplingState[i] = 2;     
    }
  }
  
  // sample active pads
  for(int i = 0;i<8;i++){
    if( padSamplingState[i] == 2 ){
      if( cVal[i] > hoester_wert[i] ) hoester_wert[i] = cVal[i];
      //Serial.printf("Do Sampling Sample pad %i, h_v = %i, s_v = %i \n",i,hoester_wert[i],cVal[i]);
      // checke ob sampletime am pad erreicht ist
      if(millis() >= (noteOnTimecVal[i] + sampleLenghtInMSeconds)){
        padSamplingState[i] = 1;
        //Serial.printf("End Sampling on Sample pad %i state 1 hw = %i\n",i,hoester_wert[i]);
        // nun können wir midi feuern
        if(hoester_wert[i] > settings.piezoSensing) hoester_wert[i] = settings.piezoSensing;
        usbMIDI.sendNoteOn( settings.controlerloockup[i], map(hoester_wert[i], settings.padthreshold[i], settings.piezoSensing, 1, 127), 10);
        hoester_wert[i] = 0;
        digitalWrite(11,HIGH);
        
      }
    }

  }

  //warte auf pad retrigger
   for(int i = 0;i<8;i++){
     if(padSamplingState[i] == 1 && thisLooptime >= noteOnTimecVal[i] + sampleLenghtInMSeconds + settings.padRetriggerWait){
      padSamplingState[i] = 0;
      digitalWrite(11,LOW);
      //Serial.printf("Retrigger end pad %i state 0\n",0);
      
    }    
   } 
}
