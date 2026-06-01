//========================================================================
// 出品方：天祥电子工作室
// 联系人：李经理
// V同号： 18101333172
// 说明：有学习板需求的用户可联系原作者
// 版本：部分代码来源参考STC官方提供或来自网络共享资料,其余部分均为作者原创,
//       如有需要引用本示例代码的用户请注明来源
// 网址：www.txmcu.com
//========================================================================
#include "Config.h"
#include "Delay.h"
#include "1602.h"
#include "At24c02.h"
#include "INTx.h"
#include "Display.h"

uchar contentTem[] = {"Current Number:"};

int main(void)
{
	uchar num;
	At24c02Init();    //AT24C02硬件初始化配置
	InterruptInit();  //定时器2初始化配置
	num = At24c02ReadAdd(0x01);  // 0x00 - 0xff
	while(1)	
	{
		if(TimeFlag)
		{
			TimeFlag = 0;
			num++;
			if(num > 10)
			{
				num = 0;
			}
			At24c02WriteAdd(0x01, num);			
		}
		display(0, num/10);
		display(1, num%10);
	}		
}


