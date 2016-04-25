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

/* ������д��ݵĽṹ���͡� */
typedef struct
{
	unsigned char ucValue;
	unsigned char ucSource;
} xData;

/* ��������xData���͵ı�����ͨ�����н��д��ݡ� */
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
	/* �����Ķ������ڱ������5��ֵ��ÿ�����ݵ�Ԫ�����㹻�Ŀռ����洢һ��long�ͱ��� */
	xQueue = xQueueCreate(5, sizeof(long));
	if (xQueue != NULL)
	{
		if (RECEIVE_BLOCK)
		{
			/* ��������д��������ʵ����������ڲ������ڴ��ݷ��͵����е�ֵ������һ��ʵ����ͣ�������з���
			100������һ������ʵ����ͣ�������з���200��������������ȼ�����Ϊ1�� */
			xTaskCreate(vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, (void *)100, 1, NULL);
			xTaskCreate(vSenderTask, "Sender2", configMINIMAL_STACK_SIZE, (void *)200, 1, NULL);

			/* ����һ������������ʵ���������ȼ���Ϊ2������д�������ȼ� */
			xTaskCreate(vReceiverTask, "Receiver", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

			/* ����������������ʼִ�� */
			vTaskStartScheduler();
		}
		else
		{
			/* Ϊд�������񴴽�2��ʵ���� The
			������ڲ������ڴ��ݷ��͵������е����ݡ��������һ��������������һֱд��
			xStructsToSend[0]������һ������������һֱд��xStructsToSend[1]����������������ȼ���
			��Ϊ2�����ڶ�������������ȼ� */
			xTaskCreate(vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, &(xStructsToSend[0]), 2, NULL);
			xTaskCreate(vSenderTask, "Sender2", configMINIMAL_STACK_SIZE, &(xStructsToSend[1]), 2, NULL);

			/* �������������񡣶������������ȼ���Ϊ1������д������������ȼ��� */
			xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 1, NULL);

			/* ����������������������õ�ִ�С� */
			vTaskStartScheduler();
		}
	}
	else
	{
		/* ���д���ʧ��*/
	}

	/* ���һ��������main()������Ӧ�û�ִ�е���������ִ�е�����ܿ������ڴ�ѿռ䲻�㵼�¿���
	�����޷��������������н�����������ڴ���������Ϣ */
	for (;;);
}
/*-----------------------------------------------------------*/

static void vSenderTask(void *pvParameters)
{
	long lValueToSend;
	portBASE_TYPE xStatus;

	/* ������ᱻ��������ʵ��������д����е�ֵͨ��������ڲ������� �C ���ַ�ʽʹ��ÿ��ʵ��ʹ�ò�ͬ��
	ֵ�����д���ʱָ�������ݵ�ԪΪlong�ͣ����԰���ڲ���ǿ��ת��Ϊ���ݵ�ԪҪ������� */
	lValueToSend = (long)pvParameters;

	if (RECEIVE_BLOCK)
	{
		/* �ʹ��������һ����������Ҳ����һ����ѭ���� */
		for (;;)
		{
			/* �����з�������
			��һ��������Ҫд��Ķ��С������ڵ���������֮ǰ�ͱ������ˣ��������ڴ�����ִ�С�

			�ڶ��������Ǳ��������ݵĵ�ַ�������м�����lValueToSend�ĵ�ַ��

			������������������ʱʱ�� �C ��������ʱ������ת������״̬�Եȴ����пռ���Ч��������û���趨��
			ʱʱ�䣬��Ϊ�˶��о����ᱣ���г���һ�����ݵ�Ԫ�Ļ��ᣬ����Ҳ����������
			*/
			xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);
			if (xStatus != pdPASS)
			{
				/* ���Ͳ������ڶ��������޷���� �C ���Ȼ���ڴ�����Ϊ�����еĶ��в��������� */
				vPrintString("Could not send to the queue.\r\n");
			}

			/* ����������������ִ�С� taskYIELD()֪ͨ���������ھ��л����������񣬶����صȵ��������ʱ��Ƭ�ľ� */
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
			�ڶ��������ǽ�Ҫ���͵����ݽṹ��ַ�������ַ�Ǵ�������ڲ����д��룬����ֱ��ʹ��
			pvParameters.

			������������������ʱʱ�� �C ��������ʱ������ת������̬�ȴ����пռ���Ч���ʱ�䡣ָ����ʱʱ
			������Ϊд������������ȼ����ڶ���������ȼ������Զ�����Ԥ��һ���ܿ�д����д��������ͻ�ת��
			����̬����ʱ����������Ż����ִ�У����ܴӶ����а����ݶ��ߡ� */
			xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);
			if (xStatus != pdPASS)
			{
				/* д���������޷�������д����У�ֱ��100���볬ʱ��
				���Ȼ���ڴ�����ΪֻҪд���������������̬������������ͻ�õ�ִ�У��Ӷ��������ݣ���
				���ռ� */
				vPrintString("Could not send to the queue.\r\n");
			}

			/* ������д��������õ�ִ�С� */
			taskYIELD();
		}

	}
}

/*-----------------------------------------------------------*/

static void vReceiverTask(void *pvParameters)
{
	/* �������������ڱ���Ӷ����н��յ������ݡ� */
	long lReceivedValue;
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
	(void*)pvParameters;

	if (RECEIVE_BLOCK)
	{
		/* ��������Ȼ������ѭ���С� */
		for (;;)
		{
			/* �˵��ûᷢ�ֶ���һֱΪ�գ���Ϊ����������ɾ����д����е����ݵ�Ԫ�� */
			if (uxQueueMessagesWaiting(xQueue) == 0)
			{
				vPrintString("Queue should have been empty!\r\n");
			}

			/* �Ӷ����н�������
			��һ�������Ǳ���ȡ�Ķ��С������ڵ���������֮ǰ�ͱ������ˣ��������ڴ�����ִ�С�

			�ڶ��������Ǳ�����յ������ݵĻ�������ַ�������м�����lReceivedValue�ĵ�ַ���˱���������
			�������ݵ�Ԫ������ͬ���������㹻�Ĵ�С���洢���յ������ݡ�

			������������������ʱʱ�� �C �����п�ʱ������ת������״̬�Եȴ�����������Ч�������г���
			portTICK_RATE_MS������100�������ʱ��ת��Ϊ��ϵͳ����Ϊ��λ��ʱ��ֵ��
			*/
			xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);
			if (xStatus == pdPASS)
			{
				/* �ɹ��������ݣ���ӡ������ */
				vPrintStringAndNumber("Received = ", lReceivedValue);
			}
			else
			{
				/* �ȴ�100msҲû���յ��κ����ݡ�
				��Ȼ���ڴ�����Ϊ���������ڲ�ͣ����������д������ */
				vPrintString("Could not receive from the queue.\r\n");
			}
		}
	}
	else
	{
		/* �����ṹ������Ա���Ӷ����ж��������ݵ�Ԫ */
		xData xReceivedStructure;
		portBASE_TYPE xStatus;
		/* This task is also defined within an infinite loop. */
		for (;;)
		{
			/* ��������������ȼ���ͣ�������ֻ������д������������ʱ�õ�ִ�С���д��������ֻ���ڶ���д
			��ʱ�Ż��������̬�����Զ���������ִ��ʱ���п϶����������Զ��������ݵ�Ԫ�ĸ���Ӧ�����ڶ��е�
			��� �C �����ж������Ϊ3 */
			if (uxQueueMessagesWaiting(xQueue) != 3)
			{
				vPrintString("Queue should have been full!\r\n");
			}
			/* Receive from the queue.
			�ڶ��������Ǵ�Ž������ݵĻ���ռ䡣�����򵥵ز���һ�������㹻�ռ��С�ı����ĵ�ַ��

			������������������ʱʱ�� �C ��������Ҫָ����ʱʱ�䣬��Ϊ�������λ�ֻ���ڶ�����ʱ�Ż�õ�ִ�У�
			�ʶ���������пն����� */
			xStatus = xQueueReceive(xQueue, &xReceivedStructure, 0);
			if (xStatus == pdPASS)
			{
				/* ���ݳɹ���������ӡ�����ֵ��������Դ�� */
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
				/* û�ж����κ����ݡ���һ���Ƿ����˴�����Ϊ������ֻ֧�ڶ�����ʱ�Ż�õ�ִ�� */
				vPrintString("Could not receive from the queue.\r\n");
			}
		}
	}
	
}

/*-----------------------------------------------------------*/
