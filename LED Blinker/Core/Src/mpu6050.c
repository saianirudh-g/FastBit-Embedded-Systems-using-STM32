#include "mpu6050.h"

static mpu6050_status_t mpu6050_read_bytes(I2C_HandleTypeDef *hi2c,
                                           uint8_t addr,
                                           uint8_t reg,
                                           uint8_t *data,
                                           uint16_t len)
{
    if (HAL_I2C_Mem_Read(hi2c, addr << 1, reg, I2C_MEMADD_SIZE_8BIT,
                         data, len, HAL_MAX_DELAY) != HAL_OK)
        return MPU6050_ERR;
    return MPU6050_OK;
}

static mpu6050_status_t mpu6050_read_byte(I2C_HandleTypeDef *hi2c,
                                          uint8_t addr,
                                          uint8_t reg,
                                          uint8_t *data)
{
    return mpu6050_read_bytes(hi2c, addr, reg, data, 1);
}

static mpu6050_status_t mpu6050_write_byte(I2C_HandleTypeDef *hi2c,
                                           uint8_t addr,
                                           uint8_t reg,
                                           uint8_t value)
{
    if (HAL_I2C_Mem_Write(hi2c, addr << 1, reg, I2C_MEMADD_SIZE_8BIT,
                          &value, 1, HAL_MAX_DELAY) != HAL_OK)
        return MPU6050_ERR;
    return MPU6050_OK;
}

mpu6050_status_t mpu6050_init(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
    uint8_t whoami = 0;

    if (mpu6050_read_byte(hi2c, addr, MPU6050_REG_WHOAMI, &whoami) != MPU6050_OK)
        return MPU6050_ERR;

    if (whoami != 0x68)
        return MPU6050_ERR;

    if (mpu6050_write_byte(hi2c, addr, MPU6050_REG_PWR_MGMT_1, 0x00) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}

mpu6050_status_t mpu6050_read_accelerometer_data(I2C_HandleTypeDef *hi2c,
                                                 uint8_t addr,
                                                 mpu6050_accel_data_t *out)
{
    uint8_t buf[6];

    if (mpu6050_read_bytes(hi2c, addr, MPU6050_REG_ACCEL_START, buf, 6) != MPU6050_OK)
        return MPU6050_ERR;

    out->x = (int16_t)((buf[0] << 8) | buf[1]);
    out->y = (int16_t)((buf[2] << 8) | buf[3]);
    out->z = (int16_t)((buf[4] << 8) | buf[5]);

    return MPU6050_OK;
}

mpu6050_status_t mpu6050_configure_low_pass_filter(I2C_HandleTypeDef *hi2c,
                                                   uint8_t addr,
                                                   mpu6050_dlpf_config_t cfg)
{
    uint8_t dlpf = (uint8_t)cfg & 0x07;

    if (mpu6050_write_byte(hi2c, addr, MPU6050_REG_CONFIG, dlpf) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}

mpu6050_accel_data_t mpu6050_accelerometer_calibration(mpu6050_accel_data_t offset,
                                                       const mpu6050_accel_data_t *raw)
{
    mpu6050_accel_data_t r;
    r.x = raw->x - offset.x;
    r.y = raw->y - offset.y;
    r.z = raw->z - offset.z;
    return r;
}

/* Interrupt configuration */

mpu6050_status_t mpu6050_interrupt_config(I2C_HandleTypeDef *hi2c,
                                          uint8_t addr,
                                          mpu6050_int_level_t level)
{
    uint8_t int_cfg = 0;

    if (mpu6050_read_byte(hi2c, addr, MPU6050_REG_INT_PIN_CFG, &int_cfg) != MPU6050_OK)
        return MPU6050_ERR;

    int_cfg &= ~0x80;                 /* clear level bit */
    int_cfg |= (uint8_t)level;        /* set new level */

    if (mpu6050_write_byte(hi2c, addr, MPU6050_REG_INT_PIN_CFG, int_cfg) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}

mpu6050_status_t mpu6050_enable_interrupt(I2C_HandleTypeDef *hi2c,
                                          uint8_t addr,
                                          mpu6050_interrupt_t interrupt)
{
    uint8_t current_int_settings = 0;

    if (mpu6050_read_byte(hi2c, addr, MPU6050_REG_INT_EN, &current_int_settings) != MPU6050_OK)
        return MPU6050_ERR;

    current_int_settings |= (uint8_t)interrupt;

    if (mpu6050_write_byte(hi2c, addr, MPU6050_REG_INT_EN, current_int_settings) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}

mpu6050_status_t mpu6050_disable_interrupts(I2C_HandleTypeDef *hi2c,
                                            uint8_t addr,
                                            mpu6050_interrupt_t interrupt)
{
    uint8_t current_int_settings = 0;

    if (interrupt != ALL_INT) {
        if (mpu6050_read_byte(hi2c, addr, MPU6050_REG_INT_EN, &current_int_settings) != MPU6050_OK)
            return MPU6050_ERR;

        current_int_settings &= ~(uint8_t)interrupt;
    } else {
        current_int_settings = 0x00;
    }

    if (mpu6050_write_byte(hi2c, addr, MPU6050_REG_INT_EN, current_int_settings) != MPU6050_OK)
        return MPU6050_ERR;

    return MPU6050_OK;
}

static uint8_t get_interrupt_status(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
    uint8_t int_status = 0;
    mpu6050_read_byte(hi2c, addr, MPU6050_REG_INT_STATUS, &int_status);
    return int_status;
}

static uint8_t get_interrupt_settings(I2C_HandleTypeDef *hi2c, uint8_t addr)
{
    uint8_t int_settings = 0;
    mpu6050_read_byte(hi2c, addr, MPU6050_REG_INT_EN, &int_settings);
    return int_settings;
}

void mpu6050_interrupt_handle(I2C_HandleTypeDef *hi2c)
{
    uint8_t int_status;
    uint8_t int_settings;

    int_status   = get_interrupt_status(hi2c, MPU6050_I2C_ADDR);
    int_settings = get_interrupt_settings(hi2c, MPU6050_I2C_ADDR);

    if ((int_settings & MOT_INT) && (int_status & MOT_INT)) {
        mpu6050_motion_detection_callback();
    }
    else if ((int_settings & RAW_RDY_INT) && (int_status & RAW_RDY_INT)) {
        mpu6050_raw_data_ready_callback();
    }
    else {
        // nothing
    }
}

__weak void mpu6050_motion_detection_callback(void)
{
}

__weak void mpu6050_raw_data_ready_callback(void)
{
}
