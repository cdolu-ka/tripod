#include <Arduino.h>

// put function declarations here:

#include <Arduino.h>

#define DIR_M1 2
#define PWM_M1 9
#define ENABLE_MOTORS 8
#define SS_M1 A3

void setup() {
    // Disable motors initially
    pinMode(ENABLE_MOTORS, OUTPUT);
    digitalWrite(ENABLE_MOTORS, HIGH);

    // Set up motor direction
    pinMode(DIR_M1, OUTPUT);
    digitalWrite(DIR_M1, LOW);

    // Set up motor speed
    pinMode(PWM_M1, OUTPUT);
    analogWrite(PWM_M1, 0);

    // Keep chip deselected
    pinMode(SS_M1, OUTPUT);
    digitalWrite(SS_M1, HIGH);

    // Enable motors
    digitalWrite(ENABLE_MOTORS, LOW);
}

void loop() {
    // Extend actuator
    digitalWrite(DIR_M1, HIGH);
    analogWrite(PWM_M1, 255);
    delay(1000);

    // Stop actuator
    analogWrite(PWM_M1, 0);
    delay(2000);

    // Retract actuator
    digitalWrite(DIR_M1, LOW);
    analogWrite(PWM_M1, 255);
    delay(1000);

    // Stop actuator
    analogWrite(PWM_M1, 0);
    delay(2000);
}