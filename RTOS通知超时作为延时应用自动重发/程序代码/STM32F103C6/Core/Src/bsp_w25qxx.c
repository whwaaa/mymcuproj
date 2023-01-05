#include "bsp_w25qxx.h"
#include "spi.h"
#include "usart.h"
#include <stdio.h>
#include "stm32f1xx.h"
//#include "FreeRTOS.h"
//#include "task.h"


//����ʹ��
static void W25Qx_Enable() {
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET); // Chip select
}

//����ʧ��
static void W25Qx_Disable() {
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET); // Chip disselect
}
 
//����1���ֽ�����
static void sip_SendByte(uint8_t byte) {
	HAL_SPI_Transmit(&hspi1,(uint8_t*) &byte,1,HAL_MAX_DELAY);
}

//����1���ֽ�����
static uint8_t spi_ReceiveByte() {
	uint8_t byte;
	HAL_SPI_Receive(&hspi1,(uint8_t*) &byte, 1, HAL_MAX_DELAY);
	return byte;
}

/**
 * @brief    SPI����ָ�����ȵ�����
 * @param    buf  ���� �������ݻ������׵�ַ
 * @param    size ���� Ҫ�������ݵ��ֽ���
 * @retval   �ɹ�����HAL_OK
 */
static HAL_StatusTypeDef SPI_Transmit(uint8_t* send_buf, uint16_t size)
{
   return HAL_SPI_Transmit(&hspi1, send_buf, size, 100);
}

/**
 * @brief   SPI����ָ�����ȵ�����
 * @param   buf  ���� �������ݻ������׵�ַ
 * @param   size ���� Ҫ�������ݵ��ֽ���
 * @retval  �ɹ�����HAL_OK
 */
static HAL_StatusTypeDef SPI_Receive(uint8_t* recv_buf, uint16_t size)
{
   return HAL_SPI_Receive(&hspi1, recv_buf, size, 100);
}

//SPI�������ݵ�ͬʱ����ָ�����ȵ�����
static HAL_StatusTypeDef SPI_TransmitReceive(uint8_t* send_buf, uint8_t* recv_buf, uint16_t size)
{
   return HAL_SPI_TransmitReceive(&hspi1, send_buf, recv_buf, size, 100);
}

//W25Qxдʹ��,��WEL��1 
static void W25Qx_Write_Enable()   
{
	W25Qx_Enable();                            //ʹ������   
  sip_SendByte(W25X_WriteEnable); 
	W25Qx_Disable();                            //ȡ��Ƭѡ     	      
}

//W25Qxдʧ��,��WEL��0 
static void W25Qx_Write_Disable()   
{
	W25Qx_Enable();                            //ʹ������   
  sip_SendByte(W25X_WriteDisable); 
	W25Qx_Disable();                            //ȡ��Ƭѡ     	      
}
 
//��ȡ�Ĵ���״̬
static uint8_t W25Qx_ReadSR(void)   
{  
	uint8_t byte=0;   
	W25Qx_Enable();                         //ʹ������   
	sip_SendByte(W25X_ReadStatusReg);    		//���Ͷ�ȡ״̬�Ĵ�������
	byte = spi_ReceiveByte();             	//��ȡһ���ֽ�
	W25Qx_Disable();                        //ȡ��Ƭѡ     
	return byte;   
} 
 
//�ȴ�����
void W25Qx_Wait_Busy()   
{   
	while((W25Qx_ReadSR()&WIP_Flag)==WIP_Flag);   // �ȴ�BUSYλ���
}
 
//������ַ���ڵ�һ������
static void Erase_one_Sector(uint32_t Address)
{
	W25Qx_Write_Enable();    //SET WEL
	W25Qx_Wait_Busy(); 		
	W25Qx_Enable();          //ʹ������ 
	sip_SendByte(W25X_SectorErase);      //������������ָ�� 
	sip_SendByte((uint8_t)((Address)>>16));  //����24bit��ַ    
	sip_SendByte((uint8_t)((Address)>>8));   
	sip_SendByte((uint8_t)Address);  
	W25Qx_Disable();         //ȡ��Ƭѡ     	      
	W25Qx_Wait_Busy(); 			 //�ȴ��������
}
 
//������ַ���ڵ�����
void Erase_Write_data_Sector(uint32_t Address,uint32_t Write_data_NUM) {
	//�ܹ�4096������
	//���� д�����ݿ�ʼ�ĵ�ַ + Ҫд�����ݸ���������ַ ����������	
	uint16_t Star_Sector,End_Sector,Num_Sector;
	Star_Sector = Address / 4096;						//����д�뿪ʼ������
	End_Sector = (Address + Write_data_NUM) / 4096;		//����д�����������
	Num_Sector = End_Sector - Star_Sector;  			//����д��缸������
 
	//��ʼ��������
	for(uint16_t i=0;i <= Num_Sector;i++)
	{
		Erase_one_Sector(Address);
		Address += 4095;
	}
 
}
 
//��������оƬ
void Erase_W25Qx_Chip(void){  
	W25Qx_WAKEUP();
	W25Qx_Write_Enable(); //SET WEL 
	W25Qx_Wait_Busy();   
	W25Qx_Enable();       //ʹ������   
	sip_SendByte(W25X_ChipErase);//����Ƭ��������  
	W25Qx_Disable();      //ȡ��Ƭѡ     	      
	W25Qx_Wait_Busy();   	//�ȴ�оƬ��������
} 
 

/**
	* @brief  ��ȡW25Qx����
	* @param 	 pBuffer���洢�������ݵ�ָ��
	* @param   ReadAddr����ȡ��ַ
	* @param   NumByteToRead����ȡ���ݳ���
	* @retval  ��
	*/
void W25Qx_Read_data(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i=0;   										    
	W25Qx_Enable();             //ʹ������   
	sip_SendByte(W25X_ReadData);         //���Ͷ�ȡ����   
	sip_SendByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ    
	sip_SendByte((uint8_t)((ReadAddr)>>8));   
	sip_SendByte((uint8_t)ReadAddr);   
	for(;i<NumByteToRead;i++) { 
		pBuffer[i]=spi_ReceiveByte();   //ѭ������  
	}
	W25Qx_Disable(); 				    	      
}

 
//д�֣�һ�����һҳ
static void W25Qx_Write_Word(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i; 
 
	W25Qx_Write_Enable();                  //SET WEL
	W25Qx_Enable();                        //ʹ������
	sip_SendByte(W25X_PageProgram);
	sip_SendByte((uint8_t)((WriteAddr) >> 16)); //д���Ŀ���ַ   
	sip_SendByte((uint8_t)((WriteAddr) >> 8));   
	sip_SendByte((uint8_t)WriteAddr);   
	for (i=0; i<NumByteToWrite; i++) {
		sip_SendByte(*pBuffer++);//ѭ��д���ֽ�����  
	}
	W25Qx_Disable();
	W25Qx_Wait_Busy();		//д��֮����Ҫ�ȴ�оƬ�����ꡣ
}
 
//��ҳд������
void W25Qx_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t Word_remain;
	Word_remain=256 - WriteAddr % 256; 	//��λҳʣ�������	
	
	if(NumByteToWrite <= Word_remain)
		Word_remain=NumByteToWrite;		//��λҳ��һ��д��
	while(1)
	{
		W25Qx_Write_Word(pBuffer,WriteAddr,Word_remain);	
		if(NumByteToWrite==Word_remain)
		{
			break;	//�ж�д��� break
		}	
	 	else //ûд�꣬��ҳ��
		{
			pBuffer += Word_remain;		//ֱ����Ƶ�ҳ��д����
			WriteAddr += Word_remain;	
			NumByteToWrite -= Word_remain;	//��ȥ�Ѿ�д���˵�����
			if(NumByteToWrite>256)
				Word_remain=256; 		//һ�ο���д��256����
			else 
				Word_remain=NumByteToWrite; 	//����256������
		}
	}	    
} 

 
 /**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
uint32_t W25Qx_ReadJedecDeviceID(void)
{
  uint32_t Temp;
  W25Qx_Enable();
  sip_SendByte(W25X_JedecDeviceID);
  Temp = (uint32_t)spi_ReceiveByte()<<16;
  Temp |= (uint32_t)spi_ReceiveByte()<<8;
  Temp |= spi_ReceiveByte();
  W25Qx_Disable();
  return Temp;
}

/**
  * @brief  ��ȡW25Qx Device ID
  * @param 	��
  * @retval W25Qx Device ID
  */
uint32_t W25Qx_ReadDeviceID(void)
{
  uint32_t Temp = 0;
  W25Qx_Enable();
  sip_SendByte(W25X_DeviceID);
  sip_SendByte(Dummy_Byte);
  sip_SendByte(Dummy_Byte);
  sip_SendByte(Dummy_Byte);
  Temp = spi_ReceiveByte();
  W25Qx_Disable();
  return Temp;
}

//�������ģʽ
void W25Qx_PowerDown(void){ 
  W25Qx_Enable();
  sip_SendByte(W25X_PowerDown);
  W25Qx_Disable();
}   

//����
void W25Qx_WAKEUP(void){
  W25Qx_Enable();
  sip_SendByte(W25X_ReleasePowerDown);
  W25Qx_Disable(); 
}   

