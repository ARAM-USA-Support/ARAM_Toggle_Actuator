//ARAM_TOGGLE_ACTUATOR Library      -  Jack Serlin                rev: 6/5/2026               A.R.A.M. - American Robotics Assisted Manufacturing

#if defined(ESP32)
  #include <soc/gpio_reg.h>
  #include <soc/io_mux_reg.h>
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #define IS_ARDUINO_ARCH
#else
  #define IS_GENERIC_ARCH
#endif

#define ARAM_SMA01P_

#define SET_RAW_PIN_OUT_HIGH(reg,a) reg |= (1<<a)
#define SET_RAW_PIN_OUT_LOW(reg,a) reg &= (!(1<<a))

#define MAX_SCHEDULED_NOTES 100
#define MICROS_TO_MILIS (1000)

#if defined(ESP32)
void fastWrite(uint8_t pin, uint8_t state){
  if((pin < 0)||pin>(48))return;
  if(pin <= 31){
    if(state==HIGH){
      *(volatile uint32_t*)(GPIO_OUT_W1TS_REG) |= 1<<pin;
    }else{
      *(volatile uint32_t*)(GPIO_OUT_W1TC_REG) |= 1<<pin;
    }
    return;
  }

  #if defined(GPIO_OUT1_W1TS_REG)
  if(state==HIGH){
    *(volatile uint32_t*)(GPIO_OUT1_W1TS_REG) |= 1<<(pin - 32);
  }else{
    *(volatile uint32_t*)(GPIO_OUT1_W1TC_REG) |= 1<<(pin - 32);
  }
  return;
  #endif
}
#else
void fastWrite(uint8_t pin, uint8_t state){
  digitalWrite(pin,state);
}
#endif

#define ARAM_TOGGLE_ACTUATOR_DEFAULT_VOLTAGE (4.5)
#define ARAM_TOGGLE_ACTUATOR_ASSUMED_FRICTION (0.05)
#define ARAM_TOGGLE_ACTUATOR_ASSUMED_FG (0.008*9.8)
#define ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_MASS (0.007)
#define ARAM_TOGGLE_ACTUATOR_ASSUMED_FORCE_PER_VOLT (0.22/5)
#define ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_TRAVEL (0.0045)
#define ARAM_TOGGLE_ACTUATOR_MIN_PULSE_INTERVAL (100)
#define ARAM_TOGGLE_ACTUATOR_DEFAULT_STRIKE_WAIT (20)
#define ARAM_TOGGLE_ACTUATOR_MAX_TOTAL_VIBRATION_TIME (2000)
#define ARAM_TOGGLE_ACTUATOR_HIT_TIME_OFF_FLAG (1)
#define ARAM_TOGGLE_ACTUATOR_VOLUME_DAMPING_CYCLES (10)
#define ARAM_TOGGLE_ACTUATOR_MAX_HARMONICS (8)

enum ARAM_TOGGLE_ACTUATOR_MODE {ARAM_TOGGLE_ACTUATOR_Actuator,ARAM_TOGGLE_ACTUATOR_Frequency_Generator};
enum ARAM_TOGGLE_ACTUATOR_VIBRATION_MODE {ARAM_TOGGLE_ACTUATOR_Vibration_None,ARAM_TOGGLE_ACTUATOR_Vibration_On,ARAM_TOGGLE_ACTUATOR_Vibration_Full_Stroke,ARAM_TOGGLE_ACTUATOR_Vibration_Pitch};


//ARAM toggle actuator object, represents a toggle actuator
class ARAM_TOGGLE_ACTUATOR{


    public:
    ARAM_TOGGLE_ACTUATOR(){

    }

    ARAM_TOGGLE_ACTUATOR(ARAM_TOGGLE_ACTUATOR &other){
        this->pin = other.pin;
        this->curState = other.curState;
    }

    //define with frequency for use in instrument
    ARAM_TOGGLE_ACTUATOR(int pin, double frequency = 0){
       
      pinMode(pin,OUTPUT);
      fastWrite(pin,LOW);

      //store pin data into struct
      this->pin = pin;
      setFrequency(frequency);
      this->curState = 0;
      this->timeClock = micros();
      this->nextSwitch = 0;
      this->isSilent = 0;
      this->doBeat = 0;

      vibrationMode = ARAM_TOGGLE_ACTUATOR_Vibration_On;
      actuatorMode = ARAM_TOGGLE_ACTUATOR_Frequency_Generator;
      
      setVoltage(ARAM_TOGGLE_ACTUATOR_DEFAULT_VOLTAGE);
      this->hitTarget = 0;
      nextScheduledSwitch = 0;
      nextHitTime = 0;
      currentlyStruck = 0;
      isPerpToGravity = 0;
      strikeVibrationDelay = ARAM_TOGGLE_ACTUATOR_DEFAULT_STRIKE_WAIT;
      doVolumeDamping = 0;
      volume = ARAM_TOGGLE_ACTUATOR_VOLUME_DAMPING_CYCLES;
      curVolumeCycle = 0;
      harmonicTarget = 1;
      frequencyAutoHarmonicCutoff = 1000;
      _updateVibrationTime();
    }

    void strike( int );
    void retract(int );
    void update();
    void schedule(int time);
    void generateBeat(int strikeLength, int timing);
    void stopBeat();
    double getFrequency();
    void clearSchedule();


    void vibrateAtFrequency(double frequency);
    void setVoltage(double);
    void setVibrationMode(ARAM_TOGGLE_ACTUATOR_VIBRATION_MODE vibrationMode);
    void setFrequency(double frequency);
    void setStrikeDelay(int delay);
    void setActuatorMode(ARAM_TOGGLE_ACTUATOR_MODE actuatorMode);
    void endMovement();
    void setVolumeDamping(int newValue);
    void setVolume(int volume);
    void setFrequencyHarmonic(int harmonic);
    void setHarmonicFrequencyCutoff(double frequencyCutoff);

    private:
    int pin;
    int curState;
    uint32_t timeClock;
    uint32_t nextSwitch;
    int nextScheduledSwitch;
    int isSilent;
    int doBeat;
    int beatStikeLength;
    int beatTiming;
    double freq;

    int hitTarget;
    int travelTimeUp;
    int travelTimeDown;
    double voltage;
    uint32_t nextHitTime;
    int currentlyStruck;

    uint32_t vibrationTimeUp;
    uint32_t vibrationTimeDown;
    int strikeVibrationDelay;
    ARAM_TOGGLE_ACTUATOR_VIBRATION_MODE vibrationMode;
    ARAM_TOGGLE_ACTUATOR_MODE actuatorMode;

    int doVolumeDamping;
    int volume;
    int curVolumeCycle;

    int harmonicTarget;
    double frequencyAutoHarmonicCutoff;

    int isPerpToGravity;


    void _updateStepperPinOutputs();
    void _updateVibrationTime();
    void _updateFrequencyVibrationTime(double frequency);

};

//Do volume damping option
void ARAM_TOGGLE_ACTUATOR::setVolumeDamping(int newValue){
  doVolumeDamping = newValue;
}

//used to set volume, only has effect if doVolumeDamping is on
void  ARAM_TOGGLE_ACTUATOR::setVolume(int volumeIn){
  if(volumeIn > ARAM_TOGGLE_ACTUATOR_VOLUME_DAMPING_CYCLES)volumeIn = ARAM_TOGGLE_ACTUATOR_VOLUME_DAMPING_CYCLES;
  if(volumeIn < 0) volumeIn = 0;
  volume = volumeIn;
}

//set current standard harmonic setting - 1 is default
void ARAM_TOGGLE_ACTUATOR::setFrequencyHarmonic(int harmonic){
  if(harmonic < 1) harmonic = 1;
  harmonicTarget = harmonic;
  _updateFrequencyVibrationTime(freq);
}

//used to establish a lowest frequency before utilizing harmonics instead
void ARAM_TOGGLE_ACTUATOR::setHarmonicFrequencyCutoff(double frequencyCutoff){
  if(frequencyCutoff < 0) frequencyCutoff = 0;
  frequencyAutoHarmonicCutoff = frequencyCutoff;
  _updateFrequencyVibrationTime(freq);
}

//used to update vibration params
void  ARAM_TOGGLE_ACTUATOR::_updateFrequencyVibrationTime(double frequency){
  if(frequency <=0) return;
  int oldHarmonicTarget = harmonicTarget;
  if(frequencyAutoHarmonicCutoff/2 > frequency){
    int temp = 2;
    double tempCutOff = frequencyAutoHarmonicCutoff/2;
    while(tempCutOff >  frequency){
      temp += 1;
      tempCutOff = frequencyAutoHarmonicCutoff/temp;
    }
    if(temp > ARAM_TOGGLE_ACTUATOR_MAX_HARMONICS) temp = ARAM_TOGGLE_ACTUATOR_MAX_HARMONICS;
    harmonicTarget = temp;
  }

  double ratio = (((double)travelTimeDown)/travelTimeUp)/1.5;
  uint32_t period = (uint32_t)((1000000.0/(frequency*harmonicTarget)));
  uint32_t pitchResonantPeriod = period/(2*ratio);//(int)(freq/100);//
  uint32_t maximumTime = ((uint32_t)(travelTimeUp/2))*1000;
  uint32_t minimumTime = ARAM_TOGGLE_ACTUATOR_MIN_PULSE_INTERVAL;
  if(pitchResonantPeriod > maximumTime){
    vibrationTimeUp = maximumTime;
  }
  else if(pitchResonantPeriod < minimumTime){
    vibrationTimeUp = minimumTime;
  }
  else{
    vibrationTimeUp = pitchResonantPeriod;
  }

  
  maximumTime = (travelTimeDown/2)*1000;
  pitchResonantPeriod = (uint32_t)((1000000.0/frequency)*ratio)/(2);

  if(pitchResonantPeriod > maximumTime){
    vibrationTimeDown = maximumTime;
  }
  else if(pitchResonantPeriod < minimumTime){
    vibrationTimeDown = minimumTime;
  }
  else{
    vibrationTimeDown = pitchResonantPeriod;
  }

  vibrationTimeUp += (period)*(harmonicTarget - 1);

  harmonicTarget = oldHarmonicTarget;

}

//end current movement
void ARAM_TOGGLE_ACTUATOR::endMovement(){
  if( ( actuatorMode == ARAM_TOGGLE_ACTUATOR_Frequency_Generator) ){
    this->strike(0);
    nextSwitch = 0;
    nextHitTime = ARAM_TOGGLE_ACTUATOR_HIT_TIME_OFF_FLAG;
    this->currentlyStruck = 0; 
    this->hitTarget = false; 
    nextScheduledSwitch = 0;
  }
  else if( actuatorMode == ARAM_TOGGLE_ACTUATOR_Actuator){
    this->retract(0);
    nextSwitch = 0;
    nextScheduledSwitch = 0;
  }
}

//used to update vibration params depending on actuator state
void ARAM_TOGGLE_ACTUATOR::_updateVibrationTime(){
  if(vibrationMode == ARAM_TOGGLE_ACTUATOR_Vibration_Pitch){
    _updateFrequencyVibrationTime(freq);
  }
  else if(vibrationMode == ARAM_TOGGLE_ACTUATOR_Vibration_On){
    vibrationTimeDown = travelTimeDown/2;
    vibrationTimeUp = travelTimeUp;
  }else if(vibrationMode == ARAM_TOGGLE_ACTUATOR_Vibration_Full_Stroke){
    vibrationTimeDown = travelTimeDown + strikeVibrationDelay;
    vibrationTimeUp = travelTimeUp + strikeVibrationDelay;
  }
}

//sets actuators frequency
void ARAM_TOGGLE_ACTUATOR::setFrequency(double frequency){
  freq = frequency;
  _updateVibrationTime();
}

//causes actuator to vibrate at a specific frequency
void ARAM_TOGGLE_ACTUATOR::vibrateAtFrequency(double frequency){
  if(nextSwitch==0){
    strike(0);
    nextHitTime = 0;
    nextSwitch = 0;
    currentlyStruck = 1;
    this->hitTarget = true;
  }
  vibrationMode = ARAM_TOGGLE_ACTUATOR_Vibration_Pitch;
  _updateFrequencyVibrationTime(frequency);
  schedule(ARAM_TOGGLE_ACTUATOR_MAX_TOTAL_VIBRATION_TIME);
}


//calualtes time of travel when gravity is opposed to plunger
int __calculateTravelTimeAgainstGravity(double volts){
  return 1000*sqrt(2*(ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_TRAVEL)*ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_MASS/(ARAM_TOGGLE_ACTUATOR_ASSUMED_FORCE_PER_VOLT*volts - ARAM_TOGGLE_ACTUATOR_ASSUMED_FG - ARAM_TOGGLE_ACTUATOR_ASSUMED_FRICTION));
}

//calulates time of travel when gravity is with plunger
int __calculateTravelTimeWithGravity(double volts){
  return 1000*sqrt(2*(ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_TRAVEL)*ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_MASS/(ARAM_TOGGLE_ACTUATOR_ASSUMED_FORCE_PER_VOLT*volts + ARAM_TOGGLE_ACTUATOR_ASSUMED_FG - ARAM_TOGGLE_ACTUATOR_ASSUMED_FRICTION));
}

//calculates time of travel when plunger is perpendicular to gravity
int __calculateTravelTimeNoGravity(double volts){
  return 1000*sqrt(2*(ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_TRAVEL)*ARAM_TOGGLE_ACTUATOR_ASSUMED_PLUNGER_MASS/(ARAM_TOGGLE_ACTUATOR_ASSUMED_FORCE_PER_VOLT*volts - ARAM_TOGGLE_ACTUATOR_ASSUMED_FRICTION));
}

//sets voltage for actuator - mostly effects travel time calulations
void ARAM_TOGGLE_ACTUATOR::setVoltage(double voltageIn){
  this->voltage = voltageIn;
  if(!isPerpToGravity){
    this->travelTimeUp = __calculateTravelTimeAgainstGravity(voltageIn);
    this->travelTimeDown = __calculateTravelTimeWithGravity(voltageIn);
  }else{

    this->travelTimeUp = __calculateTravelTimeNoGravity(voltageIn);
    this->travelTimeDown = this->travelTimeUp;
  }
  _updateVibrationTime();
}

//sets actuator vibration mode
void ARAM_TOGGLE_ACTUATOR::setVibrationMode(ARAM_TOGGLE_ACTUATOR_VIBRATION_MODE vibrationMode){
  this->vibrationMode = vibrationMode;
}

//set actuator mode
void ARAM_TOGGLE_ACTUATOR::setActuatorMode(ARAM_TOGGLE_ACTUATOR_MODE actuatorMode){
  this->actuatorMode = actuatorMode;
}

//sets strike delay (hit duration)
void ARAM_TOGGLE_ACTUATOR::setStrikeDelay(int delay){
  this->strikeVibrationDelay = delay;
}

//clear the actuators scheduled actuations
void ARAM_TOGGLE_ACTUATOR::clearSchedule(){
  nextScheduledSwitch = 0;
}

//updates pin outputs 
void ARAM_TOGGLE_ACTUATOR::_updateStepperPinOutputs(){

  //update pin states
  if(this->curState)fastWrite(this->pin,HIGH);
  else fastWrite(this->pin,LOW);
}

//Causes the actuator to strike
void ARAM_TOGGLE_ACTUATOR::strike(int skipTargetUpdate = 0){
  //update pin outputs
  this->curState = 1;
  _updateStepperPinOutputs();
  if(!skipTargetUpdate){
    this->hitTarget = 0;
    currentlyStruck = 0;
    nextHitTime = micros() +  ((uint32_t)this->travelTimeUp)*MICROS_TO_MILIS;
  }
}

//returns defined frequency for actuator
double ARAM_TOGGLE_ACTUATOR::getFrequency(){
  return this->freq;
}

//Causes Actuator to retract
void ARAM_TOGGLE_ACTUATOR::retract(int skipTargetUpdate = 0){
  //update pin outputs
  this->curState = 0;
  _updateStepperPinOutputs();
  if(!skipTargetUpdate){
    this->hitTarget = 0;
    currentlyStruck = 0;
    nextHitTime = micros() +  ((uint32_t)this->travelTimeDown)*MICROS_TO_MILIS;
  }
}
}

//stops automated beat generation
void ARAM_TOGGLE_ACTUATOR::stopBeat(){
  this->doBeat = 0;
}

//starts automated beat generation, strike length defines hit duration, timing defines interval between strikes
void ARAM_TOGGLE_ACTUATOR::generateBeat( int strikeLength, int timing ){
  this->doBeat = 1;
  this->beatStikeLength = strikeLength;
  this->beatTiming = timing;
}

//updates the actuators state, call this during loop to utilize scheduling
void ARAM_TOGGLE_ACTUATOR::update(){
  timeClock = micros();
  if(nextSwitch==0){
    if(this->nextScheduledSwitch == 0){
      if(!this->doBeat) return;
      if(!(this->curState)){
        this->nextScheduledSwitch = ((uint32_t)this->beatStikeLength)*MICROS_TO_MILIS;
      }
      else{
        this->nextScheduledSwitch = ((uint32_t)this->beatTiming)*MICROS_TO_MILIS - ((uint32_t)this->beatStikeLength)*MICROS_TO_MILIS;
      }
    } 
    nextSwitch = micros()+ this->nextScheduledSwitch;
  }
  if(timeClock > nextSwitch){
    endMovement();
  }else if((nextHitTime!=ARAM_TOGGLE_ACTUATOR_HIT_TIME_OFF_FLAG)&&(timeClock > nextHitTime)){
    if(!this->hitTarget){
      this->hitTarget = true;
      if(curState){
        currentlyStruck = 1;
      }
      if(curState&&(vibrationMode != ARAM_TOGGLE_ACTUATOR_Vibration_None)){
        nextHitTime = micros() +  ((uint32_t)strikeVibrationDelay)*MICROS_TO_MILIS;
      }
    }else if(nextHitTime!=ARAM_TOGGLE_ACTUATOR_HIT_TIME_OFF_FLAG){
      if(currentlyStruck && (vibrationMode != ARAM_TOGGLE_ACTUATOR_Vibration_None)){
        if(this->curState){
          curVolumeCycle ++;
          if((curVolumeCycle < volume)||(doVolumeDamping==0)){
            nextHitTime = micros() +  ((uint32_t)this->vibrationTimeDown);
            retract(1);
          }
          else{
            nextHitTime = micros() +  ((uint32_t)(vibrationTimeDown+vibrationTimeUp)*(ARAM_TOGGLE_ACTUATOR_VOLUME_DAMPING_CYCLES-volume));
            curVolumeCycle = 0;
          }
        }else{
          nextHitTime = micros() +  ((uint32_t)this->vibrationTimeUp);
          strike(1);
        }
      }
    }
  }
}

//schedule siwth of state in X milliseconds
void ARAM_TOGGLE_ACTUATOR::schedule(int time){
  this->nextScheduledSwitch = ((uint32_t)time)*MICROS_TO_MILIS;
}
