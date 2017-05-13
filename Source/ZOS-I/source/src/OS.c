/*$T indentinput.cpp GC 1.140 02/27/15 15:33:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    ZJYC
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include <OS.h>

OS_CPU_XUCHAR		OS_TaskReadyList[OS_MAX_NUM_TASK / 8 + 1] = { 0 };		/* 就续表 */
OS_CPU_XUCHAR		OS_TaskRegGroup[OS_MAX_NUM_TASK][OS_REG_SIZE] = { 0 };	/* 任务寄存器组 */
OS_CPU_XP_XUCHAR	OS_TaskStackAddr[OS_MAX_NUM_TASK] = { 0 };				/* 任务堆栈地址 */
OS_CPU_XUCHAR		OS_TaskCurLength[OS_MAX_NUM_TASK] = { 0 };				/* 当前堆栈长度 */
OS_CPU_XUCHAR		OS_TaskStackSize[OS_MAX_NUM_TASK] = { 0 };				/* 任务堆栈总长度 */
OS_CPU_XUINT		OS_TaskStatus[OS_MAX_NUM_TASK] = { 0 };					/* 任务状态 */
OS_CPU_XUCHAR		OS_TaskTickList[OS_MAX_NUM_TASK / 8 + 1] = { 0 };		/* 滴答列表 */
OS_CPU_XUINT		OS_TaskTickCounter[OS_MAX_NUM_TASK] = { 0 };			/* 滴答计数器 */
OS_CPU_UCHAR		OS_CurTaskNum = 0;						/* 当前任务 */
OS_CPU_UCHAR		OS_NexTaskNum = 0;						/* 下一任务 */
OS_CPU_UCHAR		OS_SPDefaultValue = 0;					/* 默认堆栈指针 */
OS_CPU_UCHAR		TH0_Reload = 0;							/* TH0 */
OS_CPU_UCHAR		TL0_Reload = 0;							/* TL0 */
OS_CPU_UCHAR		OsMaxNumOfTask = OS_MAX_NUM_TASK;		/* 任务最大数 */
OS_CPU_UCHAR		OsCpuCharSize = OS_CPU_UCHAR_LENGTH;	/* 字节大小 */
OS_CPU_XUCHAR		Task_Idle_Stack[Task_Idle_StackSize] = { 0 };	/* 空闲任务堆栈 */
OS_CPU_UCHAR		OS_RegSize = OS_REG_SIZE;						/* 寄存器组大小 */

/* 统计工作 */
#if OS_State_ENABLE
OS_CPU_UINT			CPU_Usage_Counter[OS_MAX_NUM_TASK] = { 0 };
OS_CPU_UINT			CPU_TickCounter_ALL = 0;
#endif
OS_CPU_XUINT		CPU_Usage_ALL = 0;					/* CPU使用率 */
OS_CPU_XUINT		CPU_Usage[OS_MAX_NUM_TASK] = { 0 }; /* 各个任务的CPU使用率 */

/* 定时器 */
#if OS_TIMER_ENABLE
OS_CPU_UINT			OS_TIMER_Counter = 0;
OS_CPU_UINT			OS_TIMER_ARRAY[OS_TIMER_MAX_NUM] = { 0 };
OS_X_TMR			OS_X_Tmr = { 0 };
#endif

/* 操作系统任务 */
OS_X_MSG			OS_Msg_SYS = { 0 };
OS_CPU_XUCHAR		Task_SYS_Stack[Task_SYS_StackSize] = { 0 };
OS_CPU_XUCHAR		SYS_MSG_BUFF[Task_SYS_MsgLength] = { 0 };

/* 时间戳使能 */
#if OS_TIMESTAMP_ENABLE
OS_CPU_ULONG		Timestamp = 0;						/* 时间戳 */
#endif

/* 堆栈检查功能 */
#if OS_STACKCHECK_ENABLE
OS_CPU_UCHAR		OS_StackCheckCounter = 0;
#endif

/*
 =======================================================================================================================
    操作系统初始化
 =======================================================================================================================
 */
void OS_Init(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OS_CPU_XUCHAR	ReadyListLength = sizeof(OS_TaskReadyList);
	OS_CPU_XUCHAR	i = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	PreventWarning(0);						/* 防止编译器警告 */

	for(i = 0; i < ReadyListLength; i++)
	{
		OS_TaskReadyList[i] = 0x00;			/* 就续表清零 */
	}

	for(i = 0; i < OS_MAX_NUM_TASK; i++)
	{
		OS_TaskStackAddr[i] = OS_CPU_NULL;	/* 堆栈地址清零 */
		OS_TaskCurLength[i] = OS_CPU_NULL;	/* 堆栈长度清零 */
		OS_TaskStatus[i] = OS_CPU_NULL;		/* 任务状态清零 */
	}

	/* 系统任务 */
	TaskCreate((OS_CPU_INT) OS_SYS_Task, Task_SYS_Stack, Task_SYS_StackSize, Task_SYS_Prio);

	/* 空闲任务 */
	TaskCreate((OS_CPU_INT) TaskIdle, Task_Idle_Stack, Task_Idle_StackSize, Task_Idle_Prio);

	OS_SPDefaultValue = SP - 2;				/* 获取基准堆栈位置 */
	OS_SysTickInit();						/* 滴答初始化 */
}

/*
 =======================================================================================================================
    滴答任务（时间管理任务）
 =======================================================================================================================
 */
void TickTask(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OS_CPU_UCHAR	i = 0, j = 0, Temp = 0, Temp_2 = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if OS_State_ENABLE							/* 统计单元 */
	CPU_TickCounter_ALL++;
	CPU_Usage_Counter[OS_CurTaskNum]++;
	if(CPU_TickCounter_ALL >= OS_State_Period)
	{
		CPU_TickCounter_ALL = 0;
		SYS_MSG_BUFF[0] = 2;
		OS_SendMessage_INT(&OS_Msg_SYS, &SYS_MSG_BUFF, 0);
	}
#endif
#if OS_TIMER_ENABLE							/* 定时器单元 */
	OS_TIMER_Counter++;
	{
		/*~~~~~~~~~~~~~~~~~~*/
		OS_CPU_UCHAR	i = 0;
		/*~~~~~~~~~~~~~~~~~~*/

		for(i = 0; i < OS_TIMER_MAX_NUM; i++)
		{
			if(OS_TIMER_Counter == OS_TIMER_ARRAY[i])
			{
				SYS_MSG_BUFF[1] = 1;
				OS_SendMessage_INT(&OS_Msg_SYS, &SYS_MSG_BUFF, 0);
				break;
			}
		}
	}
#endif
#if OS_TIMESTAMP_ENABLE						/* 时间戳单元 */
	Timestamp++;							/* 时间戳 */
#endif
#if OS_HOOK_ENABLE							/* 钩子函数 */
	TickTask_HOOK();
#endif
#if OS_STACKCHECK_ENABLE					/* 堆栈检查 */
	OS_StackCheckCounter++;
	if(OS_StackCheckCounter % 10 == 0)
	{
		SYS_MSG_BUFF[2] = 3;
		OS_SendMessage_INT(&OS_Msg_SYS, &SYS_MSG_BUFF, 0);
	}
#endif
#if true									/* 滴答任务主体 */
	for(i = 0; i < sizeof(OS_TaskTickList); i++)
	{
		Temp = OS_TaskTickList[i];			/* 获取等待列表 */

		if(Temp != 0)
		{
			for(j = 0; j < OS_CPU_UCHAR_LENGTH; j++)
			{
				if((Temp & (1 << j)))
				{
					Temp_2 = (i + 1) * j;	/* 获取等待任务序号 */

					if(OS_TaskTickCounter[Temp_2] == 0)
					{
						/* 计数器减为零-》任务变为就绪 */
						DISABLE_ALL_INT();
						AddToReadyList(Temp_2);
						DeleteFromTickList(Temp_2);
						ENABLE_ALL_INT();
					}
					else
					{
						/* 计数器减一 */
						OS_TaskTickCounter[Temp_2]--;
					}
				}
			}
		}
	}
#endif
}

/*
 =======================================================================================================================
    非堵塞延时（Counter：延时滴答数）
 =======================================================================================================================
 */
void OS_Delay_nTick(OS_CPU_UINT Counter)
{
	if(Counter == 0) return;
	DISABLE_ALL_INT();
	DeleteFromReadyList(OS_CurTaskNum); /* 从就绪列表删除 */
	AddToTickList(OS_CurTaskNum);		/* 添加到等待列表 */
	OS_TaskTickCounter[OS_CurTaskNum] = Counter;	/* 装入时间长度 */
	OS_TaskSwitch();	/* 任务调度 */
	ENABLE_ALL_INT();
}

/*
 =======================================================================================================================
    启动系统
 =======================================================================================================================
 */
void OS_Start(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OS_CPU_XUCHAR		ActivedTaskNum;
	OS_CPU_P_UCHAR		pTemp = (OS_CPU_P_UCHAR) 0x00 + OS_SPDefaultValue;
	OS_CPU_XP_XUCHAR	pTemp_2 = OS_TaskStackAddr[0];	/* 指向任务0 */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* 寻找已经激活的任务 */
	for(ActivedTaskNum = 0; ActivedTaskNum < OS_MAX_NUM_TASK; ActivedTaskNum++)
	{
		if((OS_TaskStatus[ActivedTaskNum] & Task_Actived) != 0)
		{
			pTemp_2 = OS_TaskStackAddr[ActivedTaskNum];

			break;
		}
	}

	if(ActivedTaskNum == OS_MAX_NUM_TASK)				/* 未找到激活任务，出错 */
	{
		while(1) DISABLE_ALL_INT();
	}

	/* 指向优先级最高已激活任务 */
	pTemp++;

	*pTemp = pTemp_2[0];

	pTemp++;

	*pTemp = pTemp_2[1];
}

/*
 =======================================================================================================================
    创建一个任务(TaskName:任务名称 Stack:任务堆栈地址
    StackSize:堆栈大小 TaskPrio: :任务优先级)
 =======================================================================================================================
 */
void TaskCreate(OS_CPU_INT TaskName, OS_CPU_P_XUCHAR Stack, OS_CPU_UCHAR StackSize, OS_CPU_CHAR TaskPrio)
{
#if OS_HOOK_ENABLE
	TaskCreate_HOOK();	/* 调用钩子 */
#endif
	if((OS_TaskStatus[TaskPrio] & Task_Actived) == 0)	/* 确保任务未被激活 */
	{
		DISABLE_ALL_INT();
		AddToReadyList(TaskPrio);						/* 任务变为就绪 */
		OS_TaskStackAddr[TaskPrio] = Stack;				/* 添加任务堆栈地址 */
		OS_TaskStackSize[TaskPrio] = StackSize;			/* 添加任务堆栈长度 */
		OS_TaskStatus[TaskPrio] |= Task_Actived;		/* 任务已激活 */
		Stack[1] = (TaskName & 0xff00) >> 8;			/* 堆栈包含任务地址 */
		Stack[0] = TaskName & 0x00ff;
		OS_TaskCurLength[TaskPrio] = 2;					/* 堆栈长度为2字节 */
		ENABLE_ALL_INT();
	}
	else
	{
		while(1) DISABLE_ALL_INT();						/* 任务已经激活了 */
	}
}

/*
 =======================================================================================================================
    系统滴答初始化
 =======================================================================================================================
 */
void OS_SysTickInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	OS_CPU_XUINT	Temp_1 = 0;
	OS_CPU_XUINT	Temp_2 = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	DISABLE_ALL_INT();
	Temp_1 = OS_CryFrequency / OS_CPU_Instruction_Cycle / 1000000L; /* 一微秒几个周期 */
	Temp_2 = 1000000L / OS_TickFrequency * Temp_1;					/* 滴答需要几个周期 */
	Temp_2 = 65536 - Temp_2;
	TH0_Reload = Temp_2 / 256;
	TL0_Reload = Temp_2 % 256;
	TMOD &= 0xF0;	/* 设置定时器模式 */
	TMOD |= 0x01;	/* 设置定时器模式 */
	TH0 = TH0_Reload;
	TL0 = TL0_Reload;
	ET0 = 1;
	TR0 = 1;
	ENABLE_ALL_INT();
}

/*
 =======================================================================================================================
    手动任务切换函数
 =======================================================================================================================
 */
void OS_TaskSwitchManual(void)
{
	OS_TaskSwitch();	/* 任务切换 */
}

/*
 =======================================================================================================================
    发送信号（RecevNum：信号接收者优先级号）
 =======================================================================================================================
 */
void OS_SendSignal(OS_CPU_UCHAR RecevNum)
{
#if OS_Signal_ENABLE
	if(RecevNum == OS_CurTaskNum) return;				/* 向自己发送信号 */
	if(OS_TaskStatus[RecevNum] & Task_Hang_SIGNAL)		/* 确认任务是否在等 */
	{
		DISABLE_ALL_INT();
		OS_TaskStatus[RecevNum] &= ~Task_Hang_SIGNAL;	/* 清除等待状态 */
		AddToReadyList(RecevNum);						/* 加入就续表 */
		OS_TaskSwitch();	/* 任务切换 */
		ENABLE_ALL_INT();
	}

#else
	RecevNum = RecevNum;
#endif
}

/*
 =======================================================================================================================
    发送信号(ISR)(RecevNum:信号接收者优先级号)
 =======================================================================================================================
 */
void OS_SendSignal_INT(OS_CPU_UCHAR RecevNum)
{
#if OS_Signal_ENABLE
	if(OS_TaskStatus[RecevNum] & Task_Hang_SIGNAL)		/* 确认任务是否在等 */
	{
		DISABLE_ALL_INT();
		OS_TaskStatus[RecevNum] &= ~Task_Hang_SIGNAL;	/* 清除等待状态 */
		AddToReadyList(RecevNum);						/* 加入就续表 */
		ENABLE_ALL_INT();
	}

#else
	RecevNum = RecevNum;
#endif
}

/*
 =======================================================================================================================
    等待信号
 =======================================================================================================================
 */
void OS_PendSignal(void)
{
#if OS_Signal_ENABLE
	DISABLE_ALL_INT();
	OS_TaskStatus[OS_CurTaskNum] |= Task_Hang_SIGNAL;	/* 状态等待SIGNAL */
	DeleteFromReadyList(OS_CurTaskNum);					/* 删除就续表 */
	OS_TaskSwitch();						/* 任务切换 */
	ENABLE_ALL_INT();
#else
#endif
}

/*
 * 等待消息。参数：Message（消息结构体地址）Size（消息大小）
 */
void OS_PendMessage (OS_MSG * Message) reentrant
{
#if OS_Message_ENABLE
	OS_MSG	*Temp = Message;
	DISABLE_ALL_INT();
	Message->PendTaskNum = OS_CurTaskNum;	/* 记录消息等待者 */
	DeleteFromReadyList(OS_CurTaskNum);		/* 删除就续表 */
	OS_TaskStatus[OS_CurTaskNum] |= Task_Hang_MEAASGE;	/* 状态等待MESSAGE */
	OS_TaskSwitch();	/* 任务切换 */
	ENABLE_ALL_INT();
	Message = Temp;		/* ？？ */
	return;				/* 返回消息指针 */
#else
	Message = Message;
	return;
#endif
}

/*
 * 发送消息。Message（消息结构体地址）Size（消息大小）
 * MsgAddr（消息地址）
 */
void OS_SendMessage (OS_MSG * Message, OS_CPU_XP_VOID MsgAddr, OS_CPU_CHAR Size) reentrant
{
#if OS_Message_ENABLE
	if(OS_TaskStatus[Message->PendTaskNum] & Task_Hang_MEAASGE)		/* 确定是否在等 */
	{
		/* 等待者就续 */
		AddToReadyList(Message->PendTaskNum);
		OS_TaskStatus[Message->PendTaskNum] &= ~Task_Hang_MEAASGE;	/* 清除状态 */
		Message->MessageAddr = MsgAddr; /* 写入消息地址 */
		Message->MessageSize = Size;	/* 写入消息大小 */
		OS_TaskSwitch();				/* 任务切换 */
	}
#else
	Message = Message;
	MsgAddr = MsgAddr;
	Size = Size;
#endif
}

/*
 * 发送消息。Message（消息结构体地址）Size（消息大小）
 * MsgAddr（消息地址）(ISR)
 */
void OS_SendMessage_INT (OS_MSG * Message, OS_CPU_XP_VOID MsgAddr, OS_CPU_CHAR Size) reentrant
{
#if OS_Message_ENABLE
	if(OS_TaskStatus[Message->PendTaskNum] & Task_Hang_MEAASGE)		/* 确定是否在等 */
	{
		/* 等待者就续 */
		AddToReadyList(Message->PendTaskNum);
		OS_TaskStatus[Message->PendTaskNum] &= ~Task_Hang_MEAASGE;	/* 清除状态 */
		Message->MessageAddr = MsgAddr; /* 写入消息地址 */
		Message->MessageSize = Size;	/* 写入消息大小 */
	}
#else
	Message = Message;
	MsgAddr = MsgAddr;
	Size = Size;
#endif
}

/*
 =======================================================================================================================
    占用互斥体，如果互斥体已经被占用，则等待(
    Mutex:互斥体地址)
 =======================================================================================================================
 */
void OS_OccupyMutex(OS_MUTEX *Mutex)reentrant
{
#if OS_Mutex_ENABLE
	OS_MUTEX * Temp = Mutex;
	if(Mutex->IsOccupy == 0x00)					/* 未被占用 */
	{
		Mutex->IsOccupy = OS_CurTaskNum + 1;
	}

	if(Mutex->IsOccupy != OS_CurTaskNum + 1)	/* 如果任务已经被占用 */
	{
		DISABLE_ALL_INT();
		DeleteFromReadyList(OS_CurTaskNum);		/* 删除就续表 */
		OS_TaskStatus[OS_CurTaskNum] |= Task_Hang_MUTEX;	/* 状态等待MUTEX */
		AddToMutexList(OS_CurTaskNum, Mutex);				/* 加入到MUTEX等待表 */
		OS_TaskSwitch();						/* 任务切换 */
		Mutex = Temp;
		ENABLE_ALL_INT();
		Mutex->IsOccupy = OS_CurTaskNum + 1;	/* 占有成功 */
	}
	else
	{
		Mutex->IsOccupy = OS_CurTaskNum + 1;	/* 占有成功 */
	}

#else
	Mutex = Mutex;
#endif
}

/*
 =======================================================================================================================
    释放互斥体，同时切换任务（Mutex：互斥体地址）
 =======================================================================================================================
 */
void OS_ReleaseMutex(OS_MUTEX *Mutex)reentrant
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if OS_Mutex_ENABLE
	OS_CPU_UCHAR	i = 0, j = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Mutex->IsOccupy = 0x00;					/* 未被占用 */

	for(i = 0; i < OS_MAX_NUM_TASK; i++)	/* 遍历等待列表 */
	{
		if(Mutex->PendList[i / 8] != 0x00)
		{
			if(Mutex->PendList[i / 8] & (1 << (i % 8)))		/* 寻找等待任务 */
			{
				DISABLE_ALL_INT();
				OS_TaskStatus[i] &= ~Task_Hang_MUTEX;		/* 清除等待MUTEX状态 */
				DeleteFromMutexList(i, Mutex);				/* 从MUTEX等待列表删除 */
				AddToReadyList(i);
				OS_TaskReadyList[i / 8] |= (1 << (i % 8));	/* 加入就续表 */
				OS_TaskSwitch();	/* 任务切换 */
				ENABLE_ALL_INT();
			}
		}
	}

#else
	Mutex = Mutex;
#endif
}

/*
 =======================================================================================================================
    释放互斥体，同时切换任务(ISR)（Mutex：互斥体地址）
 =======================================================================================================================
 */
void OS_ReleaseMutex_INT(OS_MUTEX *Mutex)reentrant
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if OS_Mutex_ENABLE
	OS_CPU_UCHAR	i = 0, j = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Mutex->IsOccupy = 0x00;					/* 未被占用 */

	for(i = 0; i < OS_MAX_NUM_TASK; i++)	/* 遍历等待列表 */
	{
		if(Mutex->PendList[i / 8] != 0x00)
		{
			if(Mutex->PendList[i / 8] & (1 << (i % 8)))		/* 寻找等待任务 */
			{
				DISABLE_ALL_INT();
				OS_TaskStatus[i] &= ~Task_Hang_MUTEX;		/* 清除等待MUTEX状态 */
				DeleteFromMutexList(i, Mutex);				/* 从MUTEX等待列表删除 */
				AddToReadyList(i);
				OS_TaskReadyList[i / 8] |= (1 << (i % 8));	/* 加入就续表 */
				ENABLE_ALL_INT();
			}
		}
	}

#else
	Mutex = Mutex;
#endif
}

/*
 =======================================================================================================================
    为防止编译器警告
 =======================================================================================================================
 */
void PreventWarning(OS_CPU_UCHAR Nothing)
{
	if(Nothing)
	{
		OS_PendMessage(OS_CPU_NULL);
		OS_SendMessage(OS_CPU_NULL, OS_CPU_NULL, OS_CPU_NULL);
		OS_SendMessage_INT(OS_CPU_NULL, OS_CPU_NULL, OS_CPU_NULL);
		OS_OccupyMutex(OS_CPU_NULL);
		OS_ReleaseMutex(OS_CPU_NULL);
		OS_ReleaseMutex_INT(OS_CPU_NULL);
		OS_Init();
		TickTask();
		OS_Delay_nTick(0);
		OS_Start();
		OS_SysTickInit();
		OS_TaskSwitchManual();
		OS_SendSignal(0);
		OS_PendSignal();
		OS_SendSignal_INT(OS_CPU_NULL);
		TaskSuspend(0);
		TaskResume(0);
		TickTask_HOOK();
		TaskCreate_HOOK();
		TaskSwitch_HOOK();
		StackCheckWarning(OS_CPU_NULL, OS_CPU_NULL);
		OS_StateTask();
		OS_SYS_Task();
		OS_TMR_Task();
		OS_TIMER_ADD(OS_CPU_NULL, OS_CPU_NULL, OS_CPU_NULL, OS_CPU_NULL);
	}
}

/*
 =======================================================================================================================
    空闲任务
 =======================================================================================================================
 */
void TaskIdle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OS_CPU_XUCHAR	i = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(1)
	{
		for(i = 0; i < 240; i++)
		{
			;
		}
	}
}

/*
 =======================================================================================================================
    任务恢复(Prio:欲恢复任务的优先级号)
 =======================================================================================================================
 */
void TaskResume(OS_CPU_UCHAR Prio)
{
	DISABLE_ALL_INT();
	AddToReadyList(Prio);
	OS_TaskSwitch();
	ENABLE_ALL_INT();
}

/*
 =======================================================================================================================
    任务暂停(Prio:欲暂停任务的优先级号)
 =======================================================================================================================
 */
void TaskSuspend(OS_CPU_UCHAR Prio)
{
	DISABLE_ALL_INT();
	DeleteFromReadyList(Prio);
	OS_TaskSwitch();
	ENABLE_ALL_INT();
}

/*
 =======================================================================================================================
    统计任务（每OS_State_Period个滴答执行一次），当任务过于简单时，统计任务几乎失效
 =======================================================================================================================
 */
void OS_StateTask(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
#if OS_State_ENABLE
	OS_CPU_UCHAR	i = 0;
	/*~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < OS_MAX_NUM_TASK; i++)
	{
		/* 任务的CPU占有率 */
		CPU_Usage[i] = CPU_Usage_Counter[i] * StateRate * 1.0 / OS_State_Period;
		CPU_Usage_Counter[i] = 0;
	}

	CPU_Usage_ALL = StateRate - CPU_Usage[OS_MAX_NUM_TASK - 1]; /* CPU使用率 */
#else
#endif
}

/*
 =======================================================================================================================
    操作系统任务(定时器和统计任务通过Message触发)
 =======================================================================================================================
 */
void OS_SYS_Task(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OS_CPU_UCHAR	Inf = 0, i = 0;
	OS_CPU_P_XCHAR	pBuff = 0x1234;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(1)
	{
		OS_PendMessage(&OS_Msg_SYS);
		pBuff = OS_Msg_SYS.MessageAddr;
		for(i = 0; i < Task_SYS_MsgLength; i++)
		{
			switch(pBuff[i])
			{
			case 0:		break;
			case 1:		{ OS_TMR_Task(); break; }
			case 2:		{ OS_StateTask(); break; }
			case 3:		{ OS_StackCheck_Task(); break; }
			default:	break;
			}

			pBuff[i] = 0;
		}
	}
}

/*
 =======================================================================================================================
    定时器任务
 =======================================================================================================================
 */
void OS_TMR_Task(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
#if OS_TIMER_ENABLE
	OS_CPU_UCHAR	i = 0;
	/*~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < OS_TIMER_MAX_NUM; i++)
	{
		if(OS_TIMER_ARRAY[i] == OS_TIMER_Counter)
		{
			if((OS_X_Tmr.State[i] & OS_TIMER_STATE_Actived) != 0x00)	/* 确保已激活 */
			{
				if(OS_X_Tmr.TimerType[i] == OS_TIMER_TYPE_nTimes)		/* 多次 */
				{
					if(OS_X_Tmr.TriggerNum[i] > 0)
					{
						OS_X_Tmr.TriggerNum[i]--;
						OS_X_Tmr.TimerCallBackFunc[i]();
						OS_TIMER_ARRAY[i] += OS_X_Tmr.TimerLength[i];
					}
					else
					{
						OS_X_Tmr.State[i] &= ~OS_TIMER_STATE_Actived;
					}
				}

				if(OS_X_Tmr.TimerType[i] == OS_TIMER_TYPE_PERIOD)		/* 周期型 */
				{
					OS_X_Tmr.TimerCallBackFunc[i]();
					OS_TIMER_ARRAY[i] += OS_X_Tmr.TimerLength[i];
				}
			}
		}
	}
#endif
}

/*
 =======================================================================================================================
    添加定时器(会以最高优先级运行)(TaskName:任务名称
    TimerLength:定时长度 TimerType:定时器类型 TriggerNum:执行次数)
 =======================================================================================================================
 */
void OS_TIMER_ADD(OS_CPU_INT TaskName, OS_CPU_INT TimerLength, OS_CPU_CHAR TimerType, OS_CPU_UCHAR TriggerNum)
{
	/*~~~~~~~~~~~~~~~~~~*/
#if OS_TIMER_ENABLE
	OS_CPU_UCHAR	i = 0;
	/*~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < OS_TIMER_MAX_NUM; i++)
	{
		if((OS_X_Tmr.State[i] & OS_TIMER_STATE_Actived) == 0x00)		/* 寻找未激活任务 */
		{
			DISABLE_ALL_INT();
			OS_X_Tmr.TimerCallBackFunc[i] = (OS_CPU_P_VOID) TaskName;	/* 任务名称 */
			OS_X_Tmr.TimerLength[i] = TimerLength;						/* 定时长度 */
			OS_X_Tmr.TimerType[i] = TimerType;				/* 定时器类型 */
			OS_X_Tmr.TriggerNum[i] = TriggerNum;			/* 触发次数 */
			OS_TIMER_ARRAY[i] += TimerLength;
			OS_X_Tmr.State[i] |= OS_TIMER_STATE_Actived;	/* 定时器激活 */
			ENABLE_ALL_INT();
			break;
		}
	}

	if(i == OS_TIMER_MAX_NUM)
	{
		while(1) DISABLE_ALL_INT();
	}

#else
	TaskName = TaskName;
	TimerLength = TimerLength;
	TimerType = TimerType;
	TriggerNum = TriggerNum;
#endif
}

/*
 =======================================================================================================================
    堆栈安全检查
 =======================================================================================================================
 */
void OS_StackCheck_Task(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	OS_CPU_UCHAR	i = 0;
	/*~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < OS_MAX_NUM_TASK; i++)
	{
		if(OS_TaskCurLength[i] >= (OS_TaskStackSize[i] - OS_StackSafeSurplus))
		{
#if OS_HOOK_ENABLE
			StackCheckWarning(i, (OS_TaskStackSize[i] - OS_TaskCurLength[i]));
#endif
		}
	}
}
