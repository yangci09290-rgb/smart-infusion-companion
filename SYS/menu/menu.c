#include "sys.h"

// 模式切换标志位
uint8_t mode_selection = 0;
// 页面显示标志位
uint8_t displayFlag = 0;
// 阈值设置标志位
uint8_t thresholdFlag = 0;

/*****************贯穿整个系统的函数 在主函数只需要调用这一个函数*/

void Mode_selection(void)
{
	// 调用执行设备函数 此函数不能删除
	Action();

	// 按键1 按下模式切换
	if (isKey1)
	{
		isKey1 = isKey2 = isKey3 = isKey4 = 0;
		mode_selection++;
		oled_Clear();// 清屏
	}

	// 模式切换标志位限位
	if (mode_selection >= 2)
	{
		mode_selection = 0;
	}

	// 根据标志位判断进入哪一个模式
	switch (mode_selection)
	{
	case 0:
		Display();	 // 传感器数据显示
		Automatic(); // 自动控制
		break;

	case 1:
		Threshold_Settings(); // 阈值设置
		Automatic();		  // 自动控制
		break;
	}
}

/****
自动控制逻辑判断
****/

void Automatic(void)
{
	if(SensorData.WtrLevelVal > WtrLevelMin){// 液位高于阈值时
		System.WtrLevel = 1;//液位状态标志位置1
	}
	else{
		System.WtrLevel = 0;//液位状态标志位置0
	}

	//检测到液位过低时，声光报警
	if(System.WtrLevel == 0){
		System.Switch3 = 1; // 灯光标志位置1
		System.Switch4 = 1; // 报警标志位置1
	}

	//温度异常，打开声光报警
	else if(SensorData.TempVal != 0 && SensorData.TempVal < Threshold.TempMin){
		System.Switch3 = 1; // 灯光标志位置1
		System.Switch4 = 1; // 报警标志位置1
	}
	else{
		System.Switch3 = 0; // 灯光标志位置0
		System.Switch4 = 0; // 报警标志位置0
	}

	//温度异常，打开加热器
	if(SensorData.TempVal != 0 && SensorData.TempVal < Threshold.TempMin){
		System.Switch2 = 1; // 继电器-加热标志位置1
	}
	else{
		System.Switch2 = 0; // 继电器-加热标志位置0
	}
}

/****
根据执行设备的标志位来开关执行设备
****/

void Action(void)
{
	if (System.Switch2 == 1)// 继电器-加热标志位为1
	{
		RELAY2_ON; // 打开继电器-加热
	}
	else
	{
		RELAY2_OFF; // 关闭继电器-加热
	}

	if (System.Switch3 == 1)// 灯光标志位为1
	{
		LED1_ON; // 打开灯光
	}
	else
	{
		LED1_OFF; // 关闭灯光
	}

	if (System.Switch4 == 1)// 报警标志位为1
	{
		BEEP_ON; // 打开蜂鸣器
	}
	else
	{
		BEEP_OFF; // 关闭蜂鸣器
	}
}

/****
传感器数据显示
****/

void Display(void)
{
	// 智能输液系统监测
	oled_ShowCHinese(16 * 0, 2 * 0, 0);
	oled_ShowCHinese(16 * 1, 2 * 0, 1);
	oled_ShowCHinese(16 * 2, 2 * 0, 2);
	oled_ShowCHinese(16 * 3, 2 * 0, 3);

	//滴速
	oled_ShowCHinese(16 * 0, 2 * 1, 15);
	oled_ShowCHinese(16 * 1, 2 * 1, 16);
	oled_ShowString(16 * 3, 2 * 1, ":", 16);
	oled_ShowNum(16 * 4, 2 * 1, SensorData.DripRateVal, 3, 16);
	oled_ShowString(16 * 6, 2 * 1, "/min", 16);

	//液位
	oled_ShowCHinese(16 * 0, 2 * 2, 17);
	oled_ShowCHinese(16 * 1, 2 * 2, 18);
	oled_ShowString(16 * 3, 2 * 2, ":", 16);

	if(System.WtrLevel == 1){
		oled_ShowCHinese(16 * 4, 2 * 2, 19);
		oled_ShowCHinese(16 * 5, 2 * 2, 20);//正常
	}
	else{
		oled_ShowCHinese(16 * 4, 2 * 2, 21);
		oled_ShowCHinese(16 * 5, 2 * 2, 29);//过低
	}

	// 温度
	oled_ShowCHinese(16 * 0, 2 * 3, 22);
	oled_ShowCHinese(16 * 1, 2 * 3, 23);
	oled_ShowString(16 * 3, 2 * 3, ":", 16);
	OLED_ShowFNum(16 * 4, 2 * 3, SensorData.TempVal, 16); // 显示温度数据
	oled_ShowCHinese(16 * 6, 2 * 3, 24);
}

/****
阈值设置
****/
void Threshold_Settings(void)
{
	//按键2按下选中设置阈值
	if (isKey2) {
		isKey2 = 0;
		thresholdFlag++;

		//限位标志
		if (thresholdFlag >= 3) {
			thresholdFlag = 0;
		}
	}

	// 调用光标显示函数
	Asterisk(thresholdFlag);

	// 按键3按下阈值加
	if (isKey3)
	{
		isKey3 = 0; // 清除按键3标志位

		switch (thresholdFlag)
		{
			case 0:
				Threshold.DripRateMax ++; // 滴速上限+1
				break;
			case 1:
				Threshold.DripRateMin ++; // 滴速下限+1
				break;
			case 2:
				Threshold.TempMin ++; // 体温下限+1
				break;
		}
	}

	// 按键4按下阈值减
	if (isKey4)
	{
		isKey4 = 0;

		switch (thresholdFlag)
		{
			case 0:
				Threshold.DripRateMax --; // 滴速上限-1
				break;
			case 1:
				Threshold.DripRateMin --; // 滴速下限-1
				break;
			case 2:
				Threshold.TempMin --; // 体温下限-1
				break;
		}
	}

	// 如果标志位小于3 显示第一页
	if (thresholdFlag < 3)
	{
		// 阈值设置
		oled_ShowCHinese(16 * 2, 2 * 0, 8);
		oled_ShowCHinese(16 * 3, 2 * 0, 9);
		oled_ShowCHinese(16 * 4, 2 * 0, 10);
		oled_ShowCHinese(16 * 5, 2 * 0, 11);

		// 滴速上限
		oled_ShowCHinese(16 * 0, 2 * 1, 15);
		oled_ShowCHinese(16 * 1, 2 * 1, 16);
		oled_ShowCHinese(16 * 2, 2 * 1, 12);
		oled_ShowCHinese(16 * 3, 2 * 1, 14);
		oled_ShowString(16 * 4, 2 * 1, ":", 16);
		oled_ShowNum(16 * 5, 2 * 1, Threshold.DripRateMax, 3, 16); // 显示滴速上限

		// 滴速下限
		oled_ShowCHinese(16 * 0, 2 * 2, 15);
		oled_ShowCHinese(16 * 1, 2 * 2, 16);
		oled_ShowCHinese(16 * 2, 2 * 2, 13);
		oled_ShowCHinese(16 * 3, 2 * 2, 14);
		oled_ShowString(16 * 4, 2 * 2, ":", 16);
		oled_ShowNum(16 * 5, 2 * 2, Threshold.DripRateMin, 3, 16); // 显示滴速下限

		// 体温下限
		oled_ShowCHinese(16 * 0, 2 * 3, 22);
		oled_ShowCHinese(16 * 1, 2 * 3, 23);
		oled_ShowCHinese(16 * 2, 2 * 3, 13);
		oled_ShowCHinese(16 * 3, 2 * 3, 14);
		oled_ShowString(16 * 4, 2 * 3, ":", 16);
		oled_ShowNum(16 * 5 + 8, 2 * 3, Threshold.TempMin, 2, 16); // 显示体温下限
	}

	/*阈值限幅*/
	//滴速限幅
	if(Threshold.DripRateMax > 99){
		Threshold.DripRateMax = 99;
	}
	if(Threshold.DripRateMin < 1){
		Threshold.DripRateMin = 0;
	}

	//体温限幅
	if(Threshold.TempMin > 99){
		Threshold.TempMin = 99;
	}
	if(Threshold.TempMin < 1){
		Threshold.TempMin = 0;
	}
}

void Asterisk(uint8_t A)
{
	if (A == 3 || A == 7)
	{
		oled_ShowString(16 * 7, 2 * 0, "*", 16);
	}
	else
	{
		oled_ShowString(16 * 7, 2 * 0, " ", 16);
	}

	if (A == 0 || A == 4)
	{
		oled_ShowString(16 * 7, 2 * 1, "*", 16);
	}
	else
	{
		oled_ShowString(16 * 7, 2 * 1, " ", 16);
	}

	if (A == 1 || A == 5)
	{
		oled_ShowString(16 * 7, 2 * 2, "*", 16);
	}
	else
	{
		oled_ShowString(16 * 7, 2 * 2, " ", 16);
	}

	if (A == 2 || A == 6)
	{
		oled_ShowString(16 * 7, 2 * 3, "*", 16);
	}
	else
	{
		oled_ShowString(16 * 7, 2 * 3, " ", 16);
	}
}
