/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : task_test.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_test_attribute.h"
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_TEST_CNT_INIT		0ul
#define G_TASK_TEST_TICK_CNT_INI	0ul

#define DEL_TEST_XX_MIN				0ul
#define DEL_TEST_XX_MED				500ul
#define DEL_TEST_XX_MAX				1000ul

/********************** internal data declaration ****************************/
task_test_dta_t task_test_dta =
	{DEL_TEST_XX_MIN};

/********************** internal functions declaration ***********************/
void task_test_statechart(void);

/********************** internal data definition *****************************/
const char *p_task_test 		= "Task Test (Test Code Integration)";
const char *p_task_test_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_test_cnt;
volatile uint32_t g_task_test_tick_cnt;

/********************** external functions definition ************************/
void task_test_init(void *parameters)
{
	task_test_dta_t 	*p_task_test_dta;
	uint32_t			tick;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_test_init), p_task_test);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_test), p_task_test_);

	/* Init & Print out: Task execution counter */
	g_task_test_cnt = G_TASK_TEST_CNT_INIT;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_test_cnt), g_task_test_cnt);

	/* Update Task Test Configuration & Data Pointer */
	p_task_test_dta = &task_test_dta;

	tick = DEL_TEST_XX_MAX;
	p_task_test_dta->tick = tick;
	LOGGER_INFO("   %s = %lu", GET_NAME(tick), (uint32_t)tick);

	/* Init & Print out: LCD Display */
	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    displayCharPositionWrite(0, 0);
	displayStringWrite("TdSE Bienvenidos");

	displayCharPositionWrite(0, 1);
	displayStringWrite("Test Nro: ");
}

void task_test_update(void *parameters)
{
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_TEST_TICK_CNT_INI < g_task_test_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_test_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_test_cnt++;

		/* Run Task Statechart */
    	task_test_statechart();

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_TEST_TICK_CNT_INI < g_task_test_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_test_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
    }
}

void task_test_statechart(void)
{
	task_test_dta_t *p_task_test_dta;
	char test_str[8];

	/* Update Task Test Configuration & Data Pointer */
    p_task_test_dta = &task_test_dta;

    if (DEL_TEST_XX_MIN < p_task_test_dta->tick)
	{
		p_task_test_dta->tick--;
	}
	else
	{
		p_task_test_dta->tick = DEL_TEST_XX_MAX ;

		/* Print out: LCD Display */
		snprintf(test_str, sizeof(test_str), "%lu", (g_task_test_cnt/1000ul));
		displayCharPositionWrite(10, 1);
		displayStringWrite(test_str);
	}
}

/********************** end of file ******************************************/
