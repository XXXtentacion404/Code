#include "All_head.h"



uint16_t adcBuff1[1024];
uint16_t adcBuff2[1024];
float fft_inputbuf1[FFT_LENGTH * 2];
float fft_outputbuf1[FFT_LENGTH];
float fft_inputbuf2[FFT_LENGTH * 2];
float fft_outputbuf2[FFT_LENGTH];
uint16_t Npoints;
uint32_t max_pos1, min_pos;
uint32_t max_pos2;
float32_t max_val2;
float  Fmax,RMS,fre;
float vmax, vmin, vpp;             // 波形电压的最大值，最小值，幅值
float wave[1024], wave1[1024];
uint32_t IC1Value,IC2Value=0;
float DutyCycle,Frequency;




void clear_point()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuff1, FFT_LENGTH);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adcBuff2, FFT_LENGTH);


    uint16_t past_vol1 = 0, pre_vol1 = 0;
    uint16_t past_vol = 0, pre_vol = 0;
    for (uint16_t x = 1; x < 799; x++)
    {
        pre_vol = adcBuff1[x] * (3.3 / 4096) * 60;
        pre_vol1 = adcBuff2[x] * (3.3 / 4096) * 60;
            lcd_draw_line(x, 250, x, 480, WHITE);
            lcd_draw_bline(x, past_vol + 280, x + 1, pre_vol + 280, 1, BLACK);
            lcd_draw_bline(x, past_vol1 + 280, x + 1, pre_vol1 + 280, 1, RED);
        past_vol1 = pre_vol1;
        past_vol = pre_vol;
    }
}
void FFT(){

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuff1, FFT_LENGTH);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adcBuff2, FFT_LENGTH);

    for ( uint16_t i= 0; i < FFT_LENGTH; i++)
    {
        fft_inputbuf1[i * 2] = adcBuff1[i] * 3.3 / 4095;//实部赋值，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
        fft_inputbuf2[i * 2] = adcBuff2[i] * 3.3 / 4095;//实部赋值，* 3.3 / 4096是为了将ADC采集到的值转换成实际电压
        wave[i] = adcBuff1[i] * 3.3 / 4095; // 转换电压
        fft_inputbuf1[i * 2 + 1] = 0;//虚部赋值，固定为0
        fft_inputbuf2[i * 2 + 1] = 0;//虚部赋值，固定为0

    }


    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf1, 0, 1);
    arm_cmplx_mag_f32(fft_inputbuf1, fft_outputbuf1, FFT_LENGTH);
    fft_outputbuf1[0] /= FFT_LENGTH;

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf2, 0, 1);
    arm_cmplx_mag_f32(fft_inputbuf2, fft_outputbuf2, FFT_LENGTH);
    fft_outputbuf2[0] /= FFT_LENGTH;
    for (int i = 1; i < FFT_LENGTH; i++)//输出各次谐波幅值
    {
        fft_outputbuf1[i] /= 512;
        fft_outputbuf2[i] /= 512;
    }

    for (int i = 290; i < 800; i++)//画出幅值
    {

        lcd_draw_line(i, 0, i , 229, WHITE);         //清屏
        lcd_draw_line(i,230-100*(fft_outputbuf1[i-290]),i+1,230-100*(fft_outputbuf1[i-289]),BLACK);//画线
        lcd_draw_line(i,230-100*(fft_outputbuf2[i-290]),i+1,230-100*(fft_outputbuf2[i-289]),RED);//画线
    }

    arm_max_f32(wave, 1024, &vmax, &max_pos1); //DSP库自带的求最值函数
    arm_min_f32(wave, 1024, &vmin, &min_pos);
    vpp = vmax - vmin;
    lcd_show_xnum(60, 88, vpp*1000, 10, 16, 0, RED);//VPP峰峰值
    lcd_show_xnum(60, 48, vmax*1000, 10, 16, 0, RED);//最大值
    lcd_show_xnum(60, 68, vmin*1000, 10, 16, 0, RED);//最小值
    lcd_show_xnum(60, 148, 1000, 10, 16, 0, RED);   //采样率
    fft_outputbuf1[0] = 0;
    fft_outputbuf2[0] = 0;

    arm_max_f32(fft_outputbuf1, 1024, &Fmax, &max_pos1);  //求取基波频率 基波的幅度是频谱中最大的 基波频率=原波形频率
    arm_max_f32(fft_outputbuf2, 1024, &max_val2, &max_pos2);

    float32_t real_part1 = fft_inputbuf1[max_pos1 * 2];
    float32_t imaginary_part1 = fft_inputbuf1[max_pos1 * 2 + 1];


    float32_t real_part2 = fft_inputbuf2[max_pos2 * 2];
    float32_t imaginary_part2 = fft_inputbuf2[max_pos2 * 2 + 1];


    float32_t phase_diff = (atan2f(imaginary_part1,real_part1))-(atan2f(imaginary_part2, real_part2))  ; // 使用 atan2f 函数计算相位差
    float32_t phase_diff_degrees = phase_diff * 180.0f / 3.14159265358979323846f;
    char temp[64];
    sprintf(temp, "%f", phase_diff_degrees);
    if (phase_diff_degrees < 180 && phase_diff_degrees > 0||phase_diff_degrees<0&&phase_diff_degrees>-180) {
        lcd_show_string(120, 168, 32, 10, 16, temp, RED);
    }

    fre = (float)10000/ 1024 * max_pos1;

    lcd_show_xnum(80, 108, fre, 5, 16, 0, RED);//频率
    Npoints = 10000 / fre;
    for (int i = 0; i < Npoints; i++) wave1[i] = wave[i];//取出一个周期的点存在wave1[i]中
    for (int i = 0; i < Npoints; i++) {
        wave1[i] = wave1[i] - vmin - vpp / 2;           //将波形平移到与时间轴对称的位置
        arm_rms_f32(wave1, Npoints,&RMS); // DSP库封装好的求rms值函数 不再手动实现 原型： arm_rms_f32 (const float32_t *pSrc, uint32_t blockSize, float32_t *pResult)
        RMS = RMS / (vpp / 2);
    }
//    printf("%f\n",phase_diff_degrees);
    if (RMS > 0.64 && RMS < 0.8)
        lcd_show_string(100,128,200,20,16,"sin        ",RED);
    if (RMS > 0.8)
        lcd_show_string(100,128,200,20,16,"square    ",RED);
    if (RMS < 0.63)
        lcd_show_string(100,128,200,20,16,"triangular",RED);
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
