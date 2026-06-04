/*
 * mpu6050.h
 *
 *  Created on: 03-Jun-2026
 *      Author: saian
 */

#ifndef SRC_MPU6050_H_
#define SRC_MPU6050_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

/* ---------------- Status Enum ---------------- */
typedef enum {
    MPU6050_OK,
    MPU6050_ERR
} mpu6050_status_t;

/* ---------------- Accelerometer Struct ---------------- */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} mpu6050_accel_data_t;

/* ---------------- DLPF CONFIG ENUM (THIS FIXES YOUR ERROR) ---------------- */
typedef enum {
    MPU6050_DLPF_260HZ = 0,
    MPU6050_DLPF_184HZ = 1,
    MPU6050_DLPF_94HZ  = 2,
    MPU6050_DLPF_44HZ  = 3,
    MPU6050_DLPF_21HZ  = 4,
    MPU6050_DLPF_10HZ  = 5,
    MPU6050_DLPF_5HZ   = 6
} mpu6050_dlpf_config_t;

/* ---------------- Function Prototypes ---------------- */
mpu6050_status_t mpu6050_init(I2C_HandleTypeDef *hi2c, uint8_t i2c_dev_addr);
mpu6050_status_t mpu6050_read_accelerometer_data(I2C_HandleTypeDef *hi2c,
                                                 uint8_t i2c_dev_addr,
                                                 mpu6050_accel_data_t *accel_data);

mpu6050_accel_data_t mpu6050_read_accelerometer_calibration(
        const mpu6050_accel_data_t *error_offset,
        mpu6050_accel_data_t *raw_data);

mpu6050_status_t mpu6050_configure_low_pass_filter(I2C_HandleTypeDef *hi2c,
                                                   mpu6050_dlpf_config_t dlpf);

/* ---------------- Register Map ---------------- */
#define MPU6050_REG_WHOAMI       0x75
#define MPU6050_REG_PWRMGMT_1    0x6B
#define MPU6050_REG_ACCEL_START  0x3B
#define MPU6050_REG_CONFIG       0x1A

#endif /* SRC_MPU6050_H_ */
