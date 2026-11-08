#pragma once
#include "globals.h"

    // imu_rotation.cpp - Automatische Rotation per Lagesensor (nur ESP32-S3)
    // imu_rotation.cpp - Automatic rotation via the motion sensor (ESP32-S3 only)

    void imuBegin() ;
    bool imuAvailable() ;
    int8_t imuReadQuadrant() ;
    uint8_t imuRotationFor(int8_t quadrant) ;
    bool imuCalibrate(uint8_t currentRotation) ;
    void handleAutoRotation() ;

#if HAS_IMU
    extern float imuLastG[3];
#endif
