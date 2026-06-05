//ARAM Toggle Actuator     Example Code                rev: 6/5/2026

/*This example shows the use of four seperate toggle actuators, in the current setup these actuators are on pins 0-3, however you may change this*/

//import ARAM Toggle Actuator Library
#include "ARAM_Toggle_Actuator.h";

//define what pins we're gonna use
#define CNTRL_PIN_A 1     //change me to your desired cntrl pin A
#define CNTRL_PIN_B 2     //change me to your desired cntrl pin B
#define CNTRL_PIN_C 0     //change me to your desired cntrl pin C
#define CNTRL_PIN_D 3     //change me to your desired cntrl pin D

//declare 4 toggle actuators
ARAM_TOGGLE_ACTUATOR a1;
ARAM_TOGGLE_ACTUATOR a2;
ARAM_TOGGLE_ACTUATOR a3;
ARAM_TOGGLE_ACTUATOR a4;

uint32_t mainClock;
uint32_t lastSwitch;

int delayAmount = 800; 


void setup() {

  //explicily set pins to output mode <- may not be nessesary
  gpio_reset_pin(GPIO_NUM_0);
  gpio_reset_pin(GPIO_NUM_3);
  gpio_reset_pin(GPIO_NUM_4);
  gpio_reset_pin(GPIO_NUM_5);
  pinMode(0, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  //initialize 4 toggle actuators
  a1  = ARAM_TOGGLE_ACTUATOR(CNTRL_PIN_A);
  a2  = ARAM_TOGGLE_ACTUATOR(CNTRL_PIN_B);
  a3  = ARAM_TOGGLE_ACTUATOR(CNTRL_PIN_C);
  a4  = ARAM_TOGGLE_ACTUATOR(CNTRL_PIN_D);

  //this var is used for the timer later
  lastSwitch = micros();
}

void loop() {

    //this basic code is a timer which activates every 800ms (see delayAmount var for timing)
  mainClock = micros();
  if(lastSwitch + delayAmount < mainClock){
    lastSwitch = micros() + delayAmount;

    //lets play every actuator
    a1.strike();
    a2.strike();
    a3.strike();
    a4.strike();

    //wait 75 ms
    delay(75);

    //retract all actuators
    a1.retract();
    a2.retract();
    a3.retract();
    a4.retract();

  }

}
