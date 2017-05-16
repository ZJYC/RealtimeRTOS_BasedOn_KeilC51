
# Just for learning not for using #

----------
## The functions this RTOS supported ##

1. task create
1. task delete
1. task delay 
1. signal(Include ISR mode)
1. mutex(Include ISR mode)
1. message(Include ISR mode)
1. soft timer
1. stack check

## Step for demonstration ##

/* init the OS,Include all the Data structure */

    OS_Init();

/* create user task */

	TaskCreate((OS_CPU_INT) Task_0, Task_0_Stack, Task_0_StackSize, Task_0_Prio);

/* Start the OS */

	OS_Start();

## As for OS timer

/* The following means : Call TimerTask_1 every 10 ticks  */

	OS_TIMER_ADD((OS_CPU_INT) TimerTask_1, 10, OS_TIMER_TYPE_PERIOD, 0);

/* the following means:call TimerTask_2 every 10 ticks,only call 4 times */

	OS_TIMER_ADD((OS_CPU_INT) TimerTask_2, 10, OS_TIMER_TYPE_nTimes, 4);


