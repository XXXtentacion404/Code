//
// Created by 最帅五岁半 on 2023/3/31.
//
#include "stdio.h"
#include "usart.h"
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
/* USER CODE BEGIN Private defines */


#define BUFFER_SIZE  100
extern  volatile uint8_t rx_len ;  //接收一帧数据的长度
extern volatile uint8_t recv_end_flag; //一帧数据接收完成标志
extern uint8_t rx_buffer[100];  //接收数据缓存数组

#ifndef TEMPLATE_UART_H
#define TEMPLATE_UART_H

#endif //TEMPLATE_UART_H
