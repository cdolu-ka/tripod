
#include <Arduino.h>

// acts
const int DIR_M1 = 2;
const int PWM_M1 = 9;
const int SS_M1 = A3;

const int DIR_M2 = 3;
const int PWM_M2 = 10;
const int SS_M2 = A2;

const int DIR_M3 = 4;
const int PWM_M3 = 5;
const int SS_M3 = A1;

const int ENABLE_MOTORS = 8;

const int motorSpeed = 255;
const int movementTime = 2000;

bool actuatorSelected = false;

// sflags
bool motor1Selected = false;
bool motor2Selected = false;
bool motor3Selected = false;

void stopActs();
void selectAct(char selection);
void controlAct(char command);
void reset();
void printSmenu();
void smenu();


void smenu() {

    Serial.println();
    Serial.println("select actuator:");
    Serial.println("  1 = act 1");
    Serial.println("  2 = act 2");
    Serial.println("  3 = act 3");
    Serial.println("  a = all acts");

    Serial.print("current: ");
}


void smenu() {

    Serial.println();
    Serial.println("actuators selected!");

    Serial.print("current: ");

    if (motor1Selected) Serial.print("1 ");
    if (motor2Selected) Serial.print("2 ");
    if (motor3Selected) Serial.print("3 ");

    Serial.println();
    Serial.println("  e = exrend");
    Serial.println("  p = pause");
    Serial.println("  r = retract");
    Serial.println("  z = change selection");

}

void selectAct(char selection) {

    // clear prevs
    motor1Selected = false;
    motor2Selected = false;
    motor3Selected = false;

    switch (selection) {

        case '1':
            motor1Selected = true;
            break;

        case '2':
            motor2Selected = true;
            break;

        case '3':
            motor3Selected = true;
            break;

        case 'a':
            motor1Selected = true;
            motor2Selected = true;
            motor3Selected = true;
            break;

    }

    actuatorSelected = true;

    smenu();
}

void controlAct(char command) {

    switch (command) {

        //extend
        case 'e':
            if (motor1Selected) {
                digitalWrite(DIR_M1, HIGH);
                analogWrite(PWM_M1, motorSpeed);
            }

            if (motor2Selected) {
                digitalWrite(DIR_M2, HIGH);
                analogWrite(PWM_M2, motorSpeed);
            }

            if (motor3Selected) {
                digitalWrite(DIR_M3, HIGH);
                analogWrite(PWM_M3, motorSpeed);
            }

            delay(movementTime);
            stopActs();
            break;

        // pasue
        case 'p':
            stopActs();
            break;

    
        // retract
        case 'r':
            if (motor1Selected) {
                digitalWrite(DIR_M1, LOW);
                analogWrite(PWM_M1, motorSpeed);
            }

            if (motor2Selected) {
                digitalWrite(DIR_M2, LOW);
                analogWrite(PWM_M2, motorSpeed);
            }

            if (motor3Selected) {
                digitalWrite(DIR_M3, LOW);
                analogWrite(PWM_M3, motorSpeed);
            }

            delay(movementTime);

            stopActs();
            break;


        default:
            Serial.println("no command.");
            break;
    }
}

void stopActs() {
    analogWrite(PWM_M1, 0);
    analogWrite(PWM_M2, 0);
    analogWrite(PWM_M3, 0);

}

// reset
void reset() {

    motor1Selected = false;
    motor2Selected = false;
    motor3Selected = false;

    actuatorSelected = false;

}


void setup() {

    Serial.begin(9600);

    // Shared motor enable
    pinMode(ENABLE_MOTORS, OUTPUT);
    digitalWrite(ENABLE_MOTORS, HIGH);

    // Direction pins
    pinMode(DIR_M1, OUTPUT);
    pinMode(DIR_M2, OUTPUT);
    pinMode(DIR_M3, OUTPUT);

    // PWM pins
    pinMode(PWM_M1, OUTPUT);
    pinMode(PWM_M2, OUTPUT);
    pinMode(PWM_M3, OUTPUT);

    // Chip-select pins
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

    // Stop all motors initially
    stopActs();

    // Enable motors
    digitalWrite(ENABLE_MOTORS, LOW);

    Serial.println("***");
    Serial.println("setup done");

    printSmenu();
}

// main

void loop() {

    if (Serial.available() > 0) {

        char c = Serial.read();

        // ignore n and carriage 
        if (c == '\n' || c == '\r') {
            return;
        }

        if (!actuatorSelected) {
            selectAct(c);

        }

        else {

            if (c == 'z') {
                // stop all motors before changing selection
                stopActs();

                reset();

                Serial.println();
                Serial.println("selection cleared");

                printSmenu();
            }

            else {
                controlAct(c);

            }

        }
    }
}

