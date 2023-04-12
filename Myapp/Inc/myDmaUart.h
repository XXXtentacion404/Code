////
//// Created by 最帅五岁半 on 2023/4/3.
//////
////#include "stdio.h"
////#include "dma.h"
////#include "usart.h"
////#ifndef TEMPLATE_MYDMAUART_H
////#define TEMPLATE_MYDMAUART_H
////
////#endif //TEMPLATE_MYDMAUART_H
////void It_cFun(DMA_HandleTypeDef *hdma_usart1_rx);
////void Dma_Usart_Init();
////void Usart_Send(uint8_t *Data,uint8_t len);
////void DMA_Usart1_Read(uint8_t *Data,uint8_t len);//中间层
////void DMA_AcceptAnyLenth();
////void VofaSend_Int(uint16_t* data,uint16_t number);
////void VofaSend_Float(float * data,uint16_t number,int mode);
///**
//*********************************************************************
//*********
//* @project_name :myproject
//* @file : myDmaUart.h
//* @author : zen3
//* @brief : None
//* @attention : None
//* @date : 2023/3/29
//*********************************************************************
//*********
//*/
////
//
//#ifndef MYPROJECT_MYDMAUART_H
//#define MYPROJECT_MYDMAUART_H
//#include "main.h"
//#include "stdio.h"
//#include "stdlib.h"
//#include "string.h"
//#define BUFFER_SIZE  100
////将这些变量放在it.c文件下声明
//extern  volatile uint8_t rx_len ;  //接收一帧数据的长度
//extern volatile uint8_t recv_end_flag; //一帧数据接收完成标志
//extern uint8_t rx_buffer[100];  //接收数据缓存数组
///*uint8_t rx_len;  //接收一帧数据的长度
//uint8_t recv_end_flag; //一帧数据接收完成标志
//uint8_t rx_buffer[100];  //接收数据缓存数组*/
//
//void VofaSend_Int(uint16_t* data,uint16_t number);
//void VofaSend_Float(float * data,uint16_t number,int mode);
//void It_cFun(DMA_HandleTypeDef *hdma_usart1_rx);
//void Dma_Usart_Init();
//void DMA_AcceptAnyLenth();
//void Usart_Send(uint8_t *Data,uint8_t len);
//void DMA_Usart_Send(uint8_t *buf,uint8_t len);
//void DMA_Usart1_Read(uint8_t *Data,uint8_t len);
//#endif //MYPROJECT_MYDMAUART_H
