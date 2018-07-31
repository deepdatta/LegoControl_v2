# LegoControl v2

This is my attempt at making a variable speed motor controller for 4 Lego Technic PF Motors using the following parts.

1x Arduino Mini Pro  
2x Chinese L298N PWM 1.5A Dual Channel Motor Driver modules  
1x HC-05 Bluetooth Serial module

## Programming the motor controllers
The boards have in1, in2 for each channel that can be set as below  

| in1  | in2   | Motor Behavior |
|----- |-----  |----------------|
| lo   | lo    | Standby        |
| hi   | hi    | Brake          |
| lo   | hi/PWM| Forward        |
|hi/PWM|  lo   |   Backward     |

Since I would need 2 PWM pins per channel for this config for a total of 8pins which Arduino mini doesn't have.  
I'll use the config below

| in1  | in2   | Motor Behavior |
|----- |-----  |----------------| 
| lo   | lo    | Standby        |
| lo   | PWM   | Forward        |
| hi   | ~PWM  | Backward       | 

~PWM: Use 255-pwm value to turn the motor on in the OFF part of the PWM cycle.

## Control using a mobile App over bluetooth
The Arduino is controlled using a simple mobile app created with the [MIT App Inventor] (http://ai2.appinventor.mit.edu).  
It uses the following char based command protocol with each command terminated with a newline.  
command format: P "1byte port num" "1byte OP code" "arg"\n

|Commands  |Description   |
|----------|--------------|
|P0E1\n    |Enable Port0  |
|P0E0\n    |Disable Port0 |
|P0S100\n  |Port0 Fwd Speed 100|
|P1S-200\n |Port1 Rev Speed 200| 


## Circuit and Schematics

Wiring the output to 4 Lego PF connectors, the pinout is as below.
The output of the L298N will go to C1 and C2. The GND and +9v is required for the Servo, the M, L, XL motors don't need them.
![Lego PF pin info](https://github.com/deepdatta/LegoControl_v2/blob/master/wirelabels.jpg)

## Reference
https://scuttlebots.com/2014/03/02/lego-pf-hacking-wiring/
