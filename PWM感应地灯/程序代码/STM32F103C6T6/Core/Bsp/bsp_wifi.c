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
	
	vTaskDelay(500);
	sprintf( (char*)ws.txBuff, "AT+RST\r\n" );
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	vTaskDelay(2000);
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_ON, eSetBits );//led����,���wifi
	
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	
	wifi_str.isConfig = 1;
	
	read_from_flash();
	param_str.startHour = 20;
	param_str.endMin = 8;
	write_to_flash();
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
		//xTaskNotify( led_taskHandle, 1U<<LED_SLOW_FLASHING, eSetBits );//led����,����wifi
		//��ȡflash,��ȡ�˺�����
		read_from_flash();
		//��������wifi
		if ( connect_wifi()==-1 || param_str.wifiName[0]==0 ) {
			printf( "����ʧ��,������������\r\n" );
			xTaskNotify( led_taskHandle, 1U<<LED_FAST_FLASHING, eSetBits );//led����,smarconfig
			//����ʧ��,������������
			wifi_smartConfig();
		}
		
		//�ϵ��Զ�����AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWAUTOCONN=1", "OK", -1, strlen("OK\r\n") );
	}
	while ( check_wifi_connect() == -1 ){;;}
	
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_ON, eSetBits );//led����,����wifi
		
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
		
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_OFF, eSetBits );//led��,�������
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
	char *ppos;
	uint8_t i;
	uint16_t cont;
	
	
	taskENTER_CRITICAL();
	memset(wifi_str.rxBuff, 0, DEBUG_BUFF_SIZE);
	vTaskDelay(100);
	printf( "\r\n------\r\n������������\r\n------\r\n" );
	taskEXIT_CRITICAL();
	//����start(����˸)
	send_at_command_a_single_reply("AT+CWMODE=1", "OK", -1, strlen("OK\r\n") );
	//��������:  AT+CWSTARTSMART=3
	
	for( ;;	) {
		cont = 600;//1min
		ws.askConfig = 0;
		sprintf((char*)ws.txBuff, "AT+CWSTOPSMART\r\n");
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.askConfig = 0;
		sprintf((char*)ws.txBuff, "AT+CWSTARTSMART=3\r\n");
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);
		//ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( !buffCompareToBuff( "\r\nOK\r\n", (char*)ws.rxBuff, 6 ) ) {
			vTaskDelay(100);
			HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
			HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
			continue;
		}
		while ( strstr((char*)ws.rxBuff,"smartconfig") == NULL ){;;}
		while ( cont ) {
			ws.askConfig = 0;
			while( !ws.askConfig ){ 
				vTaskDelay(100); --cont;
				if ( cont == 0 ) break;
			}
			if ( cont == 0 ) break;
			//ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
			if ( strstr((char*)ws.rxBuff,"WIFI GOT IP") != NULL ) {
				HAL_UART_AbortReceive( &huart2 );
				memset( param_str.wifiName, 0, 50 );
				memset( param_str.wifiPasswd, 0, 30 );
				vTaskDelay(50);
				ppos = strstr((char*)ws.rxBuff,"ssid:")+5;
				i=0; while( ppos[i]!='\r' ){i++;}
				memcpy( param_str.wifiName, ppos, i);
				ppos = strstr((char*)ws.rxBuff,"password:")+9;
				i=0; while( ppos[i]!='\r' ){i++;}
				memcpy( param_str.wifiPasswd, ppos, i);
				cont = 1;
				write_to_flash();//�洢�˺�����
				break;
			}
		}
		printf("\r\n----- \r\n%s\r\n", ws.rxBuff);
		if ( cont == 0 ) continue;//��ʱδ����,�������,���¿���
		break;
	}
	//�˳���������:   AT+CWSTOPSMART
	send_at_command_a_single_reply("AT+CWSTOPSMART", "OK", -1, strlen("OK\r\n") );

	//����end(�ƹر�)
}


static int check_wifi_connect() {
	uint8_t timeout = 5;
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
	uint8_t timeout = 5;
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
				ps.nowHour = (ws.rxBuff[ws.len-strlen("hh:mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("h:mm:ss yyyy\r\nOK\r\n")]-0x30;
				ps.nowMin = (ws.rxBuff[ws.len-strlen("mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("m:ss yyyy\r\nOK\r\n")]-0x30;
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
	ps.startHour = 19;
	ps.endHour = 20;
	if ( ps.endHour > ps.startHour ) {//һ�쵱��
		if ( ps.nowHour>=ps.startHour && ps.nowHour<ps.endHour ) {
			ps.runTime = ( ps.endHour - (ps.nowHour+1) )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			return 0;
		} else {
			return -1; 
		}
	} else {//��ֹʱ���һ��
		if ( ( ps.nowHour>=ps.startHour && ps.nowHour<=23 ) || ( ps.nowHour<ps.endHour ) ) {
			if ( ps.nowHour>=ps.startHour && ps.nowHour<=23 ) {
				ps.runTime = ( 24 - (ps.nowHour+1) + ps.endHour )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			} else {
				ps.runTime = ( ps.endHour - (ps.nowHour+1) )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			}
			//����ػ�ʱ��,����֪ͨ
			return 0;
		} else { 
			return -1; 
		}
	}
	
	
	
	
}








