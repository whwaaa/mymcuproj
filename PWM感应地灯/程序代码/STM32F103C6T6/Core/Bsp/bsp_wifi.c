#include "bsp_wifi.h"

#define ws wifi_str

WIFI_STR wifi_str;

static int send_at_command_a_single_reply( char *cmd, char *target, int headTail, uint8_t idx );
static void wifi_smartConfig( void );
		
void wifi_init( void ) {
	int res;
	
	memset( (void*)&ws, 0, sizeof( WIFI_STR ) );
	ws.rxBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	ws.txBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	
	wifi_str.isConfig = 1;
	
	//��ѯ����״̬����Ϣ
	if ( send_at_command_a_single_reply("AT+CIPSTATUS", "STATUS:2", 0, 0 ) == -1 ) {
		//��������
		//Station ģʽ:   AT+CWMODE=1
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
		//��ȡflash,��ȡ�˺�����
		if (  ) {//����ʧ��,�ٽ�������ģʽ
			
		}
		
		//��������
		wifi_smartConfig();
		
		//�ϵ��Զ�����AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
		//���ö�����:			AT+CIPMUX=1
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
		//SoftAP+Station ģʽ: 
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
	}
	
	res = send_at_command_a_single_reply("AT+CIPSTATUS", "STATUS:2", 0, 0 );
	
	printf("res: %d\r\n", res);
	while(1) {
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	}
	
	//wifi���Ӽ���� led��˸_start
	vTaskDelay( 3000 );
	

	//AT+CWJAP="MI","12345678"
	
	
	 
	
}


void wifi_parse_data( void ) {
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//�ж�����
}

static int send_at_command_a_single_reply( char *cmd, char *target, 
	int headTail, uint8_t idx ) {
	ws.askConfig = 0;
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	while( !ws.askConfig ){;;} 
	vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	
	if ( headTail == 0 ) {//0:head
		if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[idx], strlen(target) ) ){
			return -1;
		} else {
			return 0;
		}
	} else if ( headTail == -1 ) {//-1:tail
		if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[ws.len-idx], strlen(target) ) ){
			return -1;
		} else {
			return 0;
		}
	}
	return -1;
}

static void wifi_smartConfig( void ) {
	//����start(����˸)
	//��������:       AT+CWSTARTSMART=3
	//�˳���������:   AT+CWSTOPSMART
	
	//����end(�ƹر�)
}




