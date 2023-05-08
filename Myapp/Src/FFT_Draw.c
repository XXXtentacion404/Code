#include "All_head.h"
//
extern float fft_inputbuf[FFT_LENGTH * 2];
extern float fft_outputbuf[FFT_LENGTH];
float fft_inputbuf2[FFT_LENGTH * 2];
float fft_outputbuf2[FFT_LENGTH];
volatile uint8_t dma_complete_flag = 0;

uint32_t max_pos;
uint32_t min_pos;

float32_t max_val2; // 波形电压的最大值，最小值，幅值
float wave[1024], wave1[1024];
uint32_t IC1Value,IC2Value=0;
float DutyCycle,Frequency;

//void HAL_ADC_ConvCpltCallback(&hadc1)
//{
//    dma_complete_flag = 1;
//}

void  AdcGetVal_dma(uint16_t *adcbuf, uint16_t size)
{
    HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adcbuf,size);
}
//void DMA_Usart_Send(uint8_t *buf,uint8_t len)//串口发送封装
//{
//    if(HAL_UART_Transmit_DMA(&huart1, buf,len)!= HAL_OK) //判断是否发送正常，如果出现异常则进入异常中断函数
//    {
//        Error_Handler();
//    }
//
//}
void Draw_wave(uint16_t *adcBuff,uint16_t size) {
    uint16_t past_vol = 0, pre_vol = 0;
    for (uint16_t x = 0; x < 799; x++) {
        pre_vol = adcBuff[x] * (3.3 / 4096) * 60;
        lcd_draw_line(x, 250, x, 480, BLACK);
        lcd_draw_bline(x, past_vol + 280, x + 1, pre_vol + 280, 1, YELLOW);
        past_vol = pre_vol;
    }
}
void Draw_fft(float32_t *fft_outputbuf,uint16_t size){
    for (int i = 290; i < 800; i++)//画出幅值
    {
        lcd_draw_line(i, 0, i, 229,    BLACK); //清屏
        lcd_draw_line(i, 230 - 100 * (fft_outputbuf[i - 290]), i + 1, 230 - 100 * (fft_outputbuf[i - 289]),BLUE);//画线
    }

}
void FFT_1024(uint16_t *adcBuff,uint16_t FFTLENGTH) {
    for (uint16_t i = 0; i < FFT_LENGTH; i++)
    {
//        printf("%d\n",adcBuff);
        fft_inputbuf[i * 2] = adcBuff[i] * 3.3 / 4095;//实部赋值，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
        fft_inputbuf[i * 2 + 1] = 0;//虚部赋值，固定为0
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1);
    arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);
    fft_outputbuf[0] /= FFT_LENGTH;
    for (int i = 1; i < FFT_LENGTH; i++)//输出各次谐波幅值
    {
        fft_outputbuf[i] /= 512;
    }
    for (int i = 0; i < 1024; ++i) {
        printf("%f\n",fft_outputbuf[i]);
    }
}

//
//void Show_phase_diff(uint16_t len,float32_t *fft_outputbuf1,float32_t*fft_outputbuf2) {

//    fft_outputbuf1[0] = 0;
//    fft_outputbuf2[0] = 0;
//    arm_max_f32(fft_outputbuf1, 1024, &Fmax, &max_pos1);  //求取基波频率 基波的幅度是频谱中最大的 基波频率=原波形频率
//    arm_max_f32(fft_outputbuf2, 1024, &max_val2, &max_pos2);
//
//    float32_t real_part1 = fft_inputbuf1[max_pos1 * 2];
//    float32_t imaginary_part1 = fft_inputbuf1[max_pos1 * 2 + 1];
//
//
//    float32_t real_part2 = fft_inputbuf2[max_pos2 * 2];
//    float32_t imaginary_part2 = fft_inputbuf2[max_pos2 * 2 + 1];
//
//
//    float32_t phase_diff =
//            (atan2f(imaginary_part1, real_part1)) - (atan2f(imaginary_part2, real_part2)); // 使用 atan2f 函数计算相位差
//    float32_t phase_diff_degrees = phase_diff * 180.0f / 3.14159265358979323846f;
//    char temp[64];
//    sprintf(temp, "%f", phase_diff_degrees);
//    if (phase_diff_degrees < 180 && phase_diff_degrees > 0 || phase_diff_degrees < 0 && phase_diff_degrees > -180) {
//        lcd_show_string(120, 168, 32, 10, 16, temp, RED);
//    }
//}
void Show_fre(uint16_t F,uint32_t len,uint16_t *adcBuff,float32_t *fft_outputbuf) {
    float Fmax, vmax, vmin, vpp;
    uint32_t max_pos;
    uint16_t Npoints;
    float RMS, fre;
    for (int i = 0; i < len; ++i) {
        wave[i] = adcBuff[i] * 3.3 / 4095; // 转换电压
    }
    arm_max_f32(wave, 1024, &vmax, &max_pos); //DSP库自带的求最值函数
    arm_min_f32(wave, 1024, &vmin, &min_pos);
    vpp = vmax - vmin;
    char temp[64];
    sprintf(temp, "%f", vpp);
    lcd_show_string(60, 88, 32, 10, 16, temp, RED);//VPP峰峰值
    sprintf(temp, "%f", vmax);
    lcd_show_string(60, 48, 32, 10, 16, temp, RED);//VPP峰峰值
    sprintf(temp, "%f", vmin);
    lcd_show_string(60, 68, 32, 10, 16, temp, RED);//VPP峰峰值
    lcd_show_xnum(60, 148, 1000, 10, 16, 0, RED);   //采样率
    fft_outputbuf[0] = 0;
    arm_max_f32(fft_outputbuf, 1024, &Fmax, &max_pos);  //求取基波频率 基波的幅度是频谱中最大的 基波频率=原波形频率
    if (max_pos != 0) {                                                 //第一组数字会出现0导致卡死
        fre = (float) F*10 / len * max_pos;
        sprintf(temp, "%f", fre);
        lcd_show_string(80, 108, 48, 10, 16, temp, RED);//VPP峰峰值
        Npoints = F / fre;
        for (int i = 0; i < len; ++i) {
            wave[i] = adcBuff[i] * 3.3 / 4095; // 转换电压
        }
        for (int i = 0; i < Npoints; i++) wave1[i] = wave[i];//取出一个周期的点存在wave1[i]中
        for (int i = 0; i < Npoints; i++) {
            wave1[i] = wave1[i] - vmin - vpp / 2;           //将波形平移到与时间轴对称的位置
            arm_rms_f32(wave1, Npoints,
                        &RMS); // DSP库封装好的求rms值函数 不再手动实现 原型： arm_rms_f32 (const float32_t *pSrc, uint32_t blockSize, float32_t *pResult)
            RMS = RMS / (vpp / 2);
        }
        lcd_show_float(100,128,RMS,16,BLUE);
//        if (RMS > 0.64 && RMS < 0.8)
//            lcd_show_string(100, 128, 200, 20, 16, "sin        ", RED);
//        if (RMS > 0.8)
//            lcd_show_string(100, 128, 200, 20, 16, "square    ", RED);
//        if (RMS < 0.63)
//            lcd_show_string(100, 128, 200, 20, 16, "triangular", RED);
    }
}




void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        /* 获取输入捕获值 */
        IC1Value = HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_1);
        IC2Value = HAL_TIM_ReadCapturedValue(&htim8,TIM_CHANNEL_2);
        if (IC1Value != 0)
        {
            /* 占空比计算 */
            DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);
            /* 频率计算 */
            Frequency = 168000000/168/(float)(IC1Value+1);
        }
        else
        {
            DutyCycle = 0;
            Frequency = 0;
        }

    }
    char temp[64];
    sprintf(temp, "%f", DutyCycle);
    lcd_show_string(120, 188, 32, 10, 16, temp, RED);
    lcd_show_xnum(120, 208, Frequency, 2, 16, 0, RED);//频率

}
