
#define CNTRL_PIN_A 0     //change me to your desired cntrl pin
ARAM_TOGGLE_ACTUATOR a1;

//these vars are used to detrimine the the notes used, and how often we switch
double minFreq = 500;                             //minmimum frequecy of sweep
double maxFreq = 1000;                            //maximum sweep of frequency
int seconds = 30;                                //number of seconds elapsed
double rateOfFreqChangePerMilliSecond = (maxFreq-minFreq)/(seconds*1000);
uint32_t microsPerHtzChange = (((uint32_t)(30*seconds*1000*MICROS_TO_MILIS))/((maxFreq-minFreq)));
double curFreq = minFreq;               

//timer variables
uint32_t timerLength = seconds*1000*MICROS_TO_MILIS;
uint32_t timerEndVal = 0;
uint32_t switchTimer = 0;

void setup() {

  a1  = ARAM_TOGGLE_ACTUATOR(CNTRL_PIN_A,800);

  // Initialize the built-in BLEMidiServer object
  BLEMidiServer.begin("A.R.A.M. Metal Orchestra");

  // Register callbacks to the global object
  BLEMidiServer.setNoteOnCallback(handleNoteOn);
  BLEMidiServer.setNoteOffCallback(handleNoteOff);
  timerEndVal = micros() + timerLength;
  switchTimer = micros() + microsPerHtzChange;
  a1.vibrateAtFrequency(curFreq);

}


void loop() {
  uint32_t timeNow = micros();
  a1.update();
  if(switchTimer < timeNow){
    curFreq += 30;
    if(curFreq > maxFreq) curFreq = maxFreq;
    switchTimer = timeNow + microsPerHtzChange;//microsPerHtzChange;
    a1.setTone(curFreq);
    a1.schedule((seconds*1001*MICROS_TO_MILIS));
  }
}*/
