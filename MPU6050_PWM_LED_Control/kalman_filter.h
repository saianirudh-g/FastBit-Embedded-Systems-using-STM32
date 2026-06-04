/*
 * kalman_filter.h
 *
 *  Created on: 04-Jun-2026
 *      Author: saian
 */

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

#include <stdint.h>

/* Kalman filter structure */
typedef struct {
    float q_angle;      // Process noise variance for the angle
    float q_bias;       // Process noise variance for the gyro bias
    float r_measure;    // Measurement noise variance
    float angle;        // The calculated angle
    float bias;         // The calculated gyro bias
    float p[2][2];      // Error covariance matrix
} KalmanFilter;

/* Function prototypes */
void kalman_filter_init(KalmanFilter* kf);
float kalman_filter_get_angle(KalmanFilter* kf, float new_angle, float dt);

#endif /* KALMAN_FILTER_H */
