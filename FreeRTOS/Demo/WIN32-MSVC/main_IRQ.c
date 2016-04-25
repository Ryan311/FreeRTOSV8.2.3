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
* main_IRQ() :  set software irq handler not work
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
#include "portmacro.h"

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

static void vPeriodicTask(void *pvParameters);
static void vHandlerTask(void *pvParameters);
static uint32_t vExampleInterruptHandler(void);

/*-----------------------------------------------------------*/

static TaskHandle_t xPeriodicTaskHandle = NULL;
static SemaphoreHandle_t xBinarySemaphore;


void main_IRQ(void)
{
	/* 信号量在使用前都必须先创建。本例中创建了一个二值信号量 */
	vSemaphoreCreateBinary(xBinarySemaphore);

	/* 安装中断服务例程 */
	//vPortSetInterruptHandler( 1, vExampleInterruptHandler);

	/* 检查信号量是否成功创建 */
	if (xBinarySemaphore != NULL)
	{
		/* 创建延迟处理任务。此任务将与中断同步。延迟处理任务在创建时,使用了一个较高的优先级，以保证
		中断退出后会被立即执行。在本例中，为延迟处理任务赋予优先级3 */
		xTaskCreate(vHandlerTask, "Handler", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

		/* 创建一个任务用于周期性产生软件中断。此任务的优先级低于延迟处理任务。每当延迟处理任务切出
		阻塞态，就会抢占周期任务*/
		xTaskCreate(vPeriodicTask, "Periodic", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

	/* 如果一切正常，main()函数不会执行到这里，因为调度器已经开始运行任务。但如果程序运行到了这里，
	很可能是由于系统内存不足而无法创建空闲任务。第五章会提供更多关于内存管理的信息 */
	for (;;);
}
/*-----------------------------------------------------------*/

static void vPeriodicTask(void *pvParameters)
{
	(void*)pvParameters;
	for (;;)
	{
		/* 此任务通过每500毫秒产生一个软件中断来”模拟”中断事件 */
		vTaskDelay(500 / portTICK_RATE_MS);
		/* 产生中断，并在产生之前和之后输出信息，以便在执行结果中直观直出执行流程 */
		vPrintString("Periodic task - About to generate an interrupt.\r\n");
		//__asm{ int 0x00 } /* 这条语句产生中断 */
		//vPortGenerateSimulatedInterrupt(1);
		vPrintString("Periodic task - Interrupt generated.\r\n\r\n\r\n");
	}
}

static void vHandlerTask(void *pvParameters)
{
	(void *)pvParameters;
	/* As per most tasks, this task is implemented within an infinite loop. */
	for (;;)
	{
		/* 使用信号量等待一个事件。信号量在调度器启动之前，也即此任务执行之前就已被创建。任务被无超
		时阻塞，所以此函数调用也只会在成功获取信号量之后才会返回。此处也没有必要检测返回值 */
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

		/* 程序运行到这里时，事件必然已经发生。本例的事件处理只是简单地打印输出一个信息 */
		vPrintString("Handler task - Processing event.\r\n");
	}
}

static uint32_t vExampleInterruptHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	/* 'Give' the semaphore to unblock the task. */
	xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken == pdTRUE)
	{
		/* 给出信号量以使得等待此信号量的任务解除阻塞。如果解出阻塞的任务的优先级高于当前任务的优先
		级 – 强制进行一次任务切换，以确保中断直接返回到解出阻塞的任务(优选级更高)。

		说明：在实际使用中，ISR中强制上下文切换的宏依赖于具体移植。此处调用的是基于Open Watcom DOS
		移植的宏。其它平台下的移植可能有不同的语法要求。对于实际使用的平台，请参如数对应移植自带的示
		例程序，以决定正确的语法和符号。
		*/
		//portSWITCH_CONTEXT();
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	return xHigherPriorityTaskWoken;
}

/*-----------------------------------------------------------*/
