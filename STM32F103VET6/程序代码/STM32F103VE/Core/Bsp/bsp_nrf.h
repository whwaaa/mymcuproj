#ifndef __BSP_NRF__H
#define __BSP_NRF__H
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"

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
#define CD 0x09 				// ��ַ��� 
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

/*-----------------------------��������------------------------------------*/
//SPI1_GPIO����
#define SPI1_RCC						RCC_APB2PeriphClockCmd
#define SPI1_CLK						RCC_APB2Periph_SPI1
#define SPI_PORT						SPI1

// \SCK MISO \MOSI \CSN
#define SPI_GPIO_RCC				RCC_APB2PeriphClockCmd
#define SPI_GPIO_CLK				RCC_APB2Periph_GPIOA
#define SPI1_GPIO_PORT			GPIOA
#define SPI1_SCK_GPIOx			GPIO_Pin_5
#define SPI1_MISO_GPIOx			GPIO_Pin_6
#define SPI1_MOSI_GPIOx			GPIO_Pin_7
#define SPI_CSN_RCC					RCC_APB2PeriphClockCmd
#define SPI_CSN_CLK					RCC_APB2Periph_GPIOC
#define SPI_CSN_PORT				GPIOC
#define SPI_CSN_GPIOx				GPIO_Pin_6 // ָ����
//#define SPI_CSN_GPIOx				GPIO_Pin_4 // ��Сϵͳ��
// \IRQ \CE
#define NRF_IRQ_PORT				GPIOC			 // ָ����
#define NRF_IRQ_GPIOx				GPIO_Pin_4 // ָ����
#define IRQ_PortSourceGPIOx GPIO_PortSourceGPIOC // ָ����
#define IRQ_PinSource_Pin		GPIO_PinSource4	// ָ����
#define IRQ_EXTI_Lines			EXTI_Line4	// ָ����
#define NRF_IRQHandler			EXTI4_IRQHandler  // ָ����
#define NRF_CE_PORT					GPIOC			  // ָ����
#define NRF_CE_GPIOx				GPIO_Pin_5  // ָ���� 

//#define NRF_IRQ_PORT				GPIOC			 // ��Сϵͳ��
//#define NRF_IRQ_GPIOx				GPIO_Pin_4 // ��Сϵͳ��
//#define IRQ_PortSourceGPIOx GPIO_PortSourceGPIOC // ��Сϵͳ��
//#define IRQ_PinSource_Pin		GPIO_PinSource4	// ��Сϵͳ��
//#define IRQ_EXTI_Lines			EXTI_Line4	// ��Сϵͳ��
//#define NRF_IRQn						TIM4_IRQn		// ��Сϵͳ��
//#define NRF_IRQHandler			EXTI4_IRQHandler  // ��Сϵͳ��
//#define NRF_CE_PORT				GPIOA			 // ��Сϵͳ��
//#define NRF_CE_GPIOx			GPIO_Pin_4 // ��Сϵͳ��


/*----------------------------λ����-----------------------------------*/
#define CSN_Low						GPIOC->BRR = GPIO_Pin_6;
#define CSN_High					GPIOC->BSRR = GPIO_Pin_6;
#define SCK_Low						GPIOA->BRR = GPIO_Pin_5;
#define SCK_High					GPIOA->BSRR = GPIO_Pin_5;
#define CE_Low						GPIOC->BRR = GPIO_Pin_5;
#define CE_High						GPIOC->BSRR = GPIO_Pin_5;
#define IRQ_High					GPIOC->BRR = GPIO_Pin_4;
#define IRQ_Low						GPIOC->BSRR = GPIO_Pin_4;
#define TimeOut											10000

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
