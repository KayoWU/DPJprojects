#include "Config.h"
#include "Display.h"
#include "MatrixKey.h"

int main(void)
{
	MatrixKeyInit();   //独立按键初始化配置
	while(1)
	{
		static uchar num = 0;
		
		if(matrixKeyPress[0] == 1)
		{
			matrixKeyPress[0] = 0;
			num++;
			if(num > 15)
			{
				num = 0;
			}
		}
		
		if(matrixKeyPress[5] == 1)
		{
			matrixKeyPress[5] = 0;
			if(num == 0)
			{
				num = 16;
			}
			num--;
		}
		
		if(matrixKeyPress[9] == 1)
		{
			matrixKeyPress[9] = 0;
			num = 0;
		}

		if(matrixKeyPress[14] == 1)
		{
			matrixKeyPress[14] = 0;
			num = 15;
		}	
		
		display(0, num);   //显示所需的内容
	}		
}

