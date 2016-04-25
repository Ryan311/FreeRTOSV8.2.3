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
* main_Queue()		Queue test
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
static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);

/*-----------------------------------------------------------*/

#define mainSENDER_1 1
#define mainSENDER_2 2

/* 定义队列传递的结构类型。 */
typedef struct
{
	unsigned char ucValue;
	unsigned char ucSource;
} xData;

/* 声明两个xData类型的变量，通过队列进行传递。 */
static const xData xStructsToSend[2] =
{
	{ 100, mainSENDER_1 },  /* Used by Sender1. */
	{ 200, mainSENDER_2 }   /* Used by Sender2. */
};

/*-----------------------------------------------------------*/

static TaskHandle_t xSendTaskHandle = NULL;
static TaskHandle_t xRecvTaskHandle = NULL;
static QueueHandle_t xQueue = NULL;

#define RECEIVE_BLOCK 1	//1 Receiver block ;  0 Sender block

void main_Queue(void)
{
	/* 创建的队列用于保存最多5个值，每个数据单元都有足够的空间来存储一个long型变量 */
	xQueue = xQueueCreate(5, sizeof(long));
	if (xQueue != NULL)
	{
		if (RECEIVE_BLOCK)
		{
			/* 创建两个写队列任务实例，任务入口参数用于传递发送到队列的值。所以一个实例不停地往队列发送
			100，而另一个任务实例不停地往队列发送200。两个任务的优先级都设为1。 */
			xTaskCreate(vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, (void *)100, 1, NULL);
			xTaskCreate(vSenderTask, "Sender2", configMINIMAL_STACK_SIZE, (void *)200, 1, NULL);

			/* 创建一个读队列任务实例。其优先级设为2，高于写任务优先级 */
			xTaskCreate(vReceiverTask, "Receiver", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

			/* 启动调度器，任务开始执行 */
			vTaskStartScheduler();
		}
		else
		{
			/* 为写队列任务创建2个实例。 The
			任务入口参数用于传递发送到队列中的数据。因此其中一个任务往队列中一直写入
			xStructsToSend[0]，而另一个则往队列中一直写入xStructsToSend[1]。这两个任务的优先级都
			设为2，高于读队列任务的优先级 */
			xTaskCreate(vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, &(xStructsToSend[0]), 2, NULL);
			xTaskCreate(vSenderTask, "Sender2", configMINIMAL_STACK_SIZE, &(xStructsToSend[1]), 2, NULL);

			/* 创建读队列任务。读队列任务优先级设为1，低于写队列任务的优先级。 */
			xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 1, NULL);

			/* 启动调度器，创建的任务得到执行。 */
			vTaskStartScheduler();
		}
	}
	else
	{
		/* 队列创建失败*/
	}

	/* 如果一切正常，main()函数不应该会执行到这里。但如果执行到这里，很可能是内存堆空间不足导致空闲
	任务无法创建。第五章有讲述更多关于内存管理方面的信息 */
	for (;;);
}
/*-----------------------------------------------------------*/

static void vSenderTask(void *pvParameters)
{
	long lValueToSend;
	portBASE_TYPE xStatus;

	/* 该任务会被创建两个实例，所以写入队列的值通过任务入口参数传递 – 这种方式使得每个实例使用不同的
	值。队列创建时指定其数据单元为long型，所以把入口参数强制转换为数据单元要求的类型 */
	lValueToSend = (long)pvParameters;

	if (RECEIVE_BLOCK)
	{
		/* 和大多数任务一样，本任务也处于一个死循环中 */
		for (;;)
		{
			/* 往队列发送数据
			第一个参数是要写入的队列。队列在调度器启动之前就被创建了，所以先于此任务执行。

			第二个参数是被发送数据的地址，本例中即变量lValueToSend的地址。

			第三个参数是阻塞超时时间 – 当队列满时，任务转入阻塞状态以等待队列空间有效。本例中没有设定超
			时时间，因为此队列决不会保持有超过一个数据单元的机会，所以也决不会满。
			*/
			xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);
			if (xStatus != pdPASS)
			{
				/* 发送操作由于队列满而无法完成 – 这必然存在错误，因为本例中的队列不可能满。 */
				vPrintString("Could not send to the queue.\r\n");
			}

			/* 允许其它发送任务执行。 taskYIELD()通知调度器现在就切换到其它任务，而不必等到本任务的时间片耗尽 */
			taskYIELD();
		}
	}
	else
	{// Send Block
		const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

		/* As per most tasks, this task is implemented within an infinite loop. */
		for (;;)
		{
			/* Send to the queue.
			第二个参数是将要发送的数据结构地址。这个地址是从任务入口参数中传入，所以直接使用
			pvParameters.

			第三个参数是阻塞超时时间 – 当队列满时，任务转入阻塞态等待队列空间有效的最长时间。指定超时时
			间是因为写队列任务的优先级高于读任务的优先级。所以队列如预期一样很快写满，写队列任务就会转入
			阻塞态，此时读队列任务才会得以执行，才能从队列中把数据读走。 */
			xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);
			if (xStatus != pdPASS)
			{
				/* 写队列任务无法将数据写入队列，直至100毫秒超时。
				这必然存在错误，因为只要写队列任务进入阻塞态，读队列任务就会得到执行，从而读走数据，腾
				出空间 */
				vPrintString("Could not send to the queue.\r\n");
			}

			/* 让其他写队列任务得到执行。 */
			taskYIELD();
		}

	}
}

/*-----------------------------------------------------------*/

static void vReceiverTask(void *pvParameters)
{
	/* 声明变量，用于保存从队列中接收到的数据。 */
	long lReceivedValue;
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
	(void*)pvParameters;

	if (RECEIVE_BLOCK)
	{
		/* 本任务依然处于死循环中。 */
		for (;;)
		{
			/* 此调用会发现队列一直为空，因为本任务将立即删除刚写入队列的数据单元。 */
			if (uxQueueMessagesWaiting(xQueue) == 0)
			{
				vPrintString("Queue should have been empty!\r\n");
			}

			/* 从队列中接收数据
			第一个参数是被读取的队列。队列在调度器启动之前就被创建了，所以先于此任务执行。

			第二个参数是保存接收到的数据的缓冲区地址，本例中即变量lReceivedValue的地址。此变量类型与
			队列数据单元类型相同，所以有足够的大小来存储接收到的数据。

			第三个参数是阻塞超时时间 – 当队列空时，任务转入阻塞状态以等待队列数据有效。本例中常量
			portTICK_RATE_MS用来将100毫秒绝对时间转换为以系统心跳为单位的时间值。
			*/
			xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);
			if (xStatus == pdPASS)
			{
				/* 成功读出数据，打印出来。 */
				vPrintStringAndNumber("Received = ", lReceivedValue);
			}
			else
			{
				/* 等待100ms也没有收到任何数据。
				必然存在错误，因为发送任务在不停地往队列中写入数据 */
				vPrintString("Could not receive from the queue.\r\n");
			}
		}
	}
	else
	{
		/* 声明结构体变量以保存从队列中读出的数据单元 */
		xData xReceivedStructure;
		portBASE_TYPE xStatus;
		/* This task is also defined within an infinite loop. */
		for (;;)
		{
			/* 读队列任务的优先级最低，所以其只可能在写队列任务阻塞时得到执行。而写队列任务只会在队列写
			满时才会进入阻塞态，所以读队列任务执行时队列肯定已满。所以队列中数据单元的个数应当等于队列的
			深度 – 本例中队列深度为3 */
			if (uxQueueMessagesWaiting(xQueue) != 3)
			{
				vPrintString("Queue should have been full!\r\n");
			}
			/* Receive from the queue.
			第二个参数是存放接收数据的缓存空间。本例简单地采用一个具有足够空间大小的变量的地址。

			第三个参数是阻塞超时时间 – 本例不需要指定超时时间，因为读队列任会只会在队列满时才会得到执行，
			故而不会因队列空而阻塞 */
			xStatus = xQueueReceive(xQueue, &xReceivedStructure, 0);
			if (xStatus == pdPASS)
			{
				/* 数据成功读出，打印输出数值及数据来源。 */
				if (xReceivedStructure.ucSource == mainSENDER_1)
				{
					vPrintStringAndNumber("From Sender 1 = ", xReceivedStructure.ucValue);
				}
				else
				{
					vPrintStringAndNumber("From Sender 2 = ", xReceivedStructure.ucValue);
				}
			}
			else
			{
				/* 没有读到任何数据。这一定是发生了错误，因为此任务只支在队列满时才会得到执行 */
				vPrintString("Could not receive from the queue.\r\n");
			}
		}
	}
	
}

/*-----------------------------------------------------------*/
