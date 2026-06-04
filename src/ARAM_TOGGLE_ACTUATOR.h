//ARAM_TOGGLE_ACTUATOR Library      -  Jack Serlin                rev: 6/3/2026               A.R.A.M. - American Robotics Assisted Manufacturing

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
    ARAM_TOGGLE_ACTUATOR(int pin, double frequency){
        
        pinMode(pin,OUTPUT);
        fastWrite(pin,LOW);

        //store pin data into struct
        this->pin = pin;
        this->freq = frequency;
        this->curState = 0;
        this->timeClock = micros();
        this->nextSwitch = 0;
        this->isSilent = 0;
        this->doBeat = 0;
        for(int i = 0; i < MAX_SCHEDULED_NOTES; i++){
          this->scheduledNotes[i] = 0;
        }
    }

    //define with pin for generic use
    ARAM_TOGGLE_ACTUATOR(int pin){
        
        pinMode(pin,OUTPUT);
        fastWrite(pin,LOW);

        //store pin data into struct
        this->pin = pin;
        this->freq = 0;
        this->curState = 0;
        this->timeClock = micros();
        this->nextSwitch = 0;
        this->isSilent = 0;
        this->doBeat = 0;
        for(int i = 0; i < MAX_SCHEDULED_NOTES; i++){
          this->scheduledNotes[i] = 0;
        }
    }
    void strike();
    void retract();
    void update();
    void schedule(int time);
    void generateBeat(int strikeLength, int timing);
    void stopBeat();
    double getFrequency();
    void clearSchedule();



    private:
    int pin;
    int curState;
    uint32_t timeClock;
    uint32_t nextSwitch;
    uint32_t scheduledNotes [MAX_SCHEDULED_NOTES];
    int isSilent;
    int doBeat;
    int beatStikeLength;
    int beatTiming;
    double freq;


    void _updateStepperPinOutputs();
    void _cycleNotes();

};

//clear the actuators scheduled actuations
void ARAM_TOGGLE_ACTUATOR::clearSchedule(){
  this->scheduledNotes[0] = 0;
}

//updates pin outputs 
void ARAM_TOGGLE_ACTUATOR::_updateStepperPinOutputs(){

  //update pin states
  if(this->curState)fastWrite(this->pin,HIGH);
  else fastWrite(this->pin,LOW);
}

//Causes the actuator to strike
void ARAM_TOGGLE_ACTUATOR::strike(){
  //update pin outputs
  this->curState = 1;
  _updateStepperPinOutputs();
}

//returns defined frequency for actuator
double ARAM_TOGGLE_ACTUATOR::getFrequency(){
  return this->freq;
}

//Causes Actuator to retract
void ARAM_TOGGLE_ACTUATOR::retract(){
  //update pin outputs
  this->curState = 0;
  _updateStepperPinOutputs();
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

//cyles scheduled actions to next
void ARAM_TOGGLE_ACTUATOR::_cycleNotes(){
  for(int i = 0; i < MAX_SCHEDULED_NOTES-1; i++){
    this->scheduledNotes[i] = this->scheduledNotes[i+1];
  }
  this->scheduledNotes[MAX_SCHEDULED_NOTES-1] = 0;
  if(!this->scheduledNotes[0] == 0){
    nextSwitch =  micros() + this->scheduledNotes[0];
  }else{
    nextSwitch = 0;
  }

}


//updates the actuators state, call this during loop to utilize scheduling
void ARAM_TOGGLE_ACTUATOR::update(){
  timeClock = micros();
  if(nextSwitch==0){
    if(this->scheduledNotes[0] == 0){
      if(!this->doBeat) return;
      this->scheduledNotes[0] = ((uint32_t)this->beatStikeLength)*MICROS_TO_MILIS;
      this->scheduledNotes[1] = ((uint32_t)this->beatTiming)*MICROS_TO_MILIS - ((uint32_t)this->beatStikeLength)*MICROS_TO_MILIS;
    } 
    nextSwitch = micros()+ this->scheduledNotes[0];
  }
  if(timeClock > nextSwitch){
    
    if(this->curState){
      this->retract();
    }
    else {
      this->strike();
    }
    _cycleNotes();
  }
}

//schedule siwth of state in X milliseconds
void ARAM_TOGGLE_ACTUATOR::schedule(int time){
  for(int i = 0; i < MAX_SCHEDULED_NOTES; i++){
    if(this->scheduledNotes[i]==0){
      this->scheduledNotes[i] = ((uint32_t)time)*MICROS_TO_MILIS;
      return;
    }
  }
}
