#ifndef __BSP_NRF__H
#define __BSP_NRF__H

#include "../common.h"

//********************NRF24L01*********************** ************** 
#define TX_PLOAD_WIDTH 32  // 20 uints TX payload 
#define RX_PLOAD_WIDTH 32  // 20 uints TX payload  
//*****************NRF24L01 �� �� �� ָ �� ***************************
#define NRF_READ_REG 0x00  // ���Ĵ���ָ�� 
#define NRF_WRITE_REG 0x20 // д�Ĵ���ָ�� 
#define RD_RX_PLOAD 0x61   // ��ȡ��������ָ�� 
#define WR_TX_PLOAD 0xA0   // д��������ָ�� 
#define FLUSH_TX 0xE1 		 // ��ϴ���� FIFO ָ�� 
#define FLUSH_RX 0xE2 		 // ��ϴ���� FIFO ָ�� 
#define REUSE_TX_PL 0xE3 	 // �����ظ�װ������ָ�� 
#define NOP 0xFF 			     // ���� 
//******************SPI(nRF24L01) �� �� �� �� ַ *************** 
#define CONFIG 0x00     // �����շ�״̬��CRC У��ģʽ�Լ��շ�״̬��Ӧ��ʽ 
#define EN_AA 0x01      // �Զ�Ӧ�������� 
#define EN_RXADDR 0x02  // �����ŵ����� 
#define SETUP_AW 0x03   // �շ���ַ������� 
#define SETUP_RETR 0x04 // �Զ��ط��������� 
#define RF_CH 0x05 			// ����Ƶ������ 
#define RF_SETUP 0x06 	// �������ʡ����Ĺ������� 
#define STATUS 0x07 		// ״̬�Ĵ��� 
#define OBSERVE_TX 0x08 // ���ͼ�⹦�� 
#define RPD 0x09 				// �ز����(���ʼ��>-65dBm����ߵ�ƽ)
#define RX_ADDR_P0 0x0A // Ƶ�� 0 �������ݵ�ַ 
#define RX_ADDR_P1 0x0B // Ƶ�� 1 �������ݵ�ַ 
#define RX_ADDR_P2 0x0C // Ƶ�� 2 �������ݵ�ַ 
#define RX_ADDR_P3 0x0D // Ƶ�� 3 �������ݵ�ַ 
#define RX_ADDR_P4 0x0E // Ƶ�� 4 �������ݵ�ַ 
#define RX_ADDR_P5 0x0F // Ƶ�� 5 �������ݵ�ַ 
#define TX_ADDR 0x10 		// ���͵�ַ�Ĵ��� 
#define RX_PW_P0 0x11	  // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P1 0x12   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P2 0x13   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P3 0x14   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P4 0x15   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P5 0x16   // ����Ƶ�� 0 �������ݳ��� 
#define FIFO_STATUS 0x17// FIFO ջ��ջ��״̬�Ĵ������� 
#define DYNPD			0x1C	//��̬�غɳ���
#define FEATURE		0x1D	//��̬����,�Զ�ȷ��,�Զ�Ӧ��


/*----------------------------λ����-----------------------------------*/
#define CSN_Low()						NRF_CSN_GPIO_Port->BRR = NRF_CSN_Pin
#define CSN_High()					NRF_CSN_GPIO_Port->BSRR = NRF_CSN_Pin
#define CE_Low()						NRF_CE_GPIO_Port->BRR = NRF_CE_Pin
#define CE_High()						NRF_CE_GPIO_Port->BSRR = NRF_CE_Pin
#define IRQ_isHigh()			(NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin
#define IRQ_isLow()				!((NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin)


/*---------------------------Ӧ�ú���---------------------------------*/
void NRF_Init(void);
void Rx_Mode(void);
void NRF_RX_ITRPT(void);
void Tx_Mode(uint8_t temp);
void NRF_TX_ITRPT(void);
void CleanStatus(uint8_t temp); // �����־λ
uint8_t ReadStatus(void);	// ��ȡ��־λ
void ReadSuccess(void); // �������ݳɹ�����
void SendMax(void);	// ���ʹ���������
void SendDatAndSuccess(void);	// �������ݻ���ɹ�
//void TxBuf_TxBufx(uint8_t *TxBufs); //	��������
void USART_Receive(void);		// ���ڽ�������
void DisplayDat(void);	// ��ʾ��������
#endif /*__BSP_NRF__H*/
