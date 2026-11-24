
#include "imuFunction.h"

#include <Wire.h>
#include <math.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu;

// dmp

static bool DMPReady = false;

static uint8_t devStatus;
static uint16_t packetSize;
static uint8_t FIFOBuffer[64];
static Quaternion q;
static VectorFloat gravity;

static float ypr[3];

float yawInDegrees = 0.0;
float pitchInDegrees = 0.0;
float rollInDegrees = 0.0;

// imu init

bool imuBegin() {

    Wire.begin();
    Wire.setClock(400000);
    mpu.initialize();

    if (!mpu.testConnection()) {

        Serial.println("MPU6050 connection fail");

        DMPReady = false;

        return false;
    }

    Serial.println("MPU6050 connection");
    devStatus = mpu.dmpInitialize();

    // calibration settings from src/MPU6050_True_Zero_Nano.cpp file

    mpu.setXGyroOffset(65);
    mpu.setYGyroOffset(-5);
    mpu.setZGyroOffset(41);
    mpu.setXAccelOffset(-1951);
    mpu.setYAccelOffset(-917);
    mpu.setZAccelOffset(1329);

    // dmp on

    if (devStatus == 0) {

        mpu.setDMPEnabled(true);
        packetSize = mpu.dmpGetFIFOPacketSize();
        DMPReady = true;
        Serial.println("ready");
        return true;
    }

    Serial.print("init fail");
    Serial.println(devStatus);
    DMPReady = false;
    return false;
}

// imu readings

bool imuUpdate() {
    if (!DMPReady) return false;
    if (!mpu.dmpGetCurrentFIFOPacket(FIFOBuffer)) {
        return false;
    }
    mpu.dmpGetQuaternion(&q, FIFOBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    yawInDegrees = ypr[0] * 180.0 / M_PI;
    pitchInDegrees = ypr[1] * 180.0 / M_PI;
    rollInDegrees = ypr[2] * 180.0 / M_PI;

    return true;
}

bool imuAvailable() {

    return DMPReady;
}
