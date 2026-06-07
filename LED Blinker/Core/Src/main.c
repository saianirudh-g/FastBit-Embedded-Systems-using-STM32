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
#include "cmsis_os.h"
#include "debug.h"
#include <math.h>
#include "mpu6050.h"
#include "kalman_filter.h"
#include <limits.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SensorReadTask_ */
osThreadId_t SensorReadTask_Handle;
const osThreadAttr_t SensorReadTask__attributes = {
  .name = "SensorReadTask_",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PWMControlTask_ */
osThreadId_t PWMControlTask_Handle;
const osThreadAttr_t PWMControlTask__attributes = {
  .name = "PWMControlTask_",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
/* PWM state globals used in app_callback.c */
uint32_t g_counter = 0;
volatile uint32_t g_channel_2_state = 8000;
volatile uint32_t g_channel_3_state = 4000;

const mpu6050_accel_data_t error_offset = { .x = 250, .y = 200, .z = 156 };

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

static uint32_t map_value(int32_t x, int32_t in_min, int32_t in_max,
                          int32_t out_min, int32_t out_max)
{
    if (x < in_min) x = in_min;
    if (x > in_max) x = in_max;

    return (uint32_t)((x - in_min) * (out_max - out_min) /
                      (in_max - in_min) + out_min);
}

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
void StartDefaultTask(void *argument);
void sensor_read_task(void *argument);
void pwm_control_task(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  mpu6050_disable_interrupts(&hi2c1, MPU6050_I2C_ADDR, ALL_INT);
  mpu6050_interrupt_config(&hi2c1, MPU6050_I2C_ADDR, INT_LEVEL_ACTIVE_HIGH);
  mpu6050_enable_interrupt(&hi2c1, MPU6050_I2C_ADDR, RAW_RDY_INT);

  if (mpu6050_init(&hi2c1, MPU6050_I2C_ADDR) != MPU6050_OK)
      Error_Handler();

  if (mpu6050_disable_interrupts(&hi2c1, MPU6050_I2C_ADDR, ALL_INT) != MPU6050_OK)
      Error_Handler();

  if (mpu6050_interrupt_config(&hi2c1, MPU6050_I2C_ADDR, INT_LEVEL_ACTIVE_HIGH) != MPU6050_OK)
      Error_Handler();

  if (mpu6050_enable_interrupt(&hi2c1, MPU6050_I2C_ADDR, RAW_RDY_INT) != MPU6050_OK)
      Error_Handler();

  if (mpu6050_configure_low_pass_filter(&hi2c1, MPU6050_I2C_ADDR, DLPF_CFG_21HZ) != MPU6050_OK)
      Error_Handler();

  /* Enable TIM2 interrupt */
  __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
  HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  /* Start PWM (normal channels, NOT N channels) */
  if (HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2) != HAL_OK)
      Error_Handler();

  if (HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3) != HAL_OK)
      Error_Handler();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of SensorReadTask_ */
  SensorReadTask_Handle = osThreadNew(sensor_read_task, NULL, &SensorReadTask__attributes);

  /* creation of PWMControlTask_ */
  PWMControlTask_Handle = osThreadNew(pwm_control_task, NULL, &PWMControlTask__attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  #ifdef PWM_DEBUG
	      g_counter = __HAL_TIM_GET_COUNTER(&htim2);
	  #endif

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 8000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : MPU6050_INT_Pin */
  GPIO_InitStruct.Pin = MPU6050_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MPU6050_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_sensor_read_task */
/**
* @brief Function implementing the SensorReadTask_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_sensor_read_task */
void sensor_read_task(void *argument)
{
  /* USER CODE BEGIN sensor_read_task */
  mpu6050_accel_data_t accel_data;

  /* Infinite loop */
  for(;;)
  {
	  if (mpu6050_read_accelerometer_data(&hi2c1, MPU6050_I2C_ADDR, &accel_data) != MPU6050_OK) {
	  	      Error_Handler();
	  	  }

	  uint32_t accel_data_y_z = accel_data.y;
	  accel_data_y_z = (accel_data_y_z << 16) | accel_data.z;

	  accel_data = mpu6050_accelerometer_calibration(error_offset, &accel_data);

	  //send the notification to the PWM Control Task
	  xTaskNotify((TaskHandle_t) PWMControlTask_Handle, accel_data_y_z, eSetValueWithOverwrite);

	  //Enter blocked state until a direct notification is received from the ISR
	  xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
  }
  /* USER CODE END sensor_read_task */
}

/* USER CODE BEGIN Header_pwm_control_task */
/**
* @brief Function implementing the PWMControlTask_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pwm_control_task */
void pwm_control_task(void *argument)
{
  /* USER CODE BEGIN pwm_control_task */
	uint32_t previous_tick = osKernelGetTickCount();
	float roll_angle;
	float dt = 0;
	int16_t roll_angle_filtered;
	mpu6050_accel_data_t accel_data;
	uint32_t accel_data_y_z;

	KalmanFilter kf;
	kalman_filter_init(&kf);
  /* Infinite loop */
  for(;;)
  {
	  //Wakes upon notification and extract accel_data
	  //Enter blocked state until a direct notification is received from the ISR
	  xTaskNotifyWait(0, ULONG_MAX, &accel_data_y_z, portMAX_DELAY);
	  accel_data.z = (int16_t)accel_data_y_z & 0xFFFF;
	  accel_data.y = (int16_t)((accel_data_y_z >> 16) & 0xFFFF);

	  uint32_t current_tick = osKernelGetTickCount();
	  dt = (current_tick - previous_tick) / 1000.0f;
	  previous_tick = current_tick;

	  roll_angle = atan2(accel_data.y, accel_data.z) * (180.0 / M_PI);
	  DEBUG_ROLL_ANGLE(roll_angle);
	  DEBUG_DT(dt*1000);

	  roll_angle_filtered = (int16_t)kalman_filter_get_angle(&kf, roll_angle, dt);

	  uint8_t channel = (roll_angle_filtered < 0) ? TIM_CHANNEL_2 : TIM_CHANNEL_3;

	  roll_angle_filtered = (roll_angle_filtered < 0) ? -roll_angle_filtered : roll_angle_filtered;

	  uint32_t pwm_pulse = map_value(roll_angle_filtered, ANGLE_POS_MIN, ANGLE_POS_MAX, PWM_PULSE_MIN, PWM_PULSE_MAX);

	  change_pwm_duty_cycle(pwm_pulse, channel);
  }
  /* USER CODE END pwm_control_task */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
