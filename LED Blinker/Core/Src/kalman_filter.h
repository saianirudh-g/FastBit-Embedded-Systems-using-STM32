#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

#include <stdint.h>

typedef struct {
    float Q_angle;
    float Q_bias;
    float R_measure;
    float angle;
    float bias;
    float rate;      // <-- added to match kalman_filter.c
    float P[2][2];
} KalmanFilter;

void  kalman_filter_init(KalmanFilter *kf);
float kalman_filter_get_angle(KalmanFilter *kf, float newAngle, float dt);

#endif
