/* -*- c -*- */

/*
   Project:     libFIRM
   File name:   ir/ana/pto_ctx.h
   Purpose:     ...
   Author:      Florian
   Modified by:
   Created:     Sat Nov 13 19:35:27 CET 2004
   CVS-ID:      $Id$
   Copyright:   (c) 1999-2004 Universitšt Karlsruhe
   Licence:     This file is protected by the GPL -  GNU GENERAL PUBLIC LICENSE.
*/


# ifndef _PTO_CTX_
# define _PTO_CTX_

# include "ecg.h"

/* ===================================================
   Global Defines:
   =================================================== */

/* ===================================================
 Global Data Types:
 =================================================== */

/* ===================================================
   Global Prototypes:
   =================================================== */
/* Find the appropriate ctx for the given call and the given graph */
/* ctx_info_t *find_ctx (ir_node*, graph_info_t*, ctx_info_t*); */
int find_ctx_idx (ir_node*, graph_info_t*, ctx_info_t*);

/* Get the current ctx */
ctx_info_t *get_curr_ctx (void);

/* Set the current ctx to the given ctx.  Return the old value */
ctx_info_t *set_curr_ctx (ctx_info_t*);

/* ===================================================
   Global Variables:
   =================================================== */


# endif /* not defined _PTO_CTX_ */



/*
  $Log$
  Revision 1.1  2004/11/18 16:37:34  liekweg
  rewritten


*/
