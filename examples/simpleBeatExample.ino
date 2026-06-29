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

  //declare each one as a standard actuator (as opposed to vibrator)
  a1.setActuatorMode(ARAM_TOGGLE_ACTUATOR_Actuator);
  a2.setActuatorMode(ARAM_TOGGLE_ACTUATOR_Actuator);
  a3.setActuatorMode(ARAM_TOGGLE_ACTUATOR_Actuator);
  a4.setActuatorMode(ARAM_TOGGLE_ACTUATOR_Actuator);

  //have each actuator generate a seperate beat
  a1.generateBeat(100, 500);
  a2.generateBeat(75, 750);
  a3.generateBeat(100, 1250);
  a4.generateBeat(75, 1750);
}

void loop() {
  //in the loop we must call update on each actuator in order to use the beat functionality
  a1.update();
  a2.update();
  a3.update();
  a4.update();
}
