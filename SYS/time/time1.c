#include "sys.h"

/******************   	  
功能：高级定时器1中断初始化,这里时钟选择为APB2
参数：arr：自动重装值。
	  psc：时钟预分频数
返回值：无
******************/
void TIM1_Int_Init(u16 arr,u16 psc)
{  
    TIM_DeInit(TIM1);//将外设 TIM1 寄存器重设为缺省值
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//使能定时器1时钟
 
	//初始化TIM1
	TIM_TimeBaseStructure.TIM_Period = arr; 				//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 				//设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 		//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);			//把 TIM_TimeBaseInitStruct 中的每一个参数按缺省值填入
	
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清中断标志位
	TIM_ITConfig(TIM1, TIM_IT_Update|TIM_IT_Trigger,ENABLE ); //使能指定的TIM1中断,允许更新中断 允许触发中断源 
 
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1	
 
}

int jishu=0;

//定时器1中断服务程序
void TIM1_UP_IRQHandler(void)   //TIM1中断
{  	     
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源 
	{
		//应用代码编写区	
		if(DRIP == 0) jishu ++;
		else jishu = 0;
	
		count_time ++;//2个水滴之间时间累计计数
	
		if(jishu == 6) //持续60ms以上，说明是脉冲计数
		{
			pul1 ++;//脉冲计数
			perone = count_time;//2次心跳时间
			count_time = 0;
		}
	
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源 
	}	     
}
