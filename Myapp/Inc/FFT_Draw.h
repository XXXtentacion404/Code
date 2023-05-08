
#include <All_head.h>

#ifndef TEMPLATE_ADC_H
#define TEMPLATE_ADC_H

#endif //TEMPLATE_ADC_H
void Draw_fft(float32_t *fft_outputbuf,uint16_t size);
void Draw_wave(uint16_t *adcBuff,uint16_t size);
void  AdcGetVal_dma(uint16_t *adcbuf,uint16_t size);
void FFT_1024(uint16_t *adcBuff,uint16_t FFTLENGTH);
void Show_fre(uint16_t F,uint32_t len,uint16_t *adcBuff,float32_t *fft_outputbuf);

void Show_phase_diff(uint16_t len,float32_t *fft_outputbuf1,float32_t*fft_outputbuf2);