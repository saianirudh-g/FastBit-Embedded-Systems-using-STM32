#ifndef MPU6050_H_
#define MPU6050_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* I2C address */
#define MPU6050_I2C_ADDR            (0x68 << 1)

/* Register map */
#define MPU6050_REG_WHOAMI          0x75
#define MPU6050_REG_PWR_MGMT_1      0x6B
#define MPU6050_REG_INT_PIN_CFG     0x37
#define MPU6050_REG_INT_EN          0x38
#define MPU6050_REG_INT_STATUS      0x3A   // <-- needed by mpu6050.c
#define MPU6050_REG_ACCEL_START     0x3B
#define MPU6050_REG_CONFIG          0x1A

/* Interrupt bits */
#define RAW_RDY_INT_BIT             0x01
#define MOT_INT_BIT                 0x40
#define ALL_INT_BITS                0xFF

/* DLPF configuration values */
typedef enum {
    DLPF_CFG_260HZ = 0,
    DLPF_CFG_184HZ = 1,
    DLPF_CFG_94HZ  = 2,
    DLPF_CFG_44HZ  = 3,
    DLPF_CFG_21HZ  = 4,
    DLPF_CFG_10HZ  = 5,
    DLPF_CFG_5HZ   = 6
} mpu6050_dlpf_config_t;

/* Interrupt pin level */
typedef enum {
    INT_LEVEL_ACTIVE_HIGH = 0,
    INT_LEVEL_ACTIVE_LOW  = 1
} mpu6050_int_level_t;

/* Interrupt mask type */
typedef enum {
    RAW_RDY_INT = RAW_RDY_INT_BIT,
    MOT_INT     = MOT_INT_BIT,
    ALL_INT     = ALL_INT_BITS
} mpu6050_interrupt_t;

typedef enum {
    MPU6050_OK = 0,
    MPU6050_ERR
} mpu6050_status_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} mpu6050_accel_data_t;

/* API */
mpu6050_status_t mpu6050_init(I2C_HandleTypeDef *hi2c, uint8_t addr);

mpu6050_status_t mpu6050_configure_low_pass_filter(I2C_HandleTypeDef *hi2c,
                                                   uint8_t addr,
                                                   mpu6050_dlpf_config_t cfg);

mpu6050_status_t mpu6050_read_accelerometer_data(I2C_HandleTypeDef *hi2c,
                                                 uint8_t addr,
                                                 mpu6050_accel_data_t *accel);

/* NOTE: second parameter is const to match mpu6050.c */
mpu6050_accel_data_t mpu6050_accelerometer_calibration(mpu6050_accel_data_t offset,
                                                       const mpu6050_accel_data_t *raw);

mpu6050_status_t mpu6050_disable_interrupts(I2C_HandleTypeDef *hi2c,
                                            uint8_t addr,
                                            mpu6050_interrupt_t interrupt);

mpu6050_status_t mpu6050_interrupt_config(I2C_HandleTypeDef *hi2c,
                                          uint8_t addr,
                                          mpu6050_int_level_t level);

mpu6050_status_t mpu6050_enable_interrupt(I2C_HandleTypeDef *hi2c,
                                          uint8_t addr,
                                          mpu6050_interrupt_t interrupt);

/* ISR-related helpers used by app_callback.c and mpu6050.c */
void mpu6050_interrupt_handle(I2C_HandleTypeDef *hi2c);

/* Weak callbacks implemented in mpu6050.c */
void mpu6050_motion_detection_callback(void);
void mpu6050_raw_data_ready_callback(void);

#endif
