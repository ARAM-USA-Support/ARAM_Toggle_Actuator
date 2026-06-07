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

You can make an actuator vibrate at a given frequency
    actuator.vibrateAt( frequency );

You can the vibration with
    actuator.endMovement();


IMPORTANT: To use beat, vibrate, or schedule functionality you must do the following in loop function:

void loop() {
  actuator.update();
}

ADVANCED OPTIONS:

Stike Duration / Delay: 
    actuator.setStrikeDelay( X )
Allows you to detemrine the hit duration in milliseconds
    
Set Frequency: 
    actuator.setFrequency( X )
Sets standard actuator frequency, does not cause frequency to be played
    
Set Voltage: 
    actuator.setVoltage( X )
Tell the actuator what voltage it is running at, this is neccisary for avoiding noisy vibrations, correctly executing the strike delay, 
and a few other advanced timing events
    
Set Frequency Harmonic: 
    actuator.setFrequencyHarmonic( N )
(Only effects vibration, 0 < N <= 8)
This tells the actuator to try and excite the Nth harmonic instead of my target frequency, then apply damping to arrive at the target frequency
This is only useful in particular circumstances where you may not have a resonator that works well
    
Automatically adjust harmonics: 
    actuator.setHarmonicFrequencyCutoff( X )
(Only effects vibration)
Use this to automaticaly apply the harmonic excitation to all frequencies below X
Defualts to 1000
    




MODES:
    Actuator Mode:
        This detemines the general prupose for the actuator
        [ARAM_TOGGLE_ACTUATOR_Actuator] : General Purpose Actuator Mode, Call Strike, Vibrate, Retract as normal, actuator always ends in retract positon
        [ARAM_TOGGLE_ACTUATOR_Frequency_Generator] : Speaker Mode, designed for Vibration, actuator always ends in the strike position
    Vibration Mode:
        This detemines how the actuator handles vibrations, these mainly effect the actuator in standard actuator mode, since frequency generator mode will override these defualts.
        [ARAM_TOGGLE_ACTUATOR_Vibration_None] : Will Not Vibrate, may cause issues if this mode is selected for a an actuator in Frequency Generator Mode
        [ARAM_TOGGLE_ACTUATOR_Vibration_On] : While Note is On the actuator will make smaller hits, about 1/3 the length of the intial hit
        [ARAM_TOGGLE_ACTUATOR_Vibration_Full_Stroke] : While Note is On the actuator will continoully make full hits
        [ARAM_TOGGLE_ACTUATOR_Vibration_Pitch] : While Note is On Vibrate at the requested note frequency after intial hit.

Set modes like the following
    actuator.setActuatorMode( ARAM_TOGGLE_ACTUATOR_Actuator );
    actuator.setVibrationMode( ARAM_TOGGLE_ACTUATOR_Vibration_On );
    
