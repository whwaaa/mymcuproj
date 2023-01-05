#ifndef __BATTERY_INFO__H
#define __BATTERY_INFO__H

#include "main.h"

#define BATT_SIZE							sizeof(BATT)


void battParamInit( void );

//��ذ汾��
typedef struct {
	char hw[12];
	char boot[7];
	char app[7];
	char lastapp[7];//����app
} battver;

//��س����Ϣ
typedef struct {//size:800
	//char sn[15];  //���SN
	uint8_t power;	 //����
	float temperature;//�¶�
	uint8_t cycles;//ѭ������
	uint16_t capacity;//�������
	float batteryVoltage;//��ص�ѹ
	float cellVoltage;//��ص�ѹ
	//char imgurl[20];//���ͼƬ
	
} chargerBatInfo;

//���������Ϣ
typedef struct {
	uint8_t syncChargeNum;//ͬʱ�������
	uint8_t chargeMode;//���ģʽ
	battver version;//��ذ汾��Ϣ
	chargerBatInfo cbInfo[13];//��س����Ϣ
} chargeLayer;


typedef struct BATTERYINFO {

	char sn[20];//��ع�SN
	battver version;//����汾��Ϣ
	
	uint8_t serverIp[4];//������ip��ַ
	uint16_t serverPort;//�������˿ں�
	
	chargeLayer cLayer[6];//���������Ϣ//size:800

} BATT;


extern BATT batt;





#endif

