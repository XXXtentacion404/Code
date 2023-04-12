#include "All_head.h"

extern uint16_t adcBuff[1024];
float fft_inputbuf[FFT_LENGTH * 2];
float fft_outputbuf[FFT_LENGTH];
uint16_t Npoints;
uint32_t max_pos, min_pos;
float fre, Fmax;
float vmax, vmin, vpp;             // 波形电压的最大值，最小值，幅值
float wave[1024], wave1[1024];
float RMS;
uint32_t FFT_Mag[FFT_LENGTH/2]={0};//幅频特性
uint16_t T=1;//定时器2重载值，不能小于PWM的Pluse值
uint16_t  pre=8400;//定时器2预分频值
uint32_t fre1;//采样频率 kHz
int long fftin [FFT_LENGTH];//FFT输入
int long fftout[FFT_LENGTH];//FFT输出
uint16_t F;//波形频率
uint16_t temp=0;//幅值最大的频率成分



void clear_point(uint16_t mode)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuff, FFT_LENGTH);
    static uint16_t x={0};
    uint16_t past_vol,pre_vol;
   // fre=84000000/1/840;//更新采样频率

    for(x=0;x<800;x++){
        fre1=84000000/T/pre;
        lcd_draw_line(x,250,x,480,WHITE);
        //lcd_show_xnum(261,220,fre1,5,16,1,RED);//更新采样率显示
         //  lcd_show_xnum(80,108,fre1,10,16,0,RED);
    }
    for(x=0;x<800;x++)
    {
        pre_vol= adcBuff[x] * (3.3 / 4096) * 60;
        if(mode==1)
        {
            if(x>0&&x<800)	//去除第一个，最后一个以及y轴上点的绘制
                lcd_draw_bline(x,past_vol+280,x+1,pre_vol+280,1,BLACK);
        }
        past_vol = pre_vol;
    }
}
void FFT(){
    float X,Y,Mag,magmax;//实部，虚部，各频率幅值，最大幅值
    uint16_t i;
    for ( i = 0; i < FFT_LENGTH; i++)
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


    for(int i=288;i<800;i++){  //清屏
        lcd_draw_line(i+1,0,i+1,229,WHITE);
    }
    for (int i = 290; i < 800; i++)//输出各次谐波幅值
    {
        lcd_draw_line(i,230-100*(fft_outputbuf[i-290]),i+1,230-100*(fft_outputbuf[i-289]),BLACK);
    }
    arm_max_f32(wave, 1024, &vmax, &max_pos); //DSP库自带的求最值函数
    arm_min_f32(wave, 1024, &vmin, &min_pos);
    vpp = vmax - vmin;
    fft_outputbuf[0] = 0;
    arm_max_f32(fft_outputbuf, 1024, &Fmax, &max_pos);  //求取基波频率 基波的幅度是频谱中最大的 基波频率=原波形频率
    fre = (float)400000 / 1024 * max_pos;
    Npoints = 400000 / fre;

    for (int i = 0; i < Npoints; i++)
        wave1[i] = wave[i];		//取出一个周期的点存在wave1[i]中
    for (int i = 0; i < Npoints; i++) {
        wave1[i] = wave1[i] - vmin - vpp / 2; //将波形平移到与时间轴对称的位置
        arm_rms_f32(wave1, Npoints,
                    &RMS); // DSP库封装好的求rms值函数 不再手动实现 原型： arm_rms_f32 (const float32_t *pSrc, uint32_t blockSize, float32_t *pResult)
        RMS = RMS / (vpp / 2);
        if (RMS > 0.64 && RMS < 0.8)
            lcd_show_string(80,128,200,20,16,"sin      ",RED);
        if (RMS > 0.8)
            lcd_show_string(80,128,200,20,16,"FANG     ",RED);
        if (RMS < 0.63)
            lcd_show_string(80,128,200,20,16,"sanjiao",RED);
    }// 平移后波形的幅度就是所求的vpp的一半
}
void GetPowerMag(void)
{
    float X,Y,Mag,magmax;//实部，虚部，各频率幅值，最大幅值
    uint16_t i;

    //调用自cr4_fft_1024_stm32
    arm_cmplx_mag_f32(fftin, fftout, FFT_LENGTH);

    for(i=1; i<FFT_LENGTH/2; i++)
    {
        X = (fftout[i] << 16) >> 16;	//低16位存实部
        Y = (fftout[i] >> 16);			//高16位存虚部

        Mag = sqrt(X * X + Y * Y); 		//计算模值
        FFT_Mag[i]=Mag;					//存入缓存，用于输出查验
        //获取最大频率分量及其幅值
        if(Mag > magmax)
        {
            magmax = Mag;
            temp = i;
        }
    }
    F=(uint16_t)(temp*(fre*1.0/FFT_LENGTH));	//FFT所得实际频率f=点数i*(采样频率F/总采样点数N)
    lcd_show_xnum(80,108,F,10,16,0,RED);
}