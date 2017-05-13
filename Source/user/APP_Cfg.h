/*$T indentinput.cpp GC 1.140 02/25/15 21:10:55 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <config.h>
#include <STC15Fxxxx.H>

#define OS_MAX_NUM_TASK				6			/* 最大任务个数 */
#define OS_REG_SIZE					13			/* 要保存的寄存器字节大小 */
#define OS_CryFrequency				12000000L	/* CPU时钟 */
#define OS_TickFrequency			100			/* OS时钟频率 */
#define OS_CPU_Instruction_Cycle	6			/* CPU指令周期 */

#define Task_0_StackSize			10			/* 任务0的堆栈大小 */
#define Task_1_StackSize			10
#define Task_2_StackSize			10
#define Task_3_StackSize			10
#define Task_SYS_StackSize			10			/* 操作系统 */
#define Task_Idle_StackSize			6			/* 空闲任务 */

#define Task_0_Prio					1			/* 任务0优先级 */
#define Task_1_Prio					2
#define Task_2_Prio					3
#define Task_3_Prio					4
#define	Task_SYS_Prio				0
#define	Task_Idle_Prio				(OS_MAX_NUM_TASK - 1)

#define Task_SYS_MsgLength			8			/* 操作系统消息缓冲区大小 */
