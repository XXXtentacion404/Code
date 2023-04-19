#include "All_head.h"



extern uint16_t adcBuff[1024];
float fft_inputbuf[FFT_LENGTH * 2];
float fft_outputbuf[FFT_LENGTH];
uint16_t Npoints;
uint32_t max_pos, min_pos;
float  Fmax;
float vmax, vmin, vpp;             // 波形电压的最大值，最小值，幅值
float wave[1024], wave1[1024];
float RMS;
float fre, Amax;
uint32_t Amax_pos, min_pos; // 存储最大值最小值下标
uint32_t IC1Value;
uint32_t IC2Value;
float DutyCycle=0;
float Frequency=0;

void clear_point()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuff, FFT_LENGTH);
    uint16_t past_vol = 0, pre_vol = 0;
    for (uint16_t x = 1; x < 799; x++)
    {
        pre_vol = adcBuff[x] * (3.3 / 4096) * 60;
            lcd_draw_line(x, 250, x, 480, WHITE);
            lcd_draw_bline(x, past_vol + 280, x + 1, pre_vol + 280, 1, BLACK);
        past_vol = pre_vol;
    }
}
void FFT(){
    for ( uint16_t i= 0; i < FFT_LENGTH; i++)
    {
        fft_inputbuf[i * 2] = adcBuff[i] * 3.3 / 4096;//实部赋值，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
        wave[i] = adcBuff[i] * 3.3 / 4095; // 转换电压
        fft_inputbuf[i * 2 + 1] = 0;//虚部赋值，固定为0.
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1);
    arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);
    fft_outputbuf[0] /= 1024;
    for (int i = 1; i < FFT_LENGTH; i++)//输出各次谐波幅值
    {
        fft_outputbuf[i] /= 512;
    }

    for (int i = 290; i < 800; i++)//画出幅值
    {
        lcd_draw_line(i, 0, i , 229, WHITE);         //清屏
        lcd_draw_line(i,230-100*(fft_outputbuf[i-290]),i+1,230-100*(fft_outputbuf[i-289]),BLACK);//画线
    }
    arm_max_f32(fft_outputbuf, 1024, &Amax, &Amax_pos);  //求出基波在频谱中的位置
    arm_max_f32(wave, 1024, &vmax, &max_pos); //DSP库自带的求最值函数
    arm_min_f32(wave, 1024, &vmin, &min_pos);
    vpp = vmax - vmin;
    lcd_show_xnum(60, 88, vpp*1000, 10, 16, 0, RED);//VPP峰峰值
    lcd_show_xnum(60, 48, vmax*1000, 10, 16, 0, RED);//最大值
    lcd_show_xnum(60, 68, vmin*1000, 10, 16, 0, RED);//最小值
    lcd_show_xnum(60, 148, 1000, 10, 16, 0, RED);   //最小值
    fft_outputbuf[0] = 0;
    arm_max_f32(fft_outputbuf, 1024, &Fmax, &max_pos);  //求取基波频率 基波的幅度是频谱中最大的 基波频率=原波形频率
    fre = (float)10000/ 1024 * max_pos;
    lcd_show_xnum(80, 108, fre, 10, 16, 0, RED);//频率
    Npoints = 10000 / fre;
    for (int i = 0; i < Npoints; i++) wave1[i] = wave[i];//取出一个周期的点存在wave1[i]中
    for (int i = 0; i < Npoints; i++) {
        wave1[i] = wave1[i] - vmin - vpp / 2; //将波形平移到与时间轴对称的位置
        arm_rms_f32(wave1, Npoints,&RMS); // DSP库封装好的求rms值函数 不再手动实现 原型： arm_rms_f32 (const float32_t *pSrc, uint32_t blockSize, float32_t *pResult)
        RMS = RMS / (vpp / 2);
        if (RMS > 0.64 && RMS < 0.8)
            lcd_show_string(80,128,200,20,16,"sin      ",RED);
        if (RMS > 0.8)
            lcd_show_string(80,128,200,20,16,"FANG     ",RED);
        if (RMS < 0.63)
            lcd_show_string(80,128,200,20,16,"sanjiao",RED);
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

    lcd_show_xnum(80, 188, DutyCycle*10, 10, 16, 0, RED);//频率
    lcd_show_xnum(80, 208, Frequency, 10, 16, 0, RED);//频率
//    printf("IC1Value = %d  IC2Value = %d ",IC1Value,IC2Value);
//    printf("占空比：%0.2f%%   频率：%0.2fHz\n",DutyCycle,Frequency);
}

