/*$T indentinput.cpp GC 1.140 02/27/15 15:32:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef _OS_CFG_H_
#define _OS_CFG_H_

#define ENABLE					1
#define DISANLE					0

#define OS_Signal_ENABLE		ENABLE	/* 信号量使能 */
#define OS_Message_ENABLE		ENABLE	/* 消息使能 */
#define OS_Mutex_ENABLE			ENABLE	/* 互斥体使能 */
#define OS_State_ENABLE			ENABLE	/* 统计任务使能 */
#define OS_State_Period			500		/* 统计周期（滴答数） */
#define StateRate				100		/* 统计数的放大倍数 19% * 100 = 19 */
#define OS_TIMER_ENABLE			ENABLE	/* 定时器使能 */
#define OS_TIMER_MAX_NUM		4		/* 定时器数目 */
#define OS_TIMESTAMP_ENABLE		ENABLE	/* 时间戳使能 */
#define OS_HOOK_ENABLE			ENABLE	/* 钩子函数使能 */
#define OS_STACKCHECK_ENABLE	ENABLE	/* 堆栈检查 */
#define OS_StackSafeSurplus		2		/* 堆栈安全剩余量 */
#endif
