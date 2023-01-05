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
#include "bsp_debug.h"
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
QueueHandle_t Debug_RxData_QueueHandle;//Debug�ش�������Ϣ����
QueueHandle_t WH_LTE_Resend_QueueHandle;//4Gģ���Զ��ط���Ϣ����

osThreadId Debug_ControlTaskHandle;
osThreadId WH_LTE_ResendTaskHandle;//4Gģ���Զ��ط�����
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Debug_ControlTaskFun( void const * argument );
void WH_LTE_ResendTaskFun( void const * argument );
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
	Debug_RxData_QueueHandle = xQueueCreate(2, sizeof(DEBUG_DATA));
	WH_LTE_Resend_QueueHandle = xQueueCreate( 5, sizeof(DEBUG_DATA) );//4Gģ���ط�������Ϣ����
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
	//Debug���ڵ������ݴ���Task
	osThreadDef(Debug_ControlTask, Debug_ControlTaskFun, osPriorityIdle, 0, 128);
  Debug_ControlTaskHandle = osThreadCreate(osThread(Debug_ControlTask), NULL);
	//4Gģ���Զ��ط�
	osThreadDef(WH_LTE_ResendTask, WH_LTE_ResendTaskFun, osPriorityIdle, 0, 128);
  WH_LTE_ResendTaskHandle = osThreadCreate(osThread(WH_LTE_ResendTask), NULL);
	
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
	printf("��ʼ��...");
	
	//Debug���ڵ���
	Debug_DMA_rBuff = pvPortMalloc( Debug_rBuff_Size );
	HAL_UART_Receive_DMA(&huart1,Debug_DMA_rBuff,Debug_rBuff_Size);//����DMA����
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//ʹ�ܿ����ж�
	
	printf("��ʼ�����,�ڴ�ʣ��%d\r\n",xPortGetFreeHeapSize());
  for(;;)
  {
    vTaskDelay(2000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
	*	Debug���ڽ��մ�������
	*/
void Debug_ControlTaskFun ( void const * argument ) {
	
	BaseType_t result;
	BaseType_t *rxData = pvPortMalloc(Debug_rBuff_Size);
  for( ;; ) {
		result = xQueueReceive( Debug_RxData_QueueHandle, rxData, portMAX_DELAY );
		if ( result == pdTRUE ) {
			Debug_ParseRxData( rxData );
		}
	}
}

//4Gģ���Զ��ط�
void WH_LTE_ResendTaskFun ( void const * argument ) {
	
	BaseType_t result;
	BaseType_t *rxData = pvPortMalloc(sizeof(DEBUG_DATA));
	DEBUG_DATA *typeData;
	uint32_t notifyValue;
  for( ;; ) {

		result = xQueueReceive( WH_LTE_Resend_QueueHandle, rxData, portMAX_DELAY );
		if ( result == pdTRUE ){
			typeData = (DEBUG_DATA *)rxData;
			
			for ( ;; ) {
				//�õȴ�֪ͨ�ķ�ʽ�����ʱ�ط�
				//����ʱ�᷵��pdFALSE,Ϊ��ʱ�յ������򷵻�pdTRUE
				HAL_UART_Transmit(&huart1, (uint8_t *) typeData, typeData->size, portMAX_DELAY);
				result = xTaskNotifyWait( pdFALSE, portMAX_DELAY, &notifyValue, 1000 );
				if ( result == pdTRUE ) {
					if ( notifyValue == 1 ) {
						//�յ�ok
						HAL_UART_Transmit(&huart1, (uint8_t *) &"�յ�ok\r\n", strlen("�յ�ok\r\n"), portMAX_DELAY);
						break;
					} else if ( notifyValue == 2 ) {
						//��������
						HAL_UART_Transmit(&huart1, (uint8_t *) &"����\r\n", strlen("����\r\n"), portMAX_DELAY);
						break;
					}
				}
				//��ʱ,δ�յ�ok,�ط�����
			}
			
		}
			
	}
}
/* USER CODE END Application */

