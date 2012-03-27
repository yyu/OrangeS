/*************************************************************************//**
 *****************************************************************************
 * @file   tick.c
 * @brief  get_ticks()
 * @author Forrest Y. Yu
 * @date   2011
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
/**
 * Get the number of ticks of the current proc.
 * 
 * @return The number of ticks.
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}
