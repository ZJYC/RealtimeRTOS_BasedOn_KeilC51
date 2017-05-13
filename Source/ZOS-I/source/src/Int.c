#include <OS.h>

extern OS_X_MUTEX		OS_X_Mutex_1;

void Timer_1_Int (void)
{
	static OS_CPU_XUCHAR Counter = 0;
	TL1 = 0xF0;		/* 设置定时初值 */
	TH1 = 0xD8;		/* 设置定时初值 */
	Counter++;
	if(Counter % 50 == 0)
	{
		Counter = 0;
//		OS_ReleaseMutex_INT(&OS_X_Mutex_1);
	}
}
void Extern_0_Int (void)
{
	;
}
void Extern_1_Int (void)
{
	;
}
void Serial_Int (void)
{
	;
}