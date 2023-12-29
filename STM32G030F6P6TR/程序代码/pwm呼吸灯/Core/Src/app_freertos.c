/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../common.h"
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
/* USER CODE BEGIN Variables */
TaskHandle_t nrf_control_taskHandle;
TaskHandle_t pwm_taskHandle;
TaskHandle_t debug_taskHandle;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
//void debugTaskFun(void const * argument);
void nrf_control_taskFun( void const * argument );
void pwm_taskFun( void const * argument );
void debug_taskFun( void const * argument );
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(nrf_control_task, nrf_control_taskFun, osPriorityNormal, 0, nrf_control_taskSize);
	nrf_control_taskHandle = osThreadCreate(osThread(nrf_control_task), NULL);
	
	osThreadDef(pwm_task, pwm_taskFun, osPriorityNormal, 0, pwm_taskSize);
	pwm_taskHandle = osThreadCreate(osThread(pwm_task), NULL);
	
	osThreadDef(debug_task, debug_taskFun, osPriorityNormal, 0, debug_taskSize);
	debug_taskHandle = osThreadCreate(osThread(debug_task), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	read_data_from_flash();
	getSnByDeviceId_setClientId();
	xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );
	vTaskDelay(100);
	xTaskNotify( nrf_control_taskHandle, 1U<<NRF_REGISTER_DEVICE, eSetBits );
	xTaskNotify( debug_taskHandle, 1U<<DEBUG_INIT, eSetBits );
	
  /* Infinite loop */
  for(;;) {
	vTaskDelete( defaultTaskHandle );
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void nrf_control_taskFun( void const * argument ) {
	uint32_t newBits, oldBits = 0;
	for ( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<NRF_INIT_EVENT) ) {
			oldBits &=~ (1U<<NRF_INIT_EVENT);
			nrf_init();
		}
		if ( oldBits & (1U<<NRF_SEND_DATA) ) {
			oldBits &=~ (1U<<NRF_SEND_DATA);
			nrf_send_data();
		}
		if ( oldBits & (1U<<NRF_PARSE_DATA) ) {
			oldBits &=~ (1U<<NRF_PARSE_DATA);
			nrf_parse_data();
		}
		if ( oldBits & (1U<<NRF_REGISTER_DEVICE) ) {
			oldBits &=~ (1U<<NRF_REGISTER_DEVICE);
			nrf_register_device();
		}
		if ( oldBits & (1U<<NRF_HEARTBEAT) ) {
			oldBits &=~ (1U<<NRF_HEARTBEAT);
			nrf_send_heartbeat();
		}
		if ( oldBits & (1U<<SEND_KEY_DOWN) ) {
			oldBits &=~ (1U<<SEND_KEY_DOWN);
			nrf_push_data("按键按下");
		}
	}
}	

void pwm_taskFun( void const * argument ) {
	uint32_t newBits, oldBits = 0;
	for ( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<POWER_ON) ) {// power_on
			oldBits &=~ (1U<<POWER_ON);
			pwm_str.maxBtnPer = 1000;
			pwm_str.lightSta = 0;
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0 );
			HX_ON();
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
		}
		if ( oldBits & (1U<<POWER_OFF) ) {// power_off
			oldBits &=~ (1U<<POWER_OFF);
			HX_OFF();
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0 );
		}
		if ( oldBits & (1U<<LED_ON) ) {// led_on
			oldBits &=~ (1U<<LED_ON);
			led_turn_on();
		}
		if ( oldBits & (1U<<LED_OFF) ) {// led_off
			oldBits &=~ (1U<<LED_OFF);
			led_turn_off();
		}
		if ( oldBits & (1U<<PWM_FLASHING) ) {// pwm_flashing
			oldBits &=~ (1U<<PWM_FLASHING);
			pwm_flashing();
		}
		
	}
}	
void debug_taskFun( void const * argument ) {
	uint32_t newBits, oldBits = 0;
	for ( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<DEBUG_INIT) ) {
			oldBits &=~ (1U<<DEBUG_INIT);
			debug_init();
		}
		if ( oldBits & (1U<<PARSE_DATA) ) {
			oldBits &=~ (1U<<PARSE_DATA);
			debug_parse_data_fun();
		}
	}
}	

/* USER CODE END Application */

