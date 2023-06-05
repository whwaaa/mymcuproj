/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
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
#include "common.h"
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

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId usart_wifi_TaskHandle;
osThreadId usart_debug_TaskHandle;

void usart_wifi_TaskHandleFun(void const *argument);
void usart_debug_TaskHandleFun(void const *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
	osThreadDef(usart_wifi_Task, usart_wifi_TaskHandleFun, osPriorityNormal, 0, 128);
	usart_wifi_TaskHandle = osThreadCreate(osThread(usart_wifi_Task), NULL);
	
	osThreadDef(usart_debug_Task, usart_debug_TaskHandleFun, osPriorityNormal, 0, 128);
	usart_debug_TaskHandle = osThreadCreate(osThread(usart_debug_Task), NULL);
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
  /* Infinite loop */
  for(;;)
  {
		//������ʼ��

		//debug��ʼ��
		xTaskNotify( usart_debug_TaskHandle, 1U<<DEBUG_DEVICE_INIT, eSetBits );
		//wifi��ʼ��
		xTaskNotify( usart_wifi_TaskHandle, 1U<<WIFI_DEVICE_INIT, eSetBits );
		//wifi
		vTaskDelete( defaultTaskHandle );
		
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void usart_wifi_TaskHandleFun(void const * argument) {
	uint32_t newBits, oldBits;
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<WIFI_DEVICE_INIT) ) {//��ʼ��wifi����
			oldBits &=~ (1U<<WIFI_DEVICE_INIT);
			wifi_init();
		}
		if ( oldBits & (1U<<WIFI_PARSE_DATA) ) {//��������������
			oldBits &=~ (1U<<WIFI_PARSE_DATA);
			wifi_parse_data();
		}
  }
  /* USER CODE END StartDefaultTask */
}

void usart_debug_TaskHandleFun(void const * argument) {
	uint32_t newBits, oldBits;
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<DEBUG_DEVICE_INIT) ) {//��ʼ��wifi����
			oldBits &=~ (1U<<DEBUG_DEVICE_INIT);
			debug_init();
		}
		if ( oldBits & (1U<<DEBUG_PARSE_DATA) ) {//��������������
			oldBits &=~ (1U<<DEBUG_PARSE_DATA);
			debug_parse_data();
		}
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE END Application */

