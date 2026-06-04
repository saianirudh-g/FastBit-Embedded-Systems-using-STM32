/*
 * kalman_filter.c
 *
 *  Created on: 04-Jun-2026
 *      Author: saian
 */

#include <stdio.h>
#include <math.h>
#include "kalman_filter.h"

void kalman_filter_init(KalmanFilter* kf)
{
    kf->q_angle = 0.001f;
    kf->q_bias = 0.003f;
    kf->r_measure = 0.03f;

    kf->angle = 0.0f;
    kf->bias = 0.0f;

    kf->p[0][0] = 0.0f;
    kf->p[0][1] = 0.0f;
    kf->p[1][0] = 0.0f;
    kf->p[1][1] = 0.0f;
}

float kalman_filter_get_angle(KalmanFilter* kf, float new_angle, float dt)
{
    /* ---------- PREDICT ---------- */

    // No gyro input in your version, so angle stays same
    kf->angle += 0.0f;

    // Update covariance matrix
    kf->p[0][0] += dt * (kf->p[1][1] - kf->p[0][1] - kf->p[1][0] + kf->q_angle);
    kf->p[0][1] -= dt * kf->p[1][1];
    kf->p[1][0] -= dt * kf->p[1][1];
    kf->p[1][1] += kf->q_bias * dt;

    /* ---------- UPDATE ---------- */

    float s = kf->p[0][0] + kf->r_measure;

    float k[2];
    k[0] = kf->p[0][0] / s;
    k[1] = kf->p[1][0] / s;

    float y = new_angle - kf->angle;

    kf->angle += k[0] * y;
    kf->bias  += k[1] * y;

    float p00_temp = kf->p[0][0];
    float p01_temp = kf->p[0][1];

    kf->p[0][0] -= k[0] * p00_temp;
    kf->p[0][1] -= k[0] * p01_temp;
    kf->p[1][0] -= k[1] * p00_temp;
    kf->p[1][1] -= k[1] * p01_temp;

    return kf->angle;
}
