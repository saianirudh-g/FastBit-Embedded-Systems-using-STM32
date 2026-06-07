#include "kalman_filter.h"

void kalman_filter_init(KalmanFilter *kf)
{
    kf->Q_angle   = 0.001f;
    kf->Q_bias    = 0.003f;
    kf->R_measure = 0.03f;

    kf->angle = 0.0f;
    kf->bias  = 0.0f;
    kf->rate  = 0.0f;

    kf->P[0][0] = 0.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 0.0f;
}

float kalman_filter_get_angle(KalmanFilter *kf, float newAngle, float dt)
{
    float rate = newAngle - kf->bias;
    kf->angle += dt * rate;

    kf->P[0][0] += dt * (kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_bias * dt;

    float S = kf->P[0][0] + kf->R_measure;
    float K0 = kf->P[0][0] / S;
    float K1 = kf->P[1][0] / S;

    float y = newAngle - kf->angle;

    kf->angle += K0 * y;
    kf->bias  += K1 * y;

    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];

    kf->P[0][0] -= K0 * P00_temp;
    kf->P[0][1] -= K0 * P01_temp;
    kf->P[1][0] -= K1 * P00_temp;
    kf->P[1][1] -= K1 * P01_temp;

    return kf->angle;
}
