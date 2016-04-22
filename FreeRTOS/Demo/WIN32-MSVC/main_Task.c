/*
FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
All rights reserved

VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

This file is part of the FreeRTOS distribution.

FreeRTOS is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (version 2) as published by the
Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

***************************************************************************
>>!   NOTE: The modification to the GPL is included to allow you to     !<<
>>!   distribute a combined work that includes FreeRTOS without being   !<<
>>!   obliged to provide the source code for proprietary components     !<<
>>!   outside of the FreeRTOS kernel.                                   !<<
***************************************************************************

FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  Full license text is available on the following
link: http://www.freertos.org/a00114.html

***************************************************************************
*                                                                       *
*    FreeRTOS provides completely free yet professionally developed,    *
*    robust, strictly quality controlled, supported, and cross          *
*    platform software that is more than just the market leader, it     *
*    is the industry's de facto standard.                               *
*                                                                       *
*    Help yourself get started quickly while simultaneously helping     *
*    to support the FreeRTOS project by purchasing a FreeRTOS           *
*    tutorial book, reference manual, or both:                          *
*    http://www.FreeRTOS.org/Documentation                              *
*                                                                       *
***************************************************************************

http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
the FAQ page "My application does not run, what could be wrong?".  Have you
defined configASSERT()?

http://www.FreeRTOS.org/support - In return for receiving this top quality
embedded software for free we request you assist our global community by
participating in the support forum.

http://www.FreeRTOS.org/training - Investing in training allows your team to
be as productive as possible as early as possible.  Now you can receive
FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
Ltd, and the world's leading authority on the world's leading RTOS.

http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
including FreeRTOS+Trace - an indispensable productivity tool, a DOS
compatible FAT file system, and our tiny thread aware UDP/IP stack.

http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
licenses offer ticketed support, indemnification and commercial middleware.

http://www.SafeRTOS.com - High Integrity Systems also provide a safety
engineered and independently SIL3 certified version for use in safety and
mission critical applications that require provable dependability.

1 tab == 4 spaces!
*/

/******************************************************************************
* NOTE 1: Do not expect to get real time behaviour from the Win32 port or this
* demo application.  It is provided as a convenient development and
* demonstration test bed only.  Windows will not be running the FreeRTOS
* threads continuously, so the timing information in the FreeRTOS+Trace logs
* have no meaningful units.  See the documentation page for the Windows
* simulator for further explanation:
* http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
* - READ THE WEB DOCUMENTATION FOR THIS PORT FOR MORE INFORMATION ON USING IT -
*
* NOTE 2:  This project provides two demo applications.  A simple blinky style
* project, and a more comprehensive test and demo application.  The
* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
* between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
* in main.c.  This file implements the simply blinky style version.
*
* NOTE 3:  This file only contains the source code that is specific to the
* basic demo.  Generic functions, such FreeRTOS hook functions, are defined
* in main.c.
******************************************************************************
*
* main_Task1() creates one queue, one software timer, and two tasks.  It then
* starts the scheduler.
*
*/

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Priorities at which the tasks are created. */
#define	mainQUEUE_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The times are converted from
milliseconds to ticks by the pdMS_TO_TICKS() macro where they are used. */
#define mainDELAY_LOOP_COUNT				10000000

/* The number of items the queue can hold. */
#define mainQUEUE_LENGTH					( 2 )

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define mainQUEUE_PARAMETER			( 0x0000UL )

/*-----------------------------------------------------------*/

extern void vPrintString(const portCHAR *pcString);
extern void vPrintStringAndNumber(const portCHAR *pcString, portLONG lValue);

/*-----------------------------------------------------------*/

/*
* The tasks as described in the comments at the top of this file.
*/
static void vTask1(void *pvParameters);
static void vTask2(void *pvParameters);
static void vPeriodicTask(void *pvParameters);

/*-----------------------------------------------------------*/

static const char *pcTextForTask1 = "Pass Parameter, Task 1 is running\r\n";
static const char *pcTextForTask2 = "Pass Parameter, Task 2 is running\t\n";
static const char *pcTextForPeriodicTask = "Pass Parameter, PeriodicTask is running";

/*-----------------------------------------------------------*/
static TaskHandle_t xTask1Handle = NULL;
static TaskHandle_t xTask2Handle = NULL;
static TaskHandle_t xPeriodicTaskHandle = NULL;
/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

void main_Task(void)
{
	/* Start the two tasks as described in the comments at the top of this file. */
	xTaskCreate(vTask1,							/* The function that implements the task. */
		"vTask1", 								/* The text name assigned to the task - for debug only as it is not used by the kernel. */
		configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
		(void *)pcTextForTask1,					/* The parameter passed to the task - just to check the functionality. */
		mainQUEUE_TASK_PRIORITY, 				/* The priority assigned to the task. */
		NULL);									/* The task handle is not required, so NULL is passed. */

	//xTaskCreate(vTask2, "vTask2", configMINIMAL_STACK_SIZE, (void *)pcTextForTask2, mainQUEUE_TASK_PRIORITY , NULL);
	//xTaskCreate(vPeriodicTask, "vPeriodicTask", configMINIMAL_STACK_SIZE, (void *)pcTextForPeriodicTask, mainQUEUE_TASK_PRIORITY + 1, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for (;;);
}
/*-----------------------------------------------------------*/

static void vTask1(void *pvParameters)
{
	const char *pcLTaskName = "Local Task Name 1, running\r\n";
	char *pcPTaskName = (char *)pvParameters;
	//volatile unsigned long ul;

	/* 和大多数任务一样，该任务处于一个死循环中。 */
	for (;;)
	{
		/* Print out the name of this task. */
		vPrintString(pcLTaskName);
		vPrintString(pcPTaskName);

		/* 延迟，以产生一个周期 */
		//for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		//{
			/* 这个空循环是最原始的延迟实现方式。在循环中不做任何事情。后面的示例程序将采用
			delay/sleep函数代替这个原始空循环。 */
		//}
		xTaskCreate(vTask2, "vTask2", configMINIMAL_STACK_SIZE, pcTextForTask2, mainQUEUE_TASK_PRIORITY + 1, &xTask2Handle);
		vTaskDelay( 500 / portTICK_RATE_MS );
		taskYIELD();
	}
}

/*-----------------------------------------------------------*/

static void vTask2(void *pvParameters)
{
	const char *pcLTaskName = "Local Task Name 2, running\r\n";
	char *pcPTaskName = (char *)pvParameters;
	const char *delTaskInfo = "Task2 is running and about to delete itself\r\n";
	//volatile unsigned long ul;

	/* 和大多数任务一样，该任务处于一个死循环中。 */
	for (;;)
	{
		/* Print out the name of this task. */
		vPrintString(pcLTaskName);
		vPrintString(pcPTaskName);

		/* 延迟，以产生一个周期 */
		//for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		//{
			/* 这个空循环是最原始的延迟实现方式。在循环中不做任何事情。后面的示例程序将采用
			delay/sleep函数代替这个原始空循环。 */
		//}
		
		//vTaskDelay( 500 / portTICK_RATE_MS );
		vPrintString(delTaskInfo);
		vTaskDelete(xTask2Handle);
		taskYIELD();
	}
}

/*-----------------------------------------------------------*/
extern long ulIdleCycleCount;
static void vPeriodicTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	char* pcTaskName = (void *)pvParameters;

	/* 初始化xLastWakeTime,之后会在vTaskDelayUntil()中自动更新。 */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;;)
	{
		/* Print out the name of this task. */
		vPrintString("Periodic task is running\r\n");
		vPrintStringAndNumber(pcTaskName, ulIdleCycleCount);

		/* The task should execute every 10 milliseconds exactly. */
		vTaskDelayUntil(&xLastWakeTime, ( 100 / portTICK_RATE_MS ));
	}
}

/*-----------------------------------------------------------*/
