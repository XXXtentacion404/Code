#include "vofa+.h"
#include "All_head.h"
#include "usart.h"
//extern uint8_t bluteooth_receivr_Data[7]; 
void vofa_send(float*data,uint8_t number)
{
    uint16_t i= 4000;
    uint8_t *tempData,size;
		size = (number+1)*sizeof(float);
    tempData = (uint8_t*)malloc(size);
		memset(tempData, 0, size);//��ʼ����ÿ��Ԫ�ض�Ϊ��
	  memcpy(tempData, (uint8_t *)data, number*sizeof(float)); //ͨ��������������������
	  //vofa+��֡β
	  *(tempData+(size-4))=0x00;
		*(tempData+(size-3))=0x00;
		*(tempData+(size-2))=0x80;
		*(tempData+(size-1))=0x7f;
	HAL_UART_Transmit_DMA(&huart1,tempData,size);
	while(i--);
	free(tempData);//�ͷ��ڴ�
}