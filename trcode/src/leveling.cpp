
#include <Arduino.h>
#include <math.h>

#include "imuFunction.h"

// motor pins

// Motor 1
const int DIR_M1 = 2;
const int PWM_M1 = 9;
const int SS_M1  = A3;

// Motor 2
const int DIR_M2 = 3;
const int PWM_M2 = 10;
const int SS_M2  = A2;

// Motor 3
const int DIR_M3 = 4;
const int PWM_M3 = 5;
const int SS_M3  = A1;

const int ENABLE_MOTORS = 8;

const int MOTOR_ENABLE_LEVEL = LOW;
const int MOTOR_DISABLE_LEVEL = HIGH;

// actuatrs config
const int MOTOR_SPEED = 255;

// time for each correction
const unsigned long MOTOR_PULSE_MS = 100;
const unsigned long SETTLE_TIME_MS = 250;

// leveling maşn

// max acceptable tilt -- this is the error threshold we'd change after demos
const float ANGLE_TOLERANCE = 1.0;
const float CORRECTION_DEADBAND = 0.25;

const int REQUIRED_LEVEL_READINGS = 10;

// geometry
// actuator 1 has to be placed in front of the +x axis thats on the imu board
// the acts2-3 go clockwise 

const float MOTOR_ANGLES[3] = {
    0.0,
    120.0,
    240.0
};

const bool EXTEND_IS_HIGH[3] = {
    true,
    true,
    true
};

// depends on imu calibration (which we haven't done :/)

const float PITCH_SIGN = 1.0;
const float ROLL_SIGN  = 1.0;

bool autoLevelEnabled = false;
bool motorsMoving = false;
bool waitingToSettle = false;

unsigned long movementStartTime = 0;
unsigned long settleStartTime = 0;

int consecutiveLevelReadings = 0;

// actuator pins

const int DIR_PINS[3] = {
    DIR_M1,
    DIR_M2,
    DIR_M3
};

const int PWM_PINS[3] = {
    PWM_M1,
    PWM_M2,
    PWM_M3
};

const int SS_PINS[3] = {
    SS_M1,
    SS_M2,
    SS_M3
};

// function decs

void initializeMotors();
void stopActuators();
void startMotor(int motor, bool extend);
void startCorrection();
void updateActuators();
void calculateLeveling();
void checkLevel();
void enableAutoLevel();
void disableAutoLevel();
void resetController();
void processSerial();

// motor init

void initializeMotors() {

    pinMode(ENABLE_MOTORS, OUTPUT);

    digitalWrite(
        ENABLE_MOTORS,
        MOTOR_DISABLE_LEVEL
    );

    for (int i = 0; i < 3; i++) {
        pinMode(DIR_PINS[i], OUTPUT);
        pinMode(PWM_PINS[i], OUTPUT);
        pinMode(SS_PINS[i], OUTPUT);
        digitalWrite(SS_PINS[i], HIGH);
    }

    // unused driver channels low
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
    pinMode(7, OUTPUT);
    digitalWrite(7, LOW);
    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);
    stopActuators();

    Serial.println("motor init done");
}


void stopActuators() {

    for (int i = 0; i < 3; i++) {
        analogWrite(PWM_PINS[i], 0);
    }

    motorsMoving = false;
}

// start

void startMotor(int motor, bool extend) {

    if (motor < 0 || motor > 2) return;

    bool direction;

    if (extend) {
        direction = EXTEND_IS_HIGH[motor];
    } else {
        direction = !EXTEND_IS_HIGH[motor];
    }

    digitalWrite(
        DIR_PINS[motor],
        direction ? HIGH : LOW
    );

    analogWrite(
        PWM_PINS[motor],
        MOTOR_SPEED
    );
}
void startCorrection() {
    motorsMoving = true;
    movementStartTime = millis();
}

void updateActuators() {

    if (!motorsMoving) return;

    if (
        millis() - movementStartTime >=
        MOTOR_PULSE_MS
    ) {

        stopActuators();
        waitingToSettle = true;
        settleStartTime = millis();
        Serial.println("Actuators stopped. Settling.");
    }
}

// leveling

void calculateLeveling() {

    if (motorsMoving || waitingToSettle) return;
    
    // somebody do the imu physical calibration fr
    float pitch = pitchInDegrees * PITCH_SIGN;
    float roll = rollInDegrees * ROLL_SIGN;

    if (
        fabsf(pitch) <= ANGLE_TOLERANCE &&
        fabsf(roll) <= ANGLE_TOLERANCE
    ) {
        checkLevel();
        return;
    }

    consecutiveLevelReadings = 0;
    bool anyMotorStarted = false;

    // calc correction -- someone double check this math
    for (int i = 0; i < 3; i++) {

        float theta =
            MOTOR_ANGLES[i] * PI / 180.0;

        float correction =
            -(pitch * cosf(theta) +
              roll * sinf(theta));
        if (
            fabsf(correction) <
            CORRECTION_DEADBAND
        ) {
            continue;
        }

        bool extend = correction > 0.0;

        startMotor(i, extend);
        anyMotorStarted = true;
        Serial.print("Motor ");
        Serial.print(i + 1);
        Serial.print(correction, 3);
        Serial.println(
            extend ? "entend" : "retract"
        );
    }

    if (anyMotorStarted) {
        startCorrection();
    } else {
        consecutiveLevelReadings = 0;
        Serial.println("no correction happening");
    }
}

void checkLevel() {
    consecutiveLevelReadings++;
    Serial.print("level: ");
    Serial.println(consecutiveLevelReadings);
    if (
        consecutiveLevelReadings >=
        REQUIRED_LEVEL_READINGS
    ) {

        Serial.println();
        Serial.println("DONE!!!! LEVEL!!!!!!");

        disableAutoLevel();
    }
}

void disableAutoLevel() {

    stopActuators();

    digitalWrite(
        ENABLE_MOTORS,
        MOTOR_DISABLE_LEVEL
    );

    autoLevelEnabled = false;
    waitingToSettle = false;
    consecutiveLevelReadings = 0;

    Serial.println("motors turning off now that leveling is done!");
}

// the actual autoleveling

void enableAutoLevel() {

    if (!imuAvailable()) {
        Serial.println("imu not up");
        return;
    }

    stopActuators();
    consecutiveLevelReadings = 0;
    waitingToSettle = false;
    autoLevelEnabled = true;
    digitalWrite(
        ENABLE_MOTORS,
        MOTOR_ENABLE_LEVEL
    );
    Serial.println();
    Serial.println("leveling start");
}

// reset

void resetController() {
    stopActuators();
    digitalWrite(
        ENABLE_MOTORS,
        MOTOR_DISABLE_LEVEL
    );
    autoLevelEnabled = false;
    waitingToSettle = false;
    consecutiveLevelReadings = 0;
    movementStartTime = 0;
    settleStartTime = 0;
    Serial.println();
    Serial.println("reset, all actuators stopped");
}

// we will replace this with the switch/button at the final prototype
// in serial monitor:
// g -> go
// z -> reset

void processSerial() {

    while (Serial.available() > 0) {
        char command = Serial.read();
        if (command == '\n' || command == '\r') {
            continue;
        }
        switch (command) {
            case 'g':
                enableAutoLevel();
                break;
            case 'z':
                resetController();
                break;
        }
    }
}

// setup

void setup() {

    Serial.begin(115200);
    initializeMotors();

    // imu
    if (!imuBegin()) {
        Serial.println("not starting");
        digitalWrite(
            ENABLE_MOTORS,
            MOTOR_DISABLE_LEVEL
        );
        while (true) {
            stopActuators();
        }
    }
    Serial.println("ready");
}

// loop

void loop() {

    processSerial();
    bool newReading = imuUpdate();
    updateActuators();

    if (!autoLevelEnabled) {
        return;
    }

    if (motorsMoving) {
        return;
    }

    if (waitingToSettle) {

        if (
            millis() - settleStartTime <
            SETTLE_TIME_MS
        ) {
            return;
        }

        waitingToSettle = false;
    }

    if (!newReading) {
        return;
    }
    calculateLeveling();
}
