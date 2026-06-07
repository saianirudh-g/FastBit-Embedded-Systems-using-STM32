/*
 * app_callback.c
 *
 *  Created on: 03-Jun-2026
 *      Author: saian
 */
#include "main.h"
#include "mpu6050.h"
#include "cmsis_os.h"
#include "task.h"

extern uint32_t g_counter;
extern volatile uint32_t g_channel_2_state;
extern volatile uint32_t g_channel_3_state;
extern I2C_HandleTypeDef hi2c1;
extern osThreadId_t SensorReadTask_Handle;

#if 0
//Called during update event of the counter
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  g_counter = 0;
  g_channel_2_state = 8000;
  g_channel_3_state = 4000;
}
#endif

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
	  g_channel_2_state = 0;
  }
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
	  g_channel_3_state = 0;
   }
}

void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == MPU6050_INT_Pin){
		mpu6050_interrupt_handle(&hi2c1);
	}
}

//this is the ISR function
void mpu6050_raw_data_ready_callback(void){
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	if(SensorReadTask_Handle){
	xTaskNotifyFromISR( SensorReadTask_Handle,
	                    0,
	                    eNoAction,
	                    &xHigherPriorityTaskWoken );
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
