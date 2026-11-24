
#ifndef MPU6050_TRUE_ZERO_NANO_H
#define MPU6050_TRUE_ZERO_NANO_H

#include <Arduino.h>

// prev imu reads
extern float yawInDegrees;
extern float pitchInDegrees;
extern float rollInDegrees;

bool imuBegin();
bool imuUpdate();

// check imu on
bool imuAvailable();

#endif
