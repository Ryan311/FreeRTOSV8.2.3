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
	/* �ź�����ʹ��ǰ�������ȴ����������д�����һ����ֵ�ź��� */
	vSemaphoreCreateBinary(xBinarySemaphore);

	/* ��װ�жϷ������� */
	//vPortSetInterruptHandler( 1, vExampleInterruptHandler);

	/* ����ź����Ƿ�ɹ����� */
	if (xBinarySemaphore != NULL)
	{
		/* �����ӳٴ������񡣴��������ж�ͬ�����ӳٴ��������ڴ���ʱ,ʹ����һ���ϸߵ����ȼ����Ա�֤
		�ж��˳���ᱻ����ִ�С��ڱ����У�Ϊ�ӳٴ������������ȼ�3 */
		xTaskCreate(vHandlerTask, "Handler", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

		/* ����һ���������������Բ�������жϡ�����������ȼ������ӳٴ�������ÿ���ӳٴ��������г�
		����̬���ͻ���ռ��������*/
		xTaskCreate(vPeriodicTask, "Periodic", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

	/* ���һ��������main()��������ִ�е������Ϊ�������Ѿ���ʼ�������񡣵�����������е������
	�ܿ���������ϵͳ�ڴ治����޷������������񡣵����»��ṩ��������ڴ�������Ϣ */
	for (;;);
}
/*-----------------------------------------------------------*/

static void vPeriodicTask(void *pvParameters)
{
	(void*)pvParameters;
	for (;;)
	{
		/* ������ͨ��ÿ500�������һ������ж�����ģ�⡱�ж��¼� */
		vTaskDelay(500 / portTICK_RATE_MS);
		/* �����жϣ����ڲ���֮ǰ��֮�������Ϣ���Ա���ִ�н����ֱ��ֱ��ִ������ */
		vPrintString("Periodic task - About to generate an interrupt.\r\n");
		//__asm{ int 0x00 } /* �����������ж� */
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
		/* ʹ���ź����ȴ�һ���¼����ź����ڵ���������֮ǰ��Ҳ��������ִ��֮ǰ���ѱ������������޳�
		ʱ���������Դ˺�������Ҳֻ���ڳɹ���ȡ�ź���֮��Ż᷵�ء��˴�Ҳû�б�Ҫ��ⷵ��ֵ */
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

		/* �������е�����ʱ���¼���Ȼ�Ѿ��������������¼�����ֻ�Ǽ򵥵ش�ӡ���һ����Ϣ */
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
		/* �����ź�����ʹ�õȴ����ź�������������������������������������ȼ����ڵ�ǰ���������
		�� �C ǿ�ƽ���һ�������л�����ȷ���ж�ֱ�ӷ��ص��������������(��ѡ������)��

		˵������ʵ��ʹ���У�ISR��ǿ���������л��ĺ������ھ�����ֲ���˴����õ��ǻ���Open Watcom DOS
		��ֲ�ĺꡣ����ƽ̨�µ���ֲ�����в�ͬ���﷨Ҫ�󡣶���ʵ��ʹ�õ�ƽ̨�����������Ӧ��ֲ�Դ���ʾ
		�������Ծ�����ȷ���﷨�ͷ��š�
		*/
		//portSWITCH_CONTEXT();
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	return xHigherPriorityTaskWoken;
}

/*-----------------------------------------------------------*/
