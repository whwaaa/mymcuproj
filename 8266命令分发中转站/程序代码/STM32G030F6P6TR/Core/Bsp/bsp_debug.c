#include "../Bsp/bsp_debug.h"
#define ds debug_str
DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &DEBUG_HUART );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}

void debug_parse_data_fun( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &DEBUG_HUART );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
	
	if ( strstr( (char *)ds.rxBuff, "debug" ) != NULL ) {
		printf("debug\r\n");
	} 
	
//	else if ( strstr( (char *)ds.rxBuff, "clear reg" ) != NULL ) {
//		SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xf0 );//0xFF��ָ��
//		printf("clear reg ok\r\n");

//	} else if ( strstr( (char *)ds.rxBuff, "clear fifo" ) != NULL ) {
//		SPI_RW_Reg( FLUSH_RX,NOP );
//		printf("clear fifo ok\r\n");
//		
//	} else if ( strstr( (char *)ds.rxBuff, "reg" ) != NULL ) {
//		uint8_t sta = SPI_RW_Reg(NRF_READ_REG + STATUS, 0xff );//0xFF��ָ��
//		printf("sta: %02X\r\n", sta);
//		
//	} else if ( strstr( (char *)ds.rxBuff, "gpio" ) != NULL ) {
//		uint8_t temp = HAL_GPIO_ReadPin( NRF_IRQ_GPIO_Port, NRF_IRQ_Pin );
//		printf("temp: %d\r\n", temp);
//	
//	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}
