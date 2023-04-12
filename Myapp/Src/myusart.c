/**
*********************************************************************
*********
* @project_name :myproject
* @file : myDmaUart.c
* @author : zen3
* @brief : None
* @attention : None
* @date : 2023/3/29
*********************************************************************
*********
*/
//

#include <string.h>
#include "myDmaUart.h"
#include "usart.h"
#include "UART.h"

//此函数放在串口中断中 it.c文件中
void It_cFun(DMA_HandleTypeDef *hdma_usart1_rx)
{

    uint32_t tmp_flag = 0;
    uint32_t temp;
    tmp_flag =__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE); //获取IDLE标志位
    if((tmp_flag != RESET))//idle标志被置位
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);//清除标志位
        //temp = huart1.Instance->SR;  //清除状态寄存器SR,读取SR寄存器可以实现清除SR寄存器的功能
        //temp = huart1.Instance->DR; //读取数据寄存器中的数据
        //这两句和上面那句等效
        HAL_UART_DMAStop(&huart1); //  停止DMA传输，防止
        temp  =  __HAL_DMA_GET_COUNTER(hdma_usart1_rx);// 获取DMA中未传输的数据个数
        //temp  = hdma_usart1_rx.Instance->NDTR;// 读取NDTR寄存器，获取DMA中未传输的数据个数，
        rx_len =  BUFFER_SIZE - temp; //总计数减去未传输的数据个数，得到已经接收的数据个数
        recv_end_flag = 1;	// 接受完成标志位置1
    }
}
void Dma_Usart_Init()
{
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能IDLE中断

//DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
    HAL_UART_Receive_DMA(&huart1,rx_buffer,BUFFER_SIZE);
}

void DMA_Usart_Send(uint8_t *buf,uint8_t len)//中间层
{
    if(HAL_UART_Transmit_DMA(&huart1, buf,len)!= HAL_OK)   //判断是否发送正常，如果出现异常则进入异常中断函数
    {
        Error_Handler();
    }

}
void Usart_Send(uint8_t *Data,uint8_t len)
{
    if(HAL_UART_Transmit(&huart1,Data, strlen(Data),100)!=HAL_OK)
    {
        Error_Handler();
    }
}
void DMA_Usart1_Read(uint8_t *Data,uint8_t len)//中间层
{
    HAL_UART_Receive_DMA(&huart1,Data,len);//重新打开DMA接收
}

void DMA_AcceptAnyLenth()
{
    if(recv_end_flag == 1)
    {


        DMA_Usart_Send(rx_buffer, rx_len);
        rx_len = 0;//清除计数
        recv_end_flag = 0;//清除接收结束标志位
//
        memset(rx_buffer,0,rx_len);
    }
    HAL_UART_Receive_DMA(&huart1,rx_buffer,BUFFER_SIZE);//重新打开DMA接收
}

void VofaSend_Int(uint16_t* data,uint16_t number)
{
    uint8_t title[]={"The value Is:\n"};
    char tempData[64]={0};
    int size=number;
    Usart_Send((uint8_t *)title, sizeof(title));
    for (int i = 0; i <size ; ++i) {
        sprintf(tempData,"%d\n",data[i]);
        Usart_Send((uint8_t *)tempData, sizeof(tempData));
    }

}

/* data:输入的数组
 * number：输入数组的大小
 * mode：1：输出电压小数点
 *       0：输出未转换的数字*/
void VofaSend_Float(float * data,uint16_t number,int mode) {
    uint8_t title[] = {"The value Is:\n"};
    char tempData[64] = {0};
    int size = number;
    Usart_Send((uint8_t *) title, sizeof(title));
    if (mode) {
        for (int j = 0; j < number; ++j) {
            sprintf(tempData, "%f\n", data[j] * 3.3 / 4096);
            Usart_Send(tempData, size);
        }
    } else {
        for (int j = 0; j < number; ++j) {
            sprintf(tempData, "%f\n", data[j]);
            Usart_Send(tempData, size);
        }
    }
}