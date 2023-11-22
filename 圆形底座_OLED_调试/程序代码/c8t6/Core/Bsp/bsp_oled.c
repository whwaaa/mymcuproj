#include "../Bsp/bsp_oled.h"
#include "../Bsp/OLED_Font.h"

void olde_Write_cmd( uint8_t cmd ) {
	HAL_I2C_Mem_Write( &hi2c1, oledAddr, cmdMode, 1, &cmd, 1, portMAX_DELAY);
//	uint8_t data[2];
//	data[0] = 0;
//	data[1] = cmd;
//	HAL_I2C_Master_Transmit( &hi2c1, 0x78, data, 2, 0x100 );
	
}

void olde_Write_data( uint8_t dat ) {
	HAL_I2C_Mem_Write( &hi2c1, oledAddr, datMode, 1, &dat, 1, portMAX_DELAY);
//	uint8_t data[2];
//	data[0] = 0x40;
//	data[1] = dat;
//	HAL_I2C_Master_Transmit( &hi2c1, 0x78, data, 2, 0x100 );
}

void oled_clear( void ) {
	for( int i=0; i<8; i++ ){
		olde_Write_cmd(0xB0 + i);//page0--page7
		//ÿ��page��0��
		olde_Write_cmd(0x00);
		olde_Write_cmd(0x10);
		//0��127�У�����д��0��ÿд�����ݣ��е�ַ�Զ�ƫ��
		for( int j=0; j<128; j++ ){
			olde_Write_data( 0 );
		}
	}
}

void oled_init( void ) {
//	olde_Write_cmd(0xAE);//--display off
//	olde_Write_cmd(0x00);//---set low column address
//	olde_Write_cmd(0x10);//---set high column address
//	olde_Write_cmd(0x40);//--set start line address
//	olde_Write_cmd(0xB0);//--set page address
//	olde_Write_cmd(0x81);// contract control
//	olde_Write_cmd(0xFF);//--128
//	olde_Write_cmd(0xA1);//set segment remap
//	olde_Write_cmd(0xA6);//--normal / reverse
//	olde_Write_cmd(0xA8);//--set multiplex ratio(1 to 64)
//	olde_Write_cmd(0x3F);//--1/32 duty
//	olde_Write_cmd(0xC8);//Com scan direction
//	olde_Write_cmd(0xD3);//-set display offset
//	olde_Write_cmd(0x00);//
//	olde_Write_cmd(0xD5);//set osc division
//	olde_Write_cmd(0x80);//
//	olde_Write_cmd(0xD8);//set area color mode off
//	olde_Write_cmd(0x05);//
//	olde_Write_cmd(0xD9);//Set Pre-Charge Period
//	olde_Write_cmd(0xF1);//
//	olde_Write_cmd(0xDA);//set com pin configuartion
//	olde_Write_cmd(0x12);//
//	olde_Write_cmd(0xDB);//set Vcomh
//	olde_Write_cmd(0x30);//
//	olde_Write_cmd(0x8D);//set charge pump enable
//	olde_Write_cmd(0x14);//
	uint32_t i, j;
	uint8_t pData = 0xFF;

	for (i = 0; i < 1000; i++) {//�ϵ���ʱ
		for (j = 0; j < 1000; j++)
			;
	}

	//OLED_I2C_Init(); //�˿ڳ�ʼ��
	printf( "strt:pData: %d", pData );
	//HAL_I2C_Mem_Read(&hi2c1, 0x78, 0, 1, &pData, 1, 0x100);
	HAL_I2C_Master_Receive( &hi2c1, 0x78, &pData, 1, 0x100 );
	printf( "  end:pData: %d\r\n", pData );
	//HAL_I2C_Master_Receive()

	olde_Write_cmd(0xAE); //�ر���ʾ
	olde_Write_cmd(0xD5); //������ʾʱ�ӷ�Ƶ��/����Ƶ��
	olde_Write_cmd(0x80);
	
	olde_Write_cmd(0xA8); //���ö�·������
	olde_Write_cmd(0x3F);

	olde_Write_cmd(0xD3); //������ʾƫ��
	olde_Write_cmd(0x34);
	
	olde_Write_cmd(0x40); //������ʾ��ʼ��

	olde_Write_cmd(0xA1); //�������ҷ���0xA1���� 0xA0���ҷ���

	olde_Write_cmd(0xC8); //�������·���0xC8���� 0xC0���·���

	olde_Write_cmd(0xDA); //����COM����Ӳ������
	olde_Write_cmd(0x12);

	olde_Write_cmd(0x81); //���öԱȶȿ���
	olde_Write_cmd(0xCF);

	olde_Write_cmd(0xD9); //����Ԥ�������
	olde_Write_cmd(0xF1);

	olde_Write_cmd(0xDB); //����VCOMHȡ��ѡ�񼶱�
	olde_Write_cmd(0x30);

	olde_Write_cmd(0xA4); //����������ʾ��/�ر�

	olde_Write_cmd(0xA6); //��������/��ת��ʾ

	olde_Write_cmd(0x8D); //���ó���
	olde_Write_cmd(0x14);

	olde_Write_cmd(0xAF); //������ʾ

	oled_clear(); // OLED����
}



/**
 * @brief  OLED���ù��λ��
 * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
 * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
 * @retval ��
 */
void OLED_SetCursor(uint8_t Y, uint8_t X) {
	olde_Write_cmd(0xB0 | Y);				 //����Yλ��
	olde_Write_cmd(0x10 | ((X & 0xF0) >> 4));//����Xλ�õ�4λ
	olde_Write_cmd(0x00 | (X & 0x0F));		 //����Xλ�ø�4λ
}

/**
 * @brief  OLED��ʾһ���ַ�
 * @param  Line ��λ�ã���Χ��1~4
 * @param  Column ��λ�ã���Χ��1~16
 * @param  Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
 * @retval ��
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); //���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		olde_Write_data(OLED_F8x16[Char - ' '][i]); //��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); //���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		olde_Write_data(OLED_F8x16[Char - ' '][i + 8]); //��ʾ�°벿������
	}
}

/**
 * @brief  OLED��ʾһ������
 * @param  chineseIdex Ҫ��ʾ��һ�����ֿ��е�����
 * @retval ��
 */
void OLED_ShowWord_3( uint8_t idx, uint8_t Line, uint8_t chineseIdex) {
	uint8_t i;
	OLED_SetCursor( Line*2, 40+idx*16 ); //���ù��λ�����ϰ벿��
	for ( i=0; i<16; i++ ) {
		olde_Write_data(OLED_F16x16[chineseIdex][i]); //��ʾ�ϰ벿������
	}
	OLED_SetCursor( Line*2+1, 40+idx*16 ); //���ù��λ�����ϰ벿��
	for ( i=0; i<16; i++ ) {
		olde_Write_data(OLED_F16x16[chineseIdex][i+16]); //��ʾ�ϰ벿������
	}
}

void oled_show_yinxiang( void ) {
	for ( int j=0; j<6; j++ ) {
		OLED_SetCursor( j, 27 );
		for ( int i=0; i<74; i++ ) {
			olde_Write_data(oled_yinxiang[j*74+i]);
		}
	}
}

void oled_show_pinzideng( void ) {
	for ( int j=0; j<5; j++ ) {
		OLED_SetCursor( j, 7 );
		for ( int i=0; i<113; i++ ) {
			olde_Write_data(oled_pinzideng[j*113+i]);
		}
	}
}

void oled_test( void ) {
	oled_init(); //��ʼ��Oled
//	for ( int j=0; j<8; j++ ) {
//		for ( int i=0; i<60; i++ ) {
//			OLED_SetCursor(j, 30+i );
//			olde_Write_data(0xFF);
//		}
//	}
	
//	8  0 0 0 0 0 0 0 0 
//	16*3 = 48
//	128 - 48 = 80
//	80 / 2 = 40
//	OLED_ShowWord_3(0, 0, 5);
//	OLED_ShowWord_3(1, 0, 6);
//	OLED_ShowWord_3(2, 0, 7);
	while (1) {
		oled_show_pinzideng();
		vTaskDelay(2000);
		oled_clear(); // OLED����
		
		oled_show_yinxiang();
		vTaskDelay(2000);
		oled_clear(); // OLED����
	}
}





















