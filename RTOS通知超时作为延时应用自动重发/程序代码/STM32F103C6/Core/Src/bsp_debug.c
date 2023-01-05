#include "bsp_debug.h"

extern QueueHandle_t Debug_RxData_QueueHandle;//Debug�ش�������Ϣ����
extern QueueHandle_t WH_LTE_Resend_QueueHandle;//4Gģ���Զ��ط���Ϣ����
extern TaskHandle_t WH_LTE_ResendTaskHandle;//4Gģ���Զ��ط���Ϣ����

uint8_t *Debug_DMA_rBuff;//DMA���ջ���ָ��

DEBUG_DATA debug_data;

/*------------------------- Debug���ڽ����жϻص������� start -------------------------*/
void Debug_ISR_RxCallback( void ) {
	uint16_t dataLen;
	BaseType_t pxHigherPriorityTaskWoken;
	dataLen = Debug_rBuff_Size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	memcpy((void *)debug_data.data, (void *)Debug_DMA_rBuff, dataLen);
	debug_data.size = dataLen;
	
	xQueueSendFromISR(Debug_RxData_QueueHandle, (void *)&debug_data, &pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

	HAL_UART_DMAStop(&huart1); 
	HAL_UART_Receive_DMA(&huart1, Debug_DMA_rBuff, Debug_rBuff_Size);//�ٴο���DMA����
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);  //�ٴ�ʹ�ܴ��ڿ����ж�


#if 0//4Gģ��͸������
	dataLen = Debug_rBuff_Size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	if ( Debug_DMA_rBuff[dataLen-1] == '@' ) {
		Debug_DMA_rBuff[dataLen-1] = '\0';

		xQueueSendFromISR(Debug_RxData_QueueHandle, Debug_DMA_rBuff, &pxHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

		HAL_UART_DMAStop(&huart1); 
		HAL_UART_Receive_DMA(&huart1, Debug_DMA_rBuff, Debug_rBuff_Size);//�ٴο���DMA����
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);  //�ٴ�ʹ�ܴ��ڿ����ж�
	}
#endif
}
/*------------------------- Debug���ڽ����жϻص������� end -------------------------*/

/*------------------------- Debug���Դ��ڽ��������� start -------------------------*/
static BaseType_t buffCompareToBuff(char *buff1, char *buff2){
	uint16_t size = strlen(buff1);
	if ( size == 0 ) return pdFALSE;
	while(size--) {
		if ( *buff1 != *buff2 ) return pdFALSE;
		buff1++;
		buff2++;
	}
	return pdTRUE;
}
BaseType_t Debug_ParseRxData( BaseType_t *pRx ) {

	DEBUG_DATA *pTypeData = (DEBUG_DATA *)pRx;
	char *pdata = (char*)pTypeData->data;
	uint8_t size = pTypeData->size;
	
	if ( size==2 && buffCompareToBuff((char *)&"ok", pdata) ) {
		//�˴�ģ�ⷢ�ͻظ�ok���¼��ź�
		xTaskNotify(WH_LTE_ResendTaskHandle, 1, eSetValueWithOverwrite);
		
		
	} else if ( size==strlen("����") && buffCompareToBuff((char *)&"����", pdata) ) {
		//�˴�ģ��������ݷ���������
		xTaskNotify(WH_LTE_ResendTaskHandle, 2, eSetValueWithOverwrite);

	} else {
		xQueueSend( WH_LTE_Resend_QueueHandle, (void *)pRx, 0xFFF );
	}
	
	
	//HAL_UART_Transmit(&huart1, (uint8_t *) pdata, pTypeData->size, 0xffff);
	
//	printf_port = 1;printf("Debug�����յ������ݣ�%s\r\n", pdata);
//	HAL_UART_Transmit(&huart1, (uint8_t *) "Debug�����յ������ݣ�", sizeof("Debug�����յ������ݣ�"), 0xffff);
//	HAL_UART_Transmit(&huart1, (uint8_t *) pdata, pTypeData->size, 0xffff);
//	HAL_UART_Transmit(&huart1, (uint8_t *) "\r\n", sizeof("\r\n"), 0xffff);
	
	return 0;
}
/*------------------------- Debug���Դ��ڽ��������� end -------------------------*/

/*--*/

