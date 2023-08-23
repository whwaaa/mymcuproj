#include "../Bsp/bsp_nrf.h"

uint8_t Long;
//uint8_t TX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //���ص�ַ 
//uint8_t RX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //���յ�ַ
uint8_t TX_ADDRESS[5] = {0xA3,0xA3,0xA3,0xA3,0xA3}; //���ص�ַ 
uint8_t RX_ADDRESS[5] = {0xA3,0xA3,0xA3,0xA3,0xA3}; //���յ�ַ
uint8_t tx_buf[]={"NRF24L01��ǿģʽ���Գɹ���\n"};
uint8_t rx_buf[224];

static uint8_t SPI2_RW_Reg(uint8_t reg, uint8_t value);
static uint8_t SPI2_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
static uint8_t SPI2_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);

uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value) {//��ȡ�Ĵ���
	uint8_t status; 
	CSN_Low(); 
	HAL_SPI_Transmit( &hspi1, &reg, 1, 100 );
	HAL_SPI_TransmitReceive( &hspi1, &value, &status, 1, 100 );
	CSN_High();
	return(status);
} 

static uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status; 
	CSN_Low();  
	HAL_SPI_TransmitReceive( &hspi1, &reg, &status, 1, 100 );
	HAL_SPI_Transmit( &hspi1, pBuf, uchars, 100 );
	CSN_High(); 
	return(status);  
}  

static uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status; 
	CSN_Low();   
	HAL_SPI_TransmitReceive( &hspi1, &reg, &status, 1, 100 );
	HAL_SPI_Receive( &hspi1, pBuf, uchars, 100);
	CSN_High(); 
	return(status); // return nRF24L01 status uchar 
}

void NRF_Init(void) {
	uint8_t status;
	vTaskDelay(100);
	CE_Low();
	status = SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);//RX,����ͨ��0,ʹ����ǿTM�Զ�ȷ�Ϲ���
	status = SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);//RX,ʹ�����ݹܵ�0;
	status = SPI_RW_Reg(NRF_WRITE_REG + SETUP_AW, 0x03);//�ܵ���ַ���5Byte
	status = SPI_RW_Reg(NRF_WRITE_REG + SETUP_RETR, 0x01);//�Զ��ط�ʱ��500us
	status = SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 0x02);//Fo = 2400 + RF_CH(MHz) 2400MHz ~ 2525MHz
	status = SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x06);//���书��0db,����1Mbps,
	//STATUS: SPI_RW_Reg(NRF_WRITE_REG + STATUS, 1<<4,5,6);//״̬�Ĵ���,�ж����
	//OBSERVE_TX:���ݰ���ʧ�Ĵ���(ֻ��)
	//RPD: �ز����(���ʼ��>-65dBm����ߵ�ƽ)
	status = SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);//�����ŵ���ַ
	status = SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5);//�����ŵ���ַ
	
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, 0x20);//��̬��Ч�غɳ���32�ֽ�
	//FIFO_STATUS: ��ȡFIFO�Ƿ��������״̬��
	//status = SPI_RW_Reg(NRF_WRITE_REG + DYNPD, 0x01);//ͨ��0,ʹ�ܶ�̬�غɳ���
	//status = SPI_RW_Reg(NRF_WRITE_REG + FEATURE, 0x06);//ʹ�ܶ�̬����EN_DPL=1,������Ч�غ�EN_ACK_PAY=1,NOACK=0
	//0	0	0		1												1				0							1					1			
	//				MAX_RT(����ط����ж�)		EN_CRC	CRC(1Byte)		POWER_UP	PRX 
	status = SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x3B);//����TX,MAX_RT�ж�,CRC1Byte,RXģʽ
	SPI_RW_Reg(FLUSH_TX,NOP);//���FIFO
	SPI_RW_Reg(FLUSH_RX,NOP);//���FIFO
	SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xF0);
	CE_High();
}
void nrf_receive_data(void) {
	uint8_t sta,temp;
	sta = SPI_RW_Reg(NRF_READ_REG + STATUS, NOP);//0xFF��ָ��
	if ( sta & RX_DR ) {
		SPI_Read_Buf(RD_RX_PLOAD, rx_buf, RX_PLOAD_WIDTH);
		printf("%s\n",rx_buf);
		//SPI_RW_Reg(FLUSH_RX,NOP);
		SPI_RW_Reg(NRF_WRITE_REG + STATUS, sta);
	}
	
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void Tx_Mode(uint8_t temp) {
	CE_Low();
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x1A);//TXģʽ,POWER_UP=1,CRC1byte,EN_CRC,����ط����ж�
	SPI_Write_Buf(WR_TX_PLOAD , (tx_buf + temp) , 32);
	CE_High();
}
void Rx_Mode(void) {
	CE_Low(); // chip enable
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, 5);  // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);               	    	// ʹ�ܽ���ͨ��0�Զ�Ӧ��
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);           			// ʹ�ܽ���ͨ��0
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);                 			// ѡ����Ƶͨ��0x40
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);  			// ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);            			// ���ݴ�����2Mbps�����书��0dBm���������Ŵ�������(nRF24L01+���Ը�λ��
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);              			// CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
	SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xff);  						// ������е��жϱ�־λ
	CE_High(); 
}

void CleanStatus(uint8_t temp) {// �����־λ
	CSN_Low(); 
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,temp); 
	CSN_High();
}
void CleanIT(void) {
	CE_Low(); // chip enable 
	CSN_High(); // Spi disable 
	//SCK = 0; // Spi clock line init high 
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5); 
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);  
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x07); 
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0xFf); 
	CE_High();
}

void SendDatAndSuccess(void) {// ������ɲ���
	uint8_t a,b;
	a = Long / 32;
	b = Long % 32;
	//delay(0xFFFF);
	vTaskDelay(1000);
	CE_Low();
	SPI_RW_Reg(FLUSH_TX,NOP);
	CleanStatus(0xff);
	if((a==0&&b!=0)||(a==1&&b==0))	//	1ҳ��д��
	{
		NRF_Init();
		Tx_Mode(0);
		Long = 0;
	}
	else if(a>1)	// 1ҳд����
	{
		static uint8_t temp=0;
		if(temp<a)
		{
			NRF_Init();
			Tx_Mode(32*temp);
			temp++;
		}
		if(b!=0&&temp==a)
		{
			NRF_Init();
			Tx_Mode(32*temp+b);
			Long = 0;
		}
		if(temp>=a)
		{
			Long = 0;
		}
	}
	if(Long==0&&a==0&&b==0)
	{
		printf("          ------�յ�Ӧ�𣬷��ͳɹ�------\n");
		CE_Low();
		SPI_RW_Reg(FLUSH_TX,NOP);
		CleanStatus(0xff);
		CleanIT();
		NRF_Init();
		Rx_Mode();
	}
}
void SendMax(void) {// ���ʹ���������
	CE_Low();
	SPI_RW_Reg(FLUSH_TX,NOP);
	CleanStatus(0xff);
	printf("          ------Ӧ��ʱ������ʧ��------\n");
	CleanIT();
	NRF_Init();
	Rx_Mode();
}

static uint8_t SPI2_RW_Reg(uint8_t reg, uint8_t value) {// ��д���� 
	uint8_t status; 
	CSN2_Low(); 
	HAL_SPI_Transmit( &hspi2, &reg, 1, 100 );
	HAL_SPI_TransmitReceive( &hspi2, &value, &status, 1, 100 );
	CSN2_High();
	return(status);
} 
static uint8_t SPI2_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status;
	CSN2_Low();
	HAL_SPI_TransmitReceive( &hspi2, &reg, &status, 1, 100 );
	HAL_SPI_Transmit( &hspi2, pBuf, uchars, 100 );
	CSN2_High();
	return(status);
} 
static uint8_t SPI2_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status; 
	CSN2_Low();   
	HAL_SPI_TransmitReceive( &hspi2, &reg, &status, 1, 100 );
	HAL_SPI_Receive( &hspi2, pBuf, uchars, 100);
	CSN2_High(); 
	return(status); // return nRF24L01 status uchar 
}
void NRF2_Init(void) {
	uint8_t status;
	vTaskDelay(100);
	CE2_Low();
	status = SPI2_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);//RX,����ͨ��0,ʹ����ǿTM�Զ�ȷ�Ϲ���
	status = SPI2_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);//RX,ʹ�����ݹܵ�0;
	status = SPI2_RW_Reg(NRF_WRITE_REG + SETUP_AW, 0x03);//�ܵ���ַ���5Byte
	status = SPI2_RW_Reg(NRF_WRITE_REG + SETUP_RETR, 0x01);//�Զ��ط�ʱ��500us
	status = SPI2_RW_Reg(NRF_WRITE_REG + RF_CH, 0x02);//Fo = 2400 + RF_CH(MHz) 2400MHz ~ 2525MHz
	status = SPI2_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x06);//���书��0db,����1Mbps,
	//STATUS: SPI2_RW_Reg(NRF_WRITE_REG + STATUS, 1<<4,5,6);//״̬�Ĵ���,�ж����
	//OBSERVE_TX:���ݰ���ʧ�Ĵ���(ֻ��)
	//RPD: �ز����(���ʼ��>-65dBm����ߵ�ƽ)
	status = SPI2_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);//�����ŵ���ַ
	status = SPI2_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5);//�����ŵ���ַ
	
	SPI2_Read_Buf( TX_ADDR, rx_buf, 5 );
	
	SPI2_RW_Reg(NRF_WRITE_REG + RX_PW_P0, 0x20);//��̬��Ч�غɳ���32�ֽ�
	//FIFO_STATUS: ��ȡFIFO�Ƿ��������״̬��
	//status = SPI2_RW_Reg(NRF_WRITE_REG + DYNPD, 0x01);//ͨ��0,ʹ�ܶ�̬�غɳ���
	//status = SPI2_RW_Reg(NRF_WRITE_REG + FEATURE, 0x06);//ʹ�ܶ�̬����EN_DPL=1,������Ч�غ�EN_ACK_PAY=1,NOACK=0
	//0	0	0		1												1				0							1					1			
	//				MAX_RT(����ط����ж�)		EN_CRC	CRC(1Byte)		POWER_UP	PRX 
	status = SPI2_RW_Reg(NRF_WRITE_REG + CONFIG, 0x3A);//����TX,MAX_RT�ж�,CRC1Byte,TXģʽ
	SPI_RW_Reg(FLUSH_TX,NOP);//���FIFO
	SPI_RW_Reg(FLUSH_RX,NOP);//���FIFO
	CE2_High();
}

void Tx2_Mode( void ) {
	SPI2_Write_Buf(WR_TX_PLOAD , tx_buf , 32);
}
