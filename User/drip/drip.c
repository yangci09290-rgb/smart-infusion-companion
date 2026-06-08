#include "sys.h"

/* 全局变量定义 */
unsigned char two[] = "00000           ";  // 用于显示脉冲数量的字符串，初始值为"00000"（5位数字+空格填充）
char show1[] = "T=0000 ms";               // 显示两次脉冲间隔时间的字符串，格式示例："T=0123 ms"
char show2[] = "000 /min";                // 显示每分钟心跳（滴速）的字符串，格式示例："015 /min"
u16 pul1 = 0;                             // 脉冲计数器（用于累计脉冲数量）
unsigned int count_time, perone, testresult = 0;  // count_time:计时变量；perone:脉冲间隔时间；testresult:计算结果
char DripRateVal[4];                      // 存储滴速值的字符串缓存（3位数字+1个空字符）

/***************************************
 * 函数名：Drip_Init
 * 功能：初始化点滴传感器（GPIO配置）
 * 参数：无
 * 说明：将PB8配置为上拉输入模式，用于检测点滴脉冲信号
 **************************************/
void Drip_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;              // 选择PB8引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;          // 上拉输入模式（默认高电平，有脉冲时拉低）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 高速模式（适应脉冲检测）
	GPIO_Init(GPIOB, &GPIO_InitStructure);                // 应用配置
}

/***************************************
 * 函数名：DripRate_Check
 * 功能：计算并更新滴速数据
 * 参数：无
 * 说明：
 *   1. 将脉冲数pul1格式化为5位字符串（two[]）
 *   2. 计算每分钟滴速（testresult = 6000/间隔时间）
 *   3. 更新显示字符串和全局变量SensorData.DripRateVal
 **************************************/
void DripRate_Check(void)
{
	/* 将脉冲数pul1转换为5位数字字符串（例如123→"00123"） */
	two[0] = pul1 / 10000 + '0';     // 万位
	two[1] = pul1 % 10000 / 1000 + '0'; // 千位
	two[2] = pul1 % 1000 / 100 + '0';   // 百位
	two[3] = pul1 % 100 / 10 + '0';     // 十位
	two[4] = pul1 % 10 + '0';           // 个位
	// oled_ShowString(16, 0, two, 16); // 注释掉的OLED显示函数（需根据实际硬件启用）
	
	/* 计算每分钟滴速（testresult = 60000ms/每滴时间ms） */
	testresult = (unsigned int)6000 / perone;  // 假设perone单位为10ms（需根据实际计时调整）
	
	/* 更新间隔时间字符串show1（格式："T=0123 ms"） */
	show1[2] = perone % 1000 / 100 + '0';  // 百位
	show1[3] = perone % 100 / 10 + '0';    // 十位
	show1[4] = perone % 10 + '0';          // 个位
	
	/* 更新滴速字符串show2（格式："015 /min"） */
	show2[0] = testresult % 1000 / 100 + '0';  // 百位
	show2[1] = testresult % 100 / 10 + '0';    // 十位
	show2[2] = testresult % 10 + '0';          // 个位
	
	/* 将滴速值存入全局结构体 */
	memcpy(DripRateVal, show2, 3);          // 复制show2前3位数字到DripRateVal
	DripRateVal[3] = '\0';                 // 添加字符串结束符（保险操作）
	SensorData.DripRateVal = atoi(DripRateVal);  // 转换为整型存储
}
