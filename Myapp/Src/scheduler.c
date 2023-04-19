#include "All_head.h"

static void Loop_1000Hz(void)	//1msִ��һ��
{
    clear_point();
    FFT();
    HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
    HAL_TIM_IC_CaptureCallback(&htim8);

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
//
//
}



static void Loop_20Hz(void)	//50msִ��һ��
{

}


static void Loop_2Hz(void)	//500msִ��һ��
{


}

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


#define TASK_NUM (sizeof(sched_tasks)/sizeof(sched_task_t))
	


void Scheduler_Setup(void)
{
	uint8_t index = 0;
	for(index=0;index < TASK_NUM;index++)
	{
		sched_tasks[index].interval_ticks = TICK_PER_SECOND/sched_tasks[index].rate_hz;
		if(sched_tasks[index].interval_ticks < 1)
		{
			sched_tasks[index].interval_ticks = 1;
		}
	}
}


void Scheduler_Run(void)
{
	uint8_t index = 0;
	for(index=0;index < TASK_NUM;index++)
	{
		uint32_t tnow = HAL_GetTick();
		if(tnow - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks)
		{
			sched_tasks[index].last_run = tnow;
			sched_tasks[index].task_func();

		}	 
	}
	
}
