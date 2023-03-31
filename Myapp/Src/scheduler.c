#include "scheduler.h"
#include "stdio.h"
#include "adc.h"
#include "vofa+.h"
#include "lcd.h"
int ADC_Value=0;
float vol=0;
uint16_t temp[1000]={0};
uint16_t ms;

static void Loop_1000Hz(void)	//1msִ��һ��
{
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == 0) {
        ms++;
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            ADC_Value = HAL_ADC_GetValue(&hadc1);
            temp[ms] = ADC_Value * (3.3 / 4096) * 100;
        }
        if (ms == 799)//采集到800次后画一次图
        {
            display();
            for (ms = 0; ms < 799; ms++)
                lcd_draw_bline(ms, 480 - temp[ms] - 130, (ms + 1), 480 - temp[ms + 1] - 130, 2, BLACK);
            ms = 0;
        }
    }
}


static void Loop_500Hz(void)	//2msִ��һ��
{
	
	//StepProcedure();
}


static void Loop_200Hz(void)	//5msִ��һ��
{
	//task();
	 //StepProcedure();
}



static void Loop_100Hz(void)	//10msִ��һ��
{

}



static void Loop_50Hz(void)	//20msִ��һ��
{
//    float send_data[]={0};
//    vofa_send(send_data,sizeof (send_data)/4);
//
}



static void Loop_20Hz(void)	//50msִ��һ��
{

}


static void Loop_2Hz(void)	//500msִ��һ��
{

	HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
}
//ϵͳ�������ã�������ִͬ��Ƶ�ʵġ��̡߳�
static sched_task_t sched_tasks[] =
{
	{Loop_1000Hz,1000,  0, 0},
	{Loop_500Hz , 500,  0, 0},
	{Loop_200Hz , 200,  0, 0},
	{Loop_100Hz , 100,  0, 0},
	{Loop_50Hz  ,  50,  0, 0},
	{Loop_20Hz  ,  20,  0, 0},
	{Loop_2Hz   ,   2,  0, 0},
};


//�������鳤�ȣ��ж��߳�����
#define TASK_NUM (sizeof(sched_tasks)/sizeof(sched_task_t))
	


void Scheduler_Setup(void)
{
	uint8_t index = 0;
	//��ʼ�������
	for(index=0;index < TASK_NUM;index++)
	{
		//����ÿ���������ʱ������
		sched_tasks[index].interval_ticks = TICK_PER_SECOND/sched_tasks[index].rate_hz;
		//�������Ϊ1��Ҳ����1ms
		if(sched_tasks[index].interval_ticks < 1)
		{
			sched_tasks[index].interval_ticks = 1;
		}
	}
}


//��������ŵ�main������while(1)�У���ͣ�ж��Ƿ����߳�Ӧ��ִ��
void Scheduler_Run(void)
{
	uint8_t index = 0;
	//ѭ���ж������̣߳��Ƿ�Ӧ��ִ��

	
	for(index=0;index < TASK_NUM;index++)
	{
		//��ȡϵͳ��ǰʱ�䣬��λMS
		uint32_t tnow = HAL_GetTick();
		//�����жϣ������ǰʱ���ȥ��һ��ִ�е�ʱ�䣬���ڵ��ڸ��̵߳�ִ�����ڣ���ִ���߳�
		if(tnow - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks)
		{
			
			//�����̵߳�ִ��ʱ�䣬������һ���ж�
			sched_tasks[index].last_run = tnow;
			//ִ���̺߳�����ʹ�õ��Ǻ���ָ��
			sched_tasks[index].task_func();

		}	 
	}
	
}
