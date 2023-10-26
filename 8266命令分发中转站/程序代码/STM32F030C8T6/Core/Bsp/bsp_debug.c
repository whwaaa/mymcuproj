#include "../Bsp/bsp_debug.h"
#define ds debug_str
DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_TC );//使能发送完毕中断
	HAL_UART_AbortReceive( &DEBUG_HUART );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}

void debug_parse_data_fun( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &DEBUG_HUART );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
	
	if ( strstr( (char *)ds.rxBuff, "erase" ) != NULL ) {
		user_flash_erase();
		printf("擦除ok\r\n");
		
	} else if ( strstr( (char *)ds.rxBuff, "rrr" ) != NULL ) {
		printf("udata.pswd: %s\r\n", udata.wpswd);
		printf("udata.ssid: %s\r\n", udata.wssid);
		printf("udata.tcpUrl: %s\r\n", udata.tcpurl);
		printf("udata.tcpPort: %d\r\n", udata.tcpport);
		
	} else if ( strstr( (char *)ds.rxBuff, "read" ) != NULL ) {
		read_data_from_flash();
		printf("udata.pswd: %s\r\n", udata.wpswd);
		printf("udata.ssid: %s\r\n", udata.wssid);
		printf("udata.tcpUrl: %s\r\n", udata.tcpurl);
		printf("udata.tcpPort: %d\r\n", udata.tcpport);
		
	} else if ( strstr( (char *)ds.rxBuff, "write1" ) != NULL ) {
		sprintf(udata.wpswd, "pswd001");
		sprintf(udata.wssid, "ssid001");
		sprintf(udata.tcpurl, "www.xiaojumao.com");
		udata.tcpport = 12345;
		write_data_into_flash();
		printf("write1 ok\r\n");
	
	} else if ( strstr( (char *)ds.rxBuff, "write2" ) != NULL ) {
		sprintf(udata.wpswd, "aaaaab2");
		sprintf(udata.wssid, "fdsagfds2");
		sprintf(udata.tcpurl, "www.baidu.com");
		udata.tcpport = 567;
		write_data_into_flash();
		printf("write2 ok\r\n");
		
	} else if ( strstr( (char *)ds.rxBuff, "esp:" ) != NULL ) {
		HAL_UART_Transmit( &huart1, &ds.rxBuff[4], ds.len-4, 1000 );
		
	} else if ( strstr( (char *)ds.rxBuff, "nled_on" ) != NULL ) {
		led_nrf_flicker_on();
		
	} else if ( strstr( (char *)ds.rxBuff, "cled_on" ) != NULL ) {
		led_con_flicker_on();
		
	} else if ( strstr( (char *)ds.rxBuff, "nled_off" ) != NULL ) {
		led_nrf_flicker_off(0);
		
	} else if ( strstr( (char *)ds.rxBuff, "cled_off" ) != NULL ) {
		led_con_flicker_off(0);
		
	} else if ( strstr( (char *)ds.rxBuff, "flashr" ) != NULL ) {
		read_data_from_w25qFlash();
		
	} else if ( strstr( (char *)ds.rxBuff, "flashw" ) != NULL ) {
		write_data_to_w25qFlash();
		printf("write ok\r\n");
		
	} else if ( strstr( (char *)ds.rxBuff, "nrfinit" ) != NULL ) {
		nrf_init();
		printf("nrf_init ok\r\n");
				
	} else if ( strstr( (char *)ds.rxBuff, "nled_speed:" ) != NULL ) {
		if ( led_nrf_speed_set(ds.rxBuff[strlen("nled_speed:")]-'0') == 0 ) {
			printf("nled_speed设置成功\r\n");
		} else {
			printf("nled_speed设置失败\r\n");
		}
		
	} else if ( strstr( (char *)ds.rxBuff, "cled_speed:" ) != NULL ) {
		if ( led_con_speed_set(ds.rxBuff[strlen("cled_speed:")]-'0') == 0 ) {
			printf("cled_speed设置成功\r\n");
		} else {
			printf("cled_speed设置失败\r\n");
		}
		
	} else if ( strstr( (char *)ds.rxBuff, "nrfaddr" ) != NULL ) {
		nrf_str.rxBuf[0] = 0;
		nrf_str.rxBuf[4] = 0xF0;
		nrf_str.rxBuf[5] = 0xF1;
		nrf_str.rxBuf[6] = 0xF2;
		nrf_str.rxBuf[7] = 0xF0;
		if ( !get_nrfaddr_by_deviceId(nrf_str.rxBuf[0]) ||
						nrf_str.rxBuf[4]!=nrf_str.TS_txAddr[0] || nrf_str.rxBuf[5]!=nrf_str.TS_txAddr[1] ||
						nrf_str.rxBuf[6]!=nrf_str.TS_txAddr[2] || nrf_str.rxBuf[7]!=nrf_str.TS_txAddr[3] ) {
			insert_nrfaddr( nrf_str.rxBuf[0] );
			printf("insert ok\r\n");
		} else {
			printf("addr is : 0x%.2X 0x%.2X 0x%.2X 0x%.2X\r\n",
				nrf_str.TS_txAddr[0], nrf_str.TS_txAddr[1], nrf_str.TS_txAddr[2], nrf_str.TS_txAddr[3] );
		}	
		
		
	} else if ( strstr( (char *)ds.rxBuff, "查询内存" ) != NULL ) {
	
#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
		printf("\r\n------ 单个任务堆栈的历史最小内存 总大小 / 历史最小内存 start ------\r\n");
		if ( debugTaskHandle != NULL ) printf("%d / %ld   debugTaskHandle\r\n", debugTaskSize, uxTaskGetStackHighWaterMark(debugTaskHandle) );
		if ( wifi_control_taskHandle != NULL ) printf("%d / %ld   wifi_control_taskHandle\r\n", wifi_control_taskSize, uxTaskGetStackHighWaterMark(wifi_control_taskHandle) );
		//if ( data_task_handle != NULL ) printf("%d / %ld   data_task_handle\r\n", data_taskSize, uxTaskGetStackHighWaterMark(data_task_handle) );
		if ( debugTaskHandle != NULL ) printf("%d / %ld   debugTaskHandle\r\n", debugTaskSize, uxTaskGetStackHighWaterMark(debugTaskHandle) );
		if ( nrf_control_taskHandle != NULL ) printf("%d / %ld   nrf_control_taskHandle\r\n", nrf_control_taskSize, uxTaskGetStackHighWaterMark(nrf_control_taskHandle) );
		if ( time_task_handle != NULL ) printf("%d / %ld  time_task_handle\r\n", time_taskSize ,uxTaskGetStackHighWaterMark(time_task_handle));
		printf("内存剩余：%d Byte 历史最小内存剩余：%d Byte\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
		printf("------ 单个任务堆栈的历史最小内存 end ------\r\n");
#else
		printf("\r\n------ 内存剩余：%d Byte 历史最小内存剩余：%d Byte\r\n\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
#endif
	}
//	else if ( strstr( (char *)ds.rxBuff, "clear reg" ) != NULL ) {
//		SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xf0 );//0xFF空指令
//		printf("clear reg ok\r\n");

//	} else if ( strstr( (char *)ds.rxBuff, "clear fifo" ) != NULL ) {
//		SPI_RW_Reg( FLUSH_RX,NOP );
//		printf("clear fifo ok\r\n");
//
//	} else if ( strstr( (char *)ds.rxBuff, "reg" ) != NULL ) {
//		uint8_t sta = SPI_RW_Reg(NRF_READ_REG + STATUS, 0xff );//0xFF空指令
//		printf("sta: %02X\r\n", sta);
//
//	} else if ( strstr( (char *)ds.rxBuff, "gpio" ) != NULL ) {
//		uint8_t temp = HAL_GPIO_ReadPin( NRF_IRQ_GPIO_Port, NRF_IRQ_Pin );
//		printf("temp: %d\r\n", temp);
//
//	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}
