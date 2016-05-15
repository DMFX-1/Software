/*
 * DMFX_debug.h
 *
 *  Created on: Dec 12, 2015
 *      Author: Diego de la Cruz
 */

#ifndef DMFX_DEBUG_H_
#define DMFX_DEBUG_H_

#define 	DEBUG				9
#define		STS_ENABLE
#undef  	THRLOAD_ENABLE
#ifdef THRLOAD_ENABLE
// DSP/BIOS modules
#include <log.h>
#include <tsk.h>
#include <mem.h>
#include <sys.h>
#include <thrload.h>
#include <thrload_defs.h>
#endif

#endif /* DMFX_DEBUG_H_ */
