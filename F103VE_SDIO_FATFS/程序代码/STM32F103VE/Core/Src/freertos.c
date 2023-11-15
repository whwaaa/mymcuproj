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
#include "../common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
_USER_STR u_str;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
TaskHandle_t myTestTaskHandle;
TaskHandle_t debugTaskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void myTestTaskFun(void const * argument);
void debugTaskFun(void const * argument);
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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 2048);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(myTestTask, myTestTaskFun, osPriorityNormal, 0, 128);
    myTestTaskHandle = osThreadCreate(osThread(myTestTask), NULL);
	
	osThreadDef(debugTask, debugTaskFun, osPriorityNormal, 0, 128);
    debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);
	
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
	printf("\r\n\r\n\r\n\r\nstart\r\n");
	//memset( &u_str, 0, sizeof(_USER_STR) );
	xTaskNotify( debugTaskHandle, 1U<<DEBUG_DEVICE_INIT, eSetBits );
	//fatfs_init_fun();
	//my_fatfs_test_fun();
	//fatfs_test_fun();
  /* Infinite loop */
  for(;;)
  {
	osDelay(1000);
	vTaskDelete( defaultTaskHandle );
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void myTestTaskFun(void const * argument) {
	for(;;) {
		osDelay(2000);
		//printf("...\r\n");
	}
}

void debugTaskFun(void const * argument) {
	uint32_t newBits, oldBits = 0;
	for( ; ; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<DEBUG_DEVICE_INIT) ) {
			oldBits &=~ (1U<<DEBUG_DEVICE_INIT);
			debug_init();
		}
		if ( oldBits & (1U<<DEBUG_PARSE_DATA) ) {
			oldBits &=~ (1U<<DEBUG_PARSE_DATA);
			debug_parse_data_fun();
			//debug_parse_data_fun2();
		}
		if ( oldBits & (1U<<D_DMA_HT_FUN) ) {
			oldBits &=~ (1U<<D_DMA_HT_FUN);
			//dma_ht_fun();
		}
		if ( oldBits & (1U<<D_DMA_TC_FUN) ) {
			oldBits &=~ (1U<<D_DMA_TC_FUN);
			//dma_tc_fun();
		}
		if ( oldBits & (1U<<D_TEST_UART_IT_TC) ) {
			oldBits &=~ (1U<<D_TEST_UART_IT_TC);
			//test_uart_it_tc();
		}
	}
}
/* USER CODE END Application */



