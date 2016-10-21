/*
  My attempt at SBrick
  Controlling Lego Powerfunctions over bluetooth
  Arduino Micro controller with a couple of chinese 2channel H Bridge driver boards

  The boards have in1, in2 for each channel that can be set as below
  in1     in2     MotorBehavior
  ----  -------  ---------------
  lo      lo      Standby
  hi      hi      Brake
  lo      hi/PWM  Forward
  hi/PWM  lo      Backward

  since I would need 2 PWM pins per channel for this config for a total of 8pins which
  Arduino mini doesn't have. I'll use the config below
  in1     in2     MotorBehavior
  ----  -------  ---------------
  lo      lo      Standby
  lo      PWM     Forward
  hi      ~PWM    Backward    (use 255-pwm value to turn the motor on in the OFF part of the PWM cycle)

  Remote controlled with android app developed with MIT App Inventor 2

*/

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <SPI.h>                             // Serial Peripheral Interface Library
#include <String.h>                          // contains function strtok: split string into tokens
#include <SoftwareSerial.h>

#define _DEBUG_
#ifdef _DEBUG_
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else /* _DEBUG_ */
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif /* _DEBUG_ */

class LegoControlPort2 {
  private:
    int8_t pin_in1;
    int8_t pin_in2;
    int8_t enabled;
    int8_t id;      //Mostly for debugging

    void init(void) {
      // Set the direction and PWM control pins to OUTPUT mode
      pinMode(pin_in1, OUTPUT);
      pinMode(pin_in2, OUTPUT);

      // Disable the Outputs
      digitalWrite(pin_in1, LOW);
      digitalWrite(pin_in2, LOW);
    }
  public:
    LegoControlPort2(uint8_t _id, uint8_t _pin_in1, uint8_t _pin_in2) {
      pin_in1 = _pin_in1;
      pin_in2 = _pin_in2;
      id      = _id;
      enabled = 0;
      init();
    }

    void enable(uint8_t _en) {
      int8_t en = !!(_en);
      if (enabled != en) {
        init();     // Initialize on state change
      }
      enabled = en;
      DEBUG_PRINT("P"); DEBUG_PRINT(id); DEBUG_PRINTLN(en ? " : Enabled" : " : Disabled");
    }

    void set_vector(int vector) {
      if (!enabled) {
        return;
      }

      if (vector < 0) {
        digitalWrite(pin_in1, HIGH);
        analogWrite(pin_in2, UINT8_MAX + vector);
        DEBUG_PRINT("P"); DEBUG_PRINT(id); DEBUG_PRINT(", Hi - "); DEBUG_PRINTLN(vector);
      } else {
        digitalWrite(pin_in1, LOW);
        analogWrite(pin_in2, vector);
        DEBUG_PRINT("P"); DEBUG_PRINT(id); DEBUG_PRINT(", Lo - "); DEBUG_PRINTLN(vector);
      }
    }
};

#define MAX_CON_PORTS 4
class LegoControl2 {
  private:
    static LegoControlPort2 con_ports[MAX_CON_PORTS];

  public:
    static void enable(uint8_t port, uint8_t en) {
      if (port < MAX_CON_PORTS) {
        con_ports[port].enable(en);
      }
    }

    static void set_vector(uint8_t port, int vector) {
      if (port < MAX_CON_PORTS) {
        con_ports[port].set_vector(vector);
      }
    }

};
LegoControlPort2 LegoControl2::con_ports[MAX_CON_PORTS] = {
  LegoControlPort2(0, 2, 3),
  LegoControlPort2(1, 8, 9),
  LegoControlPort2(2, 14, 10),
  LegoControlPort2(3, 12, 11)
};

/*================================================================================= */

SoftwareSerial mySerial(16, 17); // RX, TX

void setup() {
  Serial.begin(9600);                       // initialize serial communication
  Serial.setTimeout(1000);                  // 1000 ms time out

  mySerial.begin(9600);
  mySerial.setTimeout(1000);

  Serial.println("Initialization Done");
}

// Serial buffer size: calculate based on max input size expected for one command over bluetooth serial interface
#define INPUT_SIZE 30

void loop() {
  // Get next command from serial bluetooth (add 1 byte for final 0)
  char buff[INPUT_SIZE + 1];
  byte size;
  int arg;

  while ((size = mySerial.readBytesUntil('\n', buff, INPUT_SIZE))) {  //read Serial until new line or buffer full or time out
    // Valid commnads are of type "P<1byte port num><1byte OP code><arg1>\n"
    // P0E1    Enable Port0
    // P0E0    Disable Port0
    // P0S-255 Port0 speed -255

    buff[size] = 0; // Add the null termination
    if (*buff != 'P' || size < 3) {
      return;
    }

    int portnum = buff[1] - '0';
    arg = atoi(buff + 3);

    switch (buff[2]) {
      case 'E':
        LegoControl2::enable(portnum, arg);
        break;
      case 'S':
        LegoControl2::set_vector(portnum, arg);
        break;
      default:
        break;
    }
  }
}
