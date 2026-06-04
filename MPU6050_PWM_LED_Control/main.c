/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <math.h>
#include <stdio.h>
#include "mpu6050.h"
#include "kalman_filter.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
uint32_t g_counter = 0;
volatile uint32_t g_channel_2_state = 8000;
volatile uint32_t g_channel_3_state = 4000;

mpu6050_accel_data_t g_accel_data;
const mpu6050_accel_data_t error_offset = {
    .x = 250,
    .y = 200,
    .z = 156
};

float dt = 0.0f;
uint32_t previous_tick = 0;
float roll_angle = 0.0f;
float kalman_roll_angle = 0.0f;
int16_t roll_angle_filtered = 0;

/* Angle and PWM limits */
#define ANGLE_POS_MIN    0
#define ANGLE_POS_MAX    90

#define PWM_PULSE_MIN    1000
#define PWM_PULSE_MAX    2000

/* Map function */
static uint32_t map_value(int32_t x, int32_t in_min, int32_t in_max,
                          int32_t out_min, int32_t out_max)
{
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;

    return (uint32_t)((x - in_min) * (out_max - out_min) /
                      (in_max - in_min) + out_min);
}

/* PWM duty cycle change */
static void change_pwm_duty_cycle(uint32_t pulse, uint32_t channel)
{
    __HAL_TIM_SET_COMPARE(&htim2, channel, pulse);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  KalmanFilter kf;
  kalman_filter_init(&kf);

  if (mpu6050_init(&hi2c1, MPU6050_I2C_ADDR) != MPU6050_OK) {
      Error_Handler();
  }

  previous_tick = HAL_GetTick();

  __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

  if (HAL_TIMEx_PWMN_Start_IT(&htim2, TIM_CHANNEL_2) != HAL_OK) {
      Error_Handler();
  }

  if (HAL_TIMEx_PWMN_Start_IT(&htim2, TIM_CHANNEL_3) != HAL_OK) {
      Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
    /* USER CODE BEGIN 3 */
    uint32_t current_tick = HAL_GetTick();
    dt = (current_tick - previous_tick) / 1000.0f;
    previous_tick = current_tick;

#ifdef PWM_DEBUG
    g_counter = __HAL_TIM_GET_COUNTER(&htim2);
#endif

    if (mpu6050_read_accelerometer_data(&hi2c1, MPU6050_I2C_ADDR, &g_accel_data) != MPU6050_OK) {
        Error_Handler();
    }

    g_accel_data = mpu6050_read_accelerometer_calibration(&error_offset, &g_accel_data);

    roll_angle = atan2f((float)g_accel_data.y, (float)g_accel_data.z) * (180.0f / (float)M_PI);

    kalman_roll_angle = kalman_filter_get_angle(&kf, roll_angle, dt);
    roll_angle_filtered = (int16_t)kalman_roll_angle;

    uint32_t channel = (roll_angle_filtered < 0) ? TIM_CHANNEL_2 : TIM_CHANNEL_3;

    if (roll_angle_filtered < 0)
        roll_angle_filtered = -roll_angle_filtered;

    uint32_t pwm_pulse = map_value(roll_angle_filtered,
                                   ANGLE_POS_MIN, ANGLE_POS_MAX,
                                   PWM_PULSE_MIN, PWM_PULSE_MAX);

    change_pwm_duty_cycle(pwm_pulse, channel);

    // printf("Roll: %.2f  Kalman: %.2f\n", roll_angle, kalman_roll_angle);
    // HAL_Delay(500);
    /* USER CODE END 3 */
  }
}

/* System Clock Configuration */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
      Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
      Error_Handler();
  }
}

/* I2C1 Initialization Function */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
      Error_Handler();
  }
}

/* TIM2 Initialization Function */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
      Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
      Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
      Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
      Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 8000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
      Error_Handler();
  }

  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
      Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);
}

/* GPIO Initialization Function */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

/* Error Handler */
void Error_Handler(void)
{
  __disable_irq();
  while (1) {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif
