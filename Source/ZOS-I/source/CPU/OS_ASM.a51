
$NOMOD51

NAME					OS_ASM

;CPU_REG_SIZE			EQU			13
CPU_XDATA_P_SIZE		EQU			2

P0						DATA	080H
P1						DATA	090H
P2						DATA	0A0H
P3						DATA	0B0H
Switch_Monitoring		BIT	0A0H.7
T0						BIT	0B0H.4
AC						BIT	0D0H.6
T1						BIT	0B0H.5
T2						BIT	090H.0
EA						BIT	0A8H.7
Switch_Remote_Control	BIT	0A0H.6
IE						DATA	0A8H
EXF2					BIT	0C8H.6
RD						BIT	0B0H.7
ES						BIT	0A8H.4
IP						DATA	0B8H
RI						BIT	098H.0
INT0					BIT	0B0H.2
CY						BIT	0D0H.7
TI						BIT	098H.1
INT1					BIT	0B0H.3
RCAP2H					DATA	0CBH
PS						BIT	0B8H.4
SP						DATA	081H
T2EX					BIT	090H.1
OV						BIT	0D0H.2
RCAP2L					DATA	0CAH
C_T2					BIT	0C8H.1
WR						BIT	0B0H.6
RCLK					BIT	0C8H.5
TCLK					BIT	0C8H.4
SBUF					DATA	099H
PCON					DATA	087H
SCON					DATA	098H
TMOD					DATA	089H
TCON					DATA	088H
IE0						BIT	088H.1
IE1						BIT	088H.3
B						DATA	0F0H
CP_RL2					BIT	0C8H.0
ACC	DATA				0E0H
ET0						BIT	0A8H.1
ET1						BIT	0A8H.3
TF0						BIT	088H.5
Speed_Control_1			BIT	090H.0
ET2						BIT	0A8H.5
TF1						BIT	088H.7
Speed_Control_2			BIT	090H.1
TF2						BIT	0C8H.7
RB8						BIT	098H.2
TH0						DATA	08CH
Speed_Control_3			BIT	090H.2
EX0						BIT	0A8H.0
IT0						BIT	088H.0
TH1						DATA	08DH
TB8						BIT	098H.3
EX1						BIT	0A8H.2
IT1						BIT	088H.2
TH2						DATA	0CDH
P						BIT	0D0H.0
SM0						BIT	098H.7
TL0						DATA	08AH
SM1						BIT	098H.6
TL1						DATA	08BH
SM2						BIT	098H.5
TL2						DATA	0CCH
PT0						BIT	0B8H.1
PT1						BIT	0B8H.3
RS0						BIT	0D0H.3
PT2						BIT	0B8H.5
TR0						BIT	088H.4
RS1						BIT	0D0H.4
TR1						BIT	088H.6
TR2						BIT	0C8H.2
PX0						BIT	0B8H.0
PX1						BIT	0B8H.2
DPH						DATA	083H
DPL						DATA	082H
EXEN2					BIT	0C8H.3
REN						BIT	098H.4
T2CON					DATA	0C8H
RXD						BIT	0B0H.0
TXD						BIT	0B0H.1
F0						BIT	0D0H.5
PSW						DATA	0D0H

?PR?OS_TaskSwitch?OS_ASM				SEGMENT CODE 
?PR?OS_TaskSwitch_Isr?OS_ASM			SEGMENT CODE
?PR?Extern_0_Int_?OS_ASM				SEGMENT CODE 
?PR?Extern_1_Int_?OS_ASM				SEGMENT CODE 
?PR?Serial_Int_?OS_ASM					SEGMENT CODE 
?PR?Timer_1_Int_?OS_ASM					SEGMENT CODE  
EXTRN									XDATA (OS_TaskStackAddr)
EXTRN									XDATA (OS_TaskStatus)
EXTRN									XDATA (OS_TaskCurLength)
EXTRN									XDATA (OS_TaskReadyList)
EXTRN									XDATA (OS_TaskRegGroup)
EXTRN									DATA (OS_CurTaskNum)
EXTRN									DATA (OS_NexTaskNum)
EXTRN									DATA (OS_SPDefaultValue)
EXTRN									DATA (TH0_Reload)
EXTRN									DATA (TL0_Reload)
EXTRN									DATA (OsMaxNumOfTask)
EXTRN									DATA (OsCpuCharSize)
EXTRN									DATA (OS_RegSize)
EXTRN									CODE (?C?PSTPTR)
EXTRN									CODE (TickTask)
PUBLIC									OS_TaskSwitch_Isr
PUBLIC									OS_TaskSwitch
PUBLIC									Extern_0_Int_
PUBLIC									Extern_1_Int_
PUBLIC									Serial_Int_
PUBLIC									Timer_1_Int_
EXTRN									CODE (Extern_0_Int)
EXTRN									CODE (Extern_1_Int)
EXTRN									CODE (Serial_Int)
EXTRN									CODE (Timer_1_Int)
EXTRN									CODE (TaskSwitch_HOOK)
DISABLE_ALL_INT	MACRO							;关闭总中断宏
	CLR		EA
ENDM
ENABLE_ALL_INT	MACRO							;开启总中断宏
	SETB	EA
ENDM

PUSH_ALL_REG	MACRO							;压栈宏
	PUSH	ACC
	PUSH	B
	PUSH	DPH
	PUSH	DPL
	PUSH	PSW
	PUSH	AR0
	PUSH	AR1
	PUSH	AR2
	PUSH	AR3
	PUSH	AR4
	PUSH	AR5
	PUSH	AR6
	PUSH	AR7
ENDM

POP_ALL_REG		MACRO							;出栈宏
	POP		AR7
	POP		AR6
	POP		AR5
	POP		AR4
	POP		AR3
	POP		AR2
	POP		AR1
	POP		AR0
	POP		PSW
	POP		DPL
	POP		DPH
	POP		B
	POP		ACC
ENDM


TIMER_INT_RELOAD	MACRO						;定时器重装宏
	MOV		TH0,TH0_Reload
	MOV		TL0,TL0_Reload
ENDM

SAVE_CONTEXT	MACRO							;保存上文宏
	LCALL	GET_REG_GROUP_ADDR
	MOV		A,SP
	SUBB	A,OS_RegSize
	MOV		R0,A
	INC		R0
	MOV		R1,OS_RegSize
	LCALL	COPY_DATA_TOX
	MOV		R1,#CPU_XDATA_P_SIZE
	LCALL	GET_POINT_ARRAY
	MOV		A,SP
	SUBB	A,OS_RegSize
	MOV		B,OS_SPDefaultValue
	SUBB	A,B
	MOV		R1,A
	MOV		B,R1
	MOV		R0,OS_SPDefaultValue
	INC		R0
	LCALL	COPY_DATA_TOX
	MOV		R1,B
	LCALL	SAVE_STACK_LENGTH
ENDM

RESTORE_CONTEXT		MACRO						;恢复下文宏
	LCALL	GET_STACK_LENGTH
	LCALL	GET_POINT_ARRAY
	MOV		R0,OS_SPDefaultValue
	CLR		A									;计算堆栈的安全位置，防止自杀
	ADD		A,R1								;所谓自杀，即函数修改了自己的堆栈里的PC指针
	ADD		A,OS_RegSize						;导致函数不能返回，此处就是要计算后
	ADD		A,R0								;得到一个不会被修改的堆栈地址，确保安全
	MOV		SP,A								;此处堆栈要重新设计位置
	INC		R0
	LCALL	COPY_DATA_FX						;<-自杀主要是这个函数造成的
	LCALL	GET_REG_GROUP_ADDR
	MOV		R1,OS_RegSize
	LCALL	COPY_DATA_FX
	DEC		R0
	MOV		SP,R0
ENDM

LOAD_NEXT_PRIO		MACRO						;装入下一优先级
	PUSH	ACC
	MOV		A,OS_NexTaskNum
	MOV		OS_CurTaskNum,A
	POP		ACC
ENDM

RSEG	?PR?OS_TaskSwitch?OS_ASM
OS_TaskSwitch:
USING	0
		DISABLE_ALL_INT								;屏蔽中断
		PUSH_ALL_REG							;压栈保存
		LCALL	TaskSwitch_HOOK					;调用钩子函数
		LCALL	FIND_NEXT_TASK					;寻找下一优先级任务
		MOV		A,OS_CurTaskNum					;判断是否需要任务切换
		CJNE	A,OS_NexTaskNum,TASK_SWITCH_1	;下一就绪任务不是当前任务，需切换
		LJMP	TASK_BACK_1						;下一就绪任务是当前任务，不用切换
		TASK_SWITCH_1:							;开始任务切换
		SAVE_CONTEXT							;保存上文
		LOAD_NEXT_PRIO							;优先级保存
		RESTORE_CONTEXT							;恢复下文
		TASK_BACK_1:							;任务不用切换直接返回
		POP_ALL_REG								;弹栈恢复
		ENABLE_ALL_INT								;打开中断
RET
CSEG	AT	00003H
	LJMP	Extern_0_Int_
CSEG	AT	0000BH
	LJMP	OS_TaskSwitch_Isr
CSEG	AT	00013H
	LJMP	Extern_1_Int_
CSEG	AT	0001BH 
	LJMP	Timer_1_Int_
CSEG	AT	00023H
	LJMP	Serial_Int_
RSEG	?PR?OS_TaskSwitch_Isr?OS_ASM
OS_TaskSwitch_Isr:
USING	0
		DISABLE_ALL_INT								;屏蔽中断
		TIMER_INT_RELOAD						;定时器重装
		PUSH_ALL_REG							;压栈保存
		LCALL	TickTask						;执行滴答任务
		LCALL	FIND_NEXT_TASK					;寻找下一优先级任务
		MOV		A,OS_CurTaskNum					;判断是否需要任务切换
		CJNE	A,OS_NexTaskNum,TASK_SWITCH_2	;下一就绪任务不是当前任务，需切换
		LJMP	TASK_BACK_2						;下一就绪任务是当前任务，不用切换
		TASK_SWITCH_2:							;开始任务切换
		SAVE_CONTEXT							;保存上文
		LOAD_NEXT_PRIO							;优先级保存
		RESTORE_CONTEXT							;恢复下文
		TASK_BACK_2:							;任务不用切换直接返回
		POP_ALL_REG								;弹栈恢复
		ENABLE_ALL_INT								;打开中断
RETI
RSEG	?PR?Extern_0_Int_?OS_ASM
Extern_0_Int_:
USING	0
		DISABLE_ALL_INT								;屏蔽中断
		PUSH_ALL_REG							;压栈保存
		LCALL	Extern_0_Int
		LCALL	FIND_NEXT_TASK					;寻找下一优先级任务
		MOV		A,OS_CurTaskNum					;判断是否需要任务切换
		CJNE	A,OS_NexTaskNum,TASK_SWITCH_3	;下一就绪任务不是当前任务，需切换
		LJMP	TASK_BACK_3						;下一就绪任务是当前任务，不用切换
		TASK_SWITCH_3:							;开始任务切换
		SAVE_CONTEXT							;保存上文
		LOAD_NEXT_PRIO							;优先级保存
		RESTORE_CONTEXT							;恢复下文
		TASK_BACK_3:							;任务不用切换直接返回
		POP_ALL_REG								;弹栈恢复
		ENABLE_ALL_INT								;打开中断
RETI
RSEG	?PR?Extern_1_Int_?OS_ASM
Extern_1_Int_:
USING	0
		DISABLE_ALL_INT								;屏蔽中断
		PUSH_ALL_REG							;压栈保存
		LCALL	Extern_1_Int
		LCALL	FIND_NEXT_TASK					;寻找下一优先级任务
		MOV		A,OS_CurTaskNum					;判断是否需要任务切换
		CJNE	A,OS_NexTaskNum,TASK_SWITCH_4	;下一就绪任务不是当前任务，需切换
		LJMP	TASK_BACK_4						;下一就绪任务是当前任务，不用切换
		TASK_SWITCH_4:							;开始任务切换
		SAVE_CONTEXT							;保存上文
		LOAD_NEXT_PRIO							;优先级保存
		RESTORE_CONTEXT							;恢复下文
		TASK_BACK_4:							;任务不用切换直接返回
		POP_ALL_REG								;弹栈恢复
		ENABLE_ALL_INT								;打开中断

RETI
RSEG	?PR?Serial_Int_?OS_ASM
Serial_Int_:
USING	0
		DISABLE_ALL_INT								;屏蔽中断
		PUSH_ALL_REG							;压栈保存
		LCALL	Serial_Int
		LCALL	FIND_NEXT_TASK					;寻找下一优先级任务
		MOV		A,OS_CurTaskNum					;判断是否需要任务切换
		CJNE	A,OS_NexTaskNum,TASK_SWITCH_5	;下一就绪任务不是当前任务，需切换
		LJMP	TASK_BACK_5						;下一就绪任务是当前任务，不用切换
		TASK_SWITCH_5:							;开始任务切换
		SAVE_CONTEXT							;保存上文
		LOAD_NEXT_PRIO							;优先级保存
		RESTORE_CONTEXT							;恢复下文
		TASK_BACK_5:							;任务不用切换直接返回
		POP_ALL_REG								;弹栈恢复
		ENABLE_ALL_INT								;打开中断

RETI
RSEG	?PR?Timer_1_Int_?OS_ASM
Timer_1_Int_:
USING	0
		DISABLE_ALL_INT								;屏蔽中断
		PUSH_ALL_REG							;压栈保存
		LCALL	Timer_1_Int
		LCALL	FIND_NEXT_TASK					;寻找下一优先级任务
		MOV		A,OS_CurTaskNum					;判断是否需要任务切换
		CJNE	A,OS_NexTaskNum,TASK_SWITCH_6	;下一就绪任务不是当前任务，需切换
		LJMP	TASK_BACK_6						;下一就绪任务是当前任务，不用切换
		TASK_SWITCH_6:							;开始任务切换
		SAVE_CONTEXT							;保存上文
		LOAD_NEXT_PRIO							;优先级保存
		RESTORE_CONTEXT							;恢复下文
		TASK_BACK_6:							;任务不用切换直接返回
		POP_ALL_REG								;弹栈恢复
		ENABLE_ALL_INT								;打开中断

RETI

DPTR_SHIFT:										;DPTR上移函数。
												;参数：偏移量（0xr6r7）,dptr需要偏移的地址
	PUSH	ACC
	CLR		C
	MOV		A,DPL
	ADD		A,AR7
	MOV		DPL,A
	MOV		A,DPH
	ADDC	A,AR6
	MOV		DPH,A
	POP		ACC
RET

GET_REG_GROUP_ADDR:								;获取CPU寄存器组的指针。
												;参数：无
	PUSH	ACC
	PUSH	B
	PUSH	AR6
	PUSH	AR7
	MOV		A,OS_RegSize
	MOV		B,OS_CurTaskNum
	MUL		AB
	MOV		AR6,B
	MOV		AR7,A
	MOV		DPTR,#OS_TaskRegGroup
	LCALL	DPTR_SHIFT
	POP		AR7
	POP		AR6
	POP		B
	POP		ACC
RET

COPY_DATA_TOX:									;内存到外村复制。
												;参数：r0首地址，r1次数，dptr外存地址
	PUSH	ACC
	LOOP_2:
	MOV		A,@R0
	MOVX	@DPTR,A
	INC		R0
	INC		DPTR
	DJNZ	R1,LOOP_2
	POP		ACC
RET

COPY_DATA_FX:									;外存到内存复制。
												;参数：r0内存首地址，r1次数，dptr外村首地址
	PUSH	ACC
	LOOP_3:
	MOVX	A,@DPTR
	MOV		@R0,A
	INC		R0
	INC		DPTR
	DJNZ	R1,LOOP_3
	POP		ACC
RET

GET_POINT_ARRAY:								;以指针数组元素为地址。
												;参数：无
	PUSH	ACC
	PUSH	B
	PUSH	AR6
	PUSH	AR7
	MOV		DPTR,#OS_TaskStackAddr
	MOV		A,OS_CurTaskNum
	MOV		B,#2
	MUL		AB
	MOV		R6,B
	MOV		R7,A
	LCALL	DPTR_SHIFT
	MOVX	A,@DPTR
	PUSH	ACC
	INC		DPTR
	MOVX	A,@DPTR
	PUSH	ACC
	POP		DPL
	POP		DPH
	POP		AR7
	POP		AR6
	POP		B
	POP		ACC
RET

SAVE_STACK_LENGTH:								;保存堆栈长度。
												;参数：r1堆栈长度
	PUSH	DPH
	PUSH	DPL
	PUSH	ACC
	PUSH	AR6
	PUSH	AR7
	MOV		DPTR,#OS_TaskCurLength
	MOV		A,OS_CurTaskNum
	MOV		R6,#00H
	MOV		R7,A
	LCALL	DPTR_SHIFT
	MOV		A,R1
	MOVX	@DPTR,A
	POP		AR7
	POP		AR6
	POP		ACC
	POP		DPL
	POP		DPH
RET

GET_STACK_LENGTH:								;获取堆栈长度。
												;参数：无，
												;返回r1堆栈长度
	PUSH	DPH
	PUSH	DPL
	PUSH	ACC
	PUSH	AR6
	PUSH	AR7
	MOV		DPTR,#OS_TaskCurLength
	MOV		A,OS_CurTaskNum
	MOV		R6,#00H
	MOV		R7,A
	LCALL	DPTR_SHIFT
	MOVX	A,@DPTR
	MOV		R1,A
	POP		AR7
	POP		AR6
	POP		ACC
	POP		DPL
	POP		DPH
RET

FIND_NEXT_TASK:									;寻找最高优先级任务。
												;参数：无。
												;返回OS_NexTaskNum
	PUSH	DPL
	PUSH	DPH
	PUSH	AR0
	PUSH	AR1
	PUSH	ACC
	PUSH	B
	MOV		DPTR,#OS_TaskReadyList
	MOV		R0,OsMaxNumOfTask
	MOV		A,R0
	MOV		B,OsCpuCharSize
	DIV		AB
	INC		A
	MOV		R0,A
	LOOP_MAIN:
	MOVX	A,@DPTR
	JZ		LOOP_4
	CLR		C
	MOV		R1,OsCpuCharSize
	LOOP_SELF:
	RRC		A
	JC		LOOP_5
	DJNZ	R1,LOOP_SELF
	LOOP_5:
	MOV		A,OsCpuCharSize
	CLR		C
	SUBB	A,R1
	MOV		B,R0
	MUL		AB
	MOV		OS_NexTaskNum,A
	LJMP	END_LOOP
	LOOP_4:
	DJNZ	R0,END_LOOP
	INC		DPTR
	LJMP	LOOP_MAIN
	END_LOOP:
	POP		B
	POP		ACC
	POP		AR1
	POP		AR0
	POP		DPH
	POP		DPL
RET
END
