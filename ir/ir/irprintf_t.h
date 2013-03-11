/*
 * Copyright (C) 1995-2008 University of Karlsruhe.  All right reserved.
 *
 * This file is part of libFirm.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * Licensees holding valid libFirm Professional Edition licenses may use
 * this file in accordance with the libFirm Commercial License.
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

/**
 * @file
 * @brief   A little printf understanding some firm types.
 * @author  Sebastian Hack
 * @date    29.11.2004
 */
#ifndef FIRM_IR_IRPRINTF_T_H
#define FIRM_IR_IRPRINTF_T_H

#include "irprintf.h"

#ifdef DEBUG_libfirm

#define ir_debugf    ir_printf
#define ir_fdebugf   ir_fprintf

#else

static inline void ir_debugf(const char *fmt, ...)
{
	(void) fmt;
}

static inline void ir_fdebugf(FILE *f, const char *fmt, ...)
{
	(void) f;
	(void) fmt;
}

#endif

#endif