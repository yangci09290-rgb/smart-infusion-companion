#ifndef __drip_H__
#define __drip_H__

#include "sys.h"

#define DRIP PBin(1)

extern char show2[];//显示一分钟心跳数
extern unsigned int count_time, perone, testresult;
extern u16 pul1;

void Drip_Init(void);
void DripRate_Check(void);

#endif

