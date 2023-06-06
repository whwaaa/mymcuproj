#include "bsp_wifi.h"

#define ws wifi_str

WIFI_STR wifi_str;

static int send_at_command_a_single_reply( char *cmd, char *target, int headTail, uint8_t idx );
static void wifi_smartConfig( void );
static int check_wifi_connect( void );
static int connect_wifi( void );
static void get_sntp_time( void );

		
void wifi_init( void ) {
	
	memset( (void*)&ws, 0, sizeof( WIFI_STR ) );
	vTaskDelay(100);
	ws.rxBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	ws.txBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	
	wifi_str.isConfig = 1;
	
//	while(1) {
//		ws.askConfig = 0;
//		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
//		while( !ws.askConfig ){;;} 
//		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
//		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
//		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
//	}
	send_at_command_a_single_reply("ATE0", "OK", -1, strlen("OK\r\n") );
	
	//��ѯ����״̬����Ϣ
	if ( check_wifi_connect() == -1 ) {
	//if ( send_at_command_a_single_reply("AT+CIPSTA?", "STATUS:2", 0, 0 ) == -1 ) {
		/*---- �������� -----*/
		//��ȡflash,��ȡ�˺�����
		read_from_flash();
		//��������wifi
		if ( connect_wifi() == -1 ) {
			//����ʧ��,������������
			wifi_smartConfig();
		}
		
		//�ϵ��Զ�����AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWAUTOCONN=1", "OK", -1, strlen("OK\r\n") );
	}
	while ( check_wifi_connect() == -1 ){;;}
		
	//SoftAP+Station ģʽ: AT+CWMODE=3
	send_at_command_a_single_reply("AT+CWMODE=3", "OK", -1, strlen("OK\r\n") );
	
	//���ö�����:	AT+CIPMUX=1
	send_at_command_a_single_reply("AT+CIPMUX=1", "OK", -1, strlen("OK\r\n") );

	//���÷����������������������:		AT+CIPSERVERMAXCONN=2
	send_at_command_a_single_reply("AT+CIPSERVERMAXCONN=2", "OK", -1, strlen("OK\r\n") );
	
	//��ѯsntp������
	send_at_command_a_single_reply("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"", "OK", -1, strlen("OK\r\n") );
	
	//��ȡsntpʱ��
	get_sntp_time();
	
	//����TCP������:		AT+CIPSERVER=1,8266
	send_at_command_a_single_reply("AT+CIPSERVER=1,8266", "OK", -1, strlen("OK\r\n") );
		
	//res = send_at_command_a_single_reply("AT+CIPSTATUS", "STATUS:2", 0, 0 );
	
	//printf("res: %d\r\n", res);
	while(1) {
		ws.askConfig = 0;
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
	}
	
	//wifi���Ӽ���� led��˸_start
	//vTaskDelay( 3000 );

	//AT+CWJAP="MI","12345678"
}


void wifi_parse_data( void ) {
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//�ж�����
}

static int send_at_command_a_single_reply( char *cmd, char *target, 
	int headTail, uint8_t idx ) {
	uint8_t times = 5;
	ws.askConfig = 0;
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	while ( times-- ) { 
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;} 
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		
		if ( headTail == 0 ) {//0:head
			if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[idx], strlen(target) ) ){
				//return -1;
				vTaskDelay(500);
			} else {
				return 0;
			}
		} else if ( headTail == -1 ) {//-1:tail
			if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[ws.len-idx], strlen(target) ) ){
				//return -1;
				vTaskDelay(500);
			} else {
				return 0;
			}
		}
	}
	return -1;
}

static void wifi_smartConfig( void ) {
	printf( "������������\r\n" );
	//����start(����˸)
	send_at_command_a_single_reply("AT+CWMODE=1", "OK", -1, strlen("OK\r\n") );
	//��������:  AT+CWSTARTSMART=3
	sprintf(ws.txBuff, "AT+CWSTARTSMART=3\r\n");
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	for( ;;	) {
		ws.askConfig = 0;
		while( !ws.askConfig ){;;} 
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff("") )
	}
	
	
	
	
	
//	sprintf( param_str.wifiName, "MI");
//	sprintf( param_str.wifiPasswd, "12345678");
	write_to_flash();
	
	
	
	//�˳���������:   AT+CWSTOPSMART
	
	//����end(�ƹر�)
}


static int check_wifi_connect() {
	uint8_t timeout = 10;
	sprintf( (char*)ws.txBuff, "AT+CIPSTA?\r\n" );	
	while( timeout ) {	
		ws.askConfig = 0;
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff( "OK\r\n", (char *)&ws.rxBuff[ws.len - strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			if ( !buffCompareToBuff( "\"0.0.0.0\"", (char *)&ws.rxBuff[ws.len - strlen("\"0.0.0.0\"\r\n\r\nOK\r\n")], strlen("\"0.0.0.0\"") ) ) {
				return 0;
			} else {
				return -1;
			}
		} else {
			timeout--;
		}
		vTaskDelay( 1000 );
	}
	return -1;
}

static int connect_wifi( void ) {
	uint8_t timeout = 10;
	sprintf( (char*)ws.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", param_str.wifiName, param_str.wifiPasswd );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	while( timeout ) {	
		ws.askConfig = 0;
		while( !ws.askConfig ){;;}
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff( "ERROR\r\n", (char *)&ws.rxBuff[ws.len - strlen("ERROR\r\n")], strlen("ERROR\r\n") ) ) {
			printf( "wifi����ʧ��!\r\n" );
			return -1;
		} else if ( buffCompareToBuff( "OK\r\n", (char *)&ws.rxBuff[ws.len - strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			printf( "wifi���ӳɹ�!\r\n" );
			return 0;
		} else {
			timeout--;
			HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		}
	}
	return -1;
}


static void get_sntp_time( void ) {
	#define ps param_str
	uint8_t times = 10;
	sprintf( (char*)ws.txBuff, "AT+CIPSNTPTIME?\r\n" );
	while(times) {
		ws.askConfig = 0;
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen("AT+CIPSNTPTIME?\r\n"), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		//printf("---\r\n%s\r\n---\r\n", ws.rxBuff);
		if ( buffCompareToBuff( "OK\r\n", (char*)&ws.rxBuff[ws.len-strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			if ( !buffCompareToBuff( "1970", (char*)&ws.rxBuff[ws.len-strlen("1970\r\nOK\r\n")], strlen("1970") ) ) {
				//��ȡ�����µ�ʱ��
				ws.nowHour = (ws.rxBuff[ws.len-strlen("hh:mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("h:mm:ss yyyy\r\nOK\r\n")]-0x30;
				ws.nowMin = (ws.rxBuff[ws.len-strlen("mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("m:ss yyyy\r\nOK\r\n")]-0x30;
				check_turn_on_time();
				break;
			} else {
				times--;
			}
		}
		vTaskDelay( 1000 );
	}
}


int check_turn_on_time( void ) {
	ws.startHour = 19;
	ws.endHour = 20;
	if ( ws.endHour > ws.startHour ) {//һ�쵱��
		if ( ws.nowHour>=ws.startHour && ws.nowHour<ws.endHour ) {
			ws.runTime = ( ws.endHour - (ws.nowHour+1) )*3600000 + (60-ws.nowMin)*60000 | UNTIL_TIME;
			return 0;
		} else {
			return -1; 
		}
	} else {//��ֹʱ���һ��
		if ( ( ws.nowHour>=ws.startHour && ws.nowHour<=23 ) || ( ws.nowHour<ws.endHour ) ) {
			if ( ws.nowHour>=ws.startHour && ws.nowHour<=23 ) {
				ws.runTime = ( 24 - (ws.nowHour+1) + ws.endHour )*3600000 + (60-ws.nowMin)*60000 | UNTIL_TIME;
			} else {
				ws.runTime = ( ws.endHour - (ws.nowHour+1) )*3600000 + (60-ws.nowMin)*60000 | UNTIL_TIME;
			}
			//����ػ�ʱ��,����֪ͨ
			return 0;
		} else { 
			return -1; 
		}
	}
	
	
	
	
}








