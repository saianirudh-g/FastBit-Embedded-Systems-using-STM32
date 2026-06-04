/*
 * mpu6050.c
 *
 *  Created on: 03-Jun-2026
 *      Author: saian
 */

#include "mpu6050.h"
#include <stdio.h>

static uint8_t mpu6050_i2c_addr;
#define I2C_TIMEOUT 500UL

/* ---------------- Low-level I2C Helpers ---------------- */

mpu6050_status_t mpu6050_read_byte(I2C_HandleTypeDef *hi2c,
                                   uint8_t reg_addr,
                                   uint8_t *data)
{
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(hi2c, mpu6050_i2c_addr << 1, reg_addr, 1, data, 1, I2C_TIMEOUT);

    return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}

mpu6050_status_t mpu6050_read(I2C_HandleTypeDef *hi2c,
                              uint8_t reg_base_addr,
                              uint8_t *buffer,
                              uint32_t nbytes)
{
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(hi2c, mpu6050_i2c_addr << 1, reg_base_addr, 1, buffer, nbytes, I2C_TIMEOUT);

    return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}

mpu6050_status_t mpu6050_write_byte(I2C_HandleTypeDef *hi2c,
                                    uint8_t reg_addr,
                                    uint8_t data)
{
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Write(hi2c, mpu6050_i2c_addr << 1, reg_addr, 1, &data, 1, I2C_TIMEOUT);

    return (status == HAL_OK) ? MPU6050_OK : MPU6050_ERR;
}

/* ---------------- Initialization ---------------- */

mpu6050_status_t mpu6050_init(I2C_HandleTypeDef *hi2c, uint8_t i2c_dev_addr)
{
    mpu6050_i2c_addr = i2c_dev_addr;

    uint8_t read_byte = 0;

    if (mpu6050_read_byte(hi2c, MPU6050_REG_WHOAMI, &read_byte) != MPU6050_OK)
        return MPU6050_ERR;

    if (read_byte != 0x68 && read_byte != 0x98)
        return MPU6050_ERR;

    uint8_t data = 0x00;
    if (mpu6050_write_byte(hi2c, MPU6050_REG_PWRMGMT_1, data) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}

/* ---------------- Accelerometer Read ---------------- */

mpu6050_status_t mpu6050_read_accelerometer_data(I2C_HandleTypeDef *hi2c,
                                                 uint8_t i2c_dev_addr,
                                                 mpu6050_accel_data_t *accel_data)
{
    mpu6050_i2c_addr = i2c_dev_addr;

    uint8_t raw_data[6];

    if (mpu6050_read(hi2c, MPU6050_REG_ACCEL_START, raw_data, sizeof(raw_data)) != MPU6050_OK)
        return MPU6050_ERR;

    accel_data->x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
    accel_data->y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
    accel_data->z = (int16_t)((raw_data[4] << 8) | raw_data[5]);

    return MPU6050_OK;
}

mpu6050_accel_data_t mpu6050_read_accelerometer_calibration(
        const mpu6050_accel_data_t *error_offset,
        mpu6050_accel_data_t *raw_data)
{
    mpu6050_accel_data_t accel_calibrated;

    accel_calibrated.x = raw_data->x - error_offset->x;
    accel_calibrated.y = raw_data->y - error_offset->y;
    accel_calibrated.z = raw_data->z - error_offset->z;

    return accel_calibrated;
}

/* ---------------- Low-Pass Filter Config ---------------- */

mpu6050_status_t mpu6050_configure_low_pass_filter(I2C_HandleTypeDef *hi2c,
                                                   mpu6050_dlpf_config_t dlpf)
{
    uint8_t value = 0;

    if (mpu6050_read_byte(hi2c, MPU6050_REG_CONFIG, &value) != MPU6050_OK)
        return MPU6050_ERR;

    value &= ~0x07;
    value |= (uint8_t)dlpf;

    if (mpu6050_write_byte(hi2c, MPU6050_REG_CONFIG, value) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}
