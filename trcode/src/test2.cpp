#include <Arduino.h>

// put function declarations here:

// Motor 1
const int DIR_M1 = 2;
const int PWM_M1 = 9;
const int SS_M1 = A3;

// Motor 2
const int DIR_M2 = 3;
const int PWM_M2 = 10;
const int SS_M2 = A2;

// Motor 3
const int DIR_M3 = 4;
const int PWM_M3 = 5;
const int SS_M3 = A1;

// Shared motor enable
const int ENABLE_MOTORS = 8;

// Movement parameters
const int motorSpeed = 255;
const int movementTime = 2000;

void setup() {
    Serial.begin(9600);

    // Disable all motors initially
    pinMode(ENABLE_MOTORS, OUTPUT);
    digitalWrite(ENABLE_MOTORS, HIGH);

    // Configure direction pins
    pinMode(DIR_M1, OUTPUT);
    pinMode(DIR_M2, OUTPUT);
    pinMode(DIR_M3, OUTPUT);

    // Configure PWM pins
    pinMode(PWM_M1, OUTPUT);
    pinMode(PWM_M2, OUTPUT);
    pinMode(PWM_M3, OUTPUT);

    // Configure chip-select pins
    pinMode(SS_M1, OUTPUT);
    pinMode(SS_M2, OUTPUT);
    pinMode(SS_M3, OUTPUT);

    // Deselect all chips
    digitalWrite(SS_M1, HIGH);
    digitalWrite(SS_M2, HIGH);
    digitalWrite(SS_M3, HIGH);

    // Keep unused Motor 4 inactive
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);
    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);

    // Initially stop all actuators
    analogWrite(PWM_M1, 0);
    analogWrite(PWM_M2, 0);
    analogWrite(PWM_M3, 0);

    // Enable all motors
    digitalWrite(ENABLE_MOTORS, LOW);
}

void loop() {

    // Extend actuator 1
    Serial.println("Extending actuator 1");
    digitalWrite(DIR_M1, HIGH);
    analogWrite(PWM_M1, motorSpeed);
    delay(movementTime);
    analogWrite(PWM_M1, 0);

    // Extend actuator 2
    Serial.println("Extending actuator 2");
    digitalWrite(DIR_M2, HIGH);
    analogWrite(PWM_M2, motorSpeed);
    delay(movementTime);
    analogWrite(PWM_M2, 0);

    // Extend actuator 3
    Serial.println("Extending actuator 3");
    digitalWrite(DIR_M3, HIGH);
    analogWrite(PWM_M3, motorSpeed);
    delay(movementTime);
    analogWrite(PWM_M3, 0);

    // Brief pause before reversing direction
    delay(500);

    // Set all actuators to retract
    Serial.println("Retracting all actuators");
    digitalWrite(DIR_M1, LOW);
    digitalWrite(DIR_M2, LOW);
    digitalWrite(DIR_M3, LOW);

    // Retract all three simultaneously
    analogWrite(PWM_M1, motorSpeed);
    analogWrite(PWM_M2, motorSpeed);
    analogWrite(PWM_M3, motorSpeed);
    delay(movementTime);

    // Stop all actuators
    analogWrite(PWM_M1, 0);
    analogWrite(PWM_M2, 0);
    analogWrite(PWM_M3, 0);

    Serial.println("All actuators stopped");

    // Wait 2 seconds before repeating
    delay(2000);
}