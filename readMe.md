This library is for use with ARAM's line of toggle control actuators

Usage is fairly simple:


BASIC USAGE:

To declare a toggle actuator using pin A control
    ARAM_TOGGLE_ACTUATOR actuator;
    actuator = ARAM_TOGGLE_ACTUATOR( A );

To strike the actuator
    actuator.strike();

To retract the actuator();
    actuator.retract();


ADVANCED:

You may declare the actuator with a frequency
    ARAM_TOGGLE_ACTUATOR actuator;
    actuator = ARAM_TOGGLE_ACTUATOR( A , frequency);

You can start a beat with a strike duration of A and a beat timing of B
    actuator.generateBeat( A , B );

You can stop a beat
    actuator.stopBeat();

You can schedule a change of state in X milliseconds with
    actuator.schedule(X);

IMPORTANT: To use beat or schedule functionality you must do the following in loop function:

void loop() {
  instrument.update();
}