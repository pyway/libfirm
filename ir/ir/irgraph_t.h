/*
 * Project:     libFIRM
 * File name:   ir/ir/irgraph.c
 * Purpose:     Entry point to the representation of procedure code -- internal header.
 * Author:      Martin Trapp, Christian Schaefer
 * Modified by: Goetz Lindenmaier
 * Created:
 * CVS-ID:      $Id$
 * Copyright:   (c) 1998-2003 Universitšt Karlsruhe
 * Licence:     This file protected by GPL -  GNU GENERAL PUBLIC LICENSE.
 */

/**
 * @file irgraph_t.h
 *
 * ir graph construction.
 *
 * @author Martin Trapp, Christian Schaefer
 */


#ifndef _IRGRAPH_T_H_
#define _IRGRAPH_T_H_

#include "firm_types.h"
#include "irgraph.h"

#include "firm_common_t.h"
#include "irtypeinfo.h"
#include "irprog.h"
#include "pseudo_irg.h"
#include "type_t.h"
#include "entity_t.h"
#include "typegmod.h"
#include "tr_inheritance.h"

#include "irloop.h"
#include "execution_frequency.h"

#include "obst.h"
#include "pset.h"
#include "set.h"

#define FRAME_TP_SUFFIX "frame_tp"

/**
 * Edge info to put into an irg.
 */
typedef struct _irg_edge_info_t {
	set *edges;
	unsigned activated : 1;
} irg_edge_info_t;

/**
 * Index constants for nodes that can be accessed through the graph itself.
 */
enum irg_anchors {
  anchor_start_block = 0, /**< block the start node will belong to */
  anchor_start,           /**< start node of this ir_graph */
  anchor_end_block,       /**< block the end node will belong to */
  anchor_end,             /**< end node of this ir_graph */
  anchor_end_reg,         /**< end node of this ir_graph */
  anchor_end_except,      /**< end node of this ir_graph */
  anchor_cstore,          /**< constant store -- no more needed!! */
  anchor_frame,           /**< method's frame */
  anchor_globals,         /**< pointer to the data segment containing all
                               globals as well as global procedures. */
  anchor_initial_mem,     /**< initial memory of this graph */
  anchor_args,            /**< methods arguments */
  anchor_bad,             /**< bad node of this ir_graph, the one and
                               only in this graph */
  anchor_no_mem,          /**< NoMem node of this ir_graph, the one and only in this graph */
  anchor_max
};

/** ir_graph holds all information for a procedure */
struct ir_graph {
  firm_kind         kind;            /**<  always set to k_ir_graph*/
  /* --  Basics of the representation -- */
  entity  *ent;           /**< The entity of this procedure, i.e.,
                    the type of the procedure and the
                    class it belongs to. */
  ir_type *frame_type;           /**< A class type representing the stack frame.
                                      Can include "inner" methods. */
  ir_node *anchors[anchor_max];  /**< anchor nodes */
  ir_node **proj_args;           /**< projs of the methods arguments */
  struct obstack *obst;          /**< obstack where all of the ir_nodes live */
  ir_node *current_block;        /**< block for newly gen_*()-erated ir_nodes */
  struct obstack *extbb_obst;    /**< obstack for extended basic block info */

  /* -- Fields for graph properties -- */
  irg_inline_property inline_property;     /**< How to handle inlineing. */
  unsigned additional_properties;          /**< additional graph properties. */

  /* -- Fields indicating different states of irgraph -- */
  irg_phase_state phase_state;       /**< compiler phase */
  op_pin_state irg_pinned_state;     /**< Flag for status of nodes */
  irg_outs_state outs_state;         /**< Out edges. */
  irg_dom_state dom_state;           /**< Dominator state information */
  irg_dom_state pdom_state;          /**< Post Dominator state information */
  ir_typeinfo_state typeinfo_state;        /**< Validity of type information */
  irg_callee_info_state callee_info_state; /**< Validity of callee information */
  irg_loopinfo_state loopinfo_state;       /**< state of loop information */
  exec_freq_state   execfreq_state;        /**< state of execution frequency information */
  ir_class_cast_state class_cast_state;    /**< kind of cast operations in code. */
  irg_extblk_info_state extblk_state;      /**< state of extended basic block info */

  /* -- Fields for construction -- */
#if USE_EXPLICIT_PHI_IN_STACK
  struct Phi_in_stack *Phi_in_stack; /**< needed for automatic Phi construction */
#endif
  int n_loc;                         /**< number of local variable in this
                                          procedure including procedure parameters. */
  void **loc_descriptions;           /**< storage for local variable desriptions */

  /* -- Fields for optimizations / analysis information -- */
  pset *value_table;                 /**< hash table for global value numbering (cse)
                    for optimizing use in iropt.c */
  ir_node **outs;                    /**< Space for the out arrays. */

#ifdef DEBUG_libfirm
  int             n_outs;            /**< Size wasted for outs */
#endif /* defined DEBUG_libfirm */
  ir_loop *loop;                     /**< The outermost loop */
  void *link;                        /**< A void* field to link any information to
                    the node. */

  ir_graph **callers;                /**< For callgraph analysis. */
  unsigned char *caller_isbe;        /**< For callgraph analysis: set if backedge. */
  ir_graph **callees;                /**< For callgraph analysis. */
  unsigned char *callee_isbe;        /**< For callgraph analysis: set if backedge. */
  int        callgraph_loop_depth;         /**< For callgraph analysis */
  int        callgraph_recursion_depth;    /**< For callgraph analysis */
  double     method_execution_frequency;   /**< For callgraph analysis */

  ir_loop   *l;

  /* -- Fields for Walking the graph -- */
  unsigned long visited;             /**< this flag is an identifier for
                    ir walk. it will be incremented
                    every time someone walks through
                    the graph */
  unsigned long block_visited;       /**< same as visited, for a complete block */
  unsigned estimated_node_count;     /**< estimated number of nodes in this graph,
                                          updated after every walk */
#if FIRM_EDGES_INPLACE
  irg_edge_info_t edge_info;  /**< edge info for automatic outs */
#endif
#ifdef DEBUG_libfirm
  long graph_nr;              /**< a unique graph number for each graph to make output
                   readable. */
#endif

};

/**
 * Initializes the graph construction module
 */
void firm_init_irgraph(void);

/* Internal constructor that does not add to irp_irgs or the like. */
ir_graph *new_r_ir_graph (entity *ent, int n_loc);

/** Make a rudimentary ir graph for the constant code.
   Must look like a correct irg, spare everything else. */
ir_graph *new_const_code_irg(void);

/**
 * Set the op_pin_state_pinned state of a graph.
 *
 * @param irg     the IR graph
 * @param p       new pin state
 */
INLINE void
set_irg_pinned (ir_graph *irg, op_pin_state p);

/** Returns the obstack associated with the graph. */
struct obstack *get_irg_obstack(const ir_graph *irg);

/**
 * Returns true if the node n is allocated on the storage of graph irg.
 *
 * @param irg   the IR graph
 * @param n the IR node
 */
int node_is_in_irgs_storage(ir_graph *irg, ir_node *n);

/*-------------------------------------------------------------------*/
/* inline functions for graphs                                       */
/*-------------------------------------------------------------------*/

extern int firm_interprocedural_view;

static INLINE int
_get_interprocedural_view(void) {
  return firm_interprocedural_view;
}

static INLINE int
_is_ir_graph(const void *thing) {
  return (get_kind(thing) == k_ir_graph);
}

/** Returns the start block of a graph. */
static INLINE ir_node *
_get_irg_start_block(const ir_graph *irg) {
  return irg->anchors[anchor_start_block];
}

static INLINE void
_set_irg_start_block(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_start_block] = node;
}

static INLINE ir_node *
_get_irg_start(const ir_graph *irg) {
  return irg->anchors[anchor_start];
}

static INLINE void
_set_irg_start(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_start] = node;
}

static INLINE ir_node *
_get_irg_end_block(const ir_graph *irg) {
  return irg->anchors[anchor_end_block];
}

static INLINE void
_set_irg_end_block(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_end_block] = node;
}

static INLINE ir_node *
_get_irg_end(const ir_graph *irg) {
  return irg->anchors[anchor_end];
}

static INLINE void
_set_irg_end(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_end] = node;
}

static INLINE ir_node *
_get_irg_end_reg(const ir_graph *irg) {
  return irg->anchors[anchor_end_reg];
}

static INLINE ir_node *
_get_irg_end_except (const ir_graph *irg) {
  return irg->anchors[anchor_end_except];
}

static INLINE ir_node *
_get_irg_cstore(const ir_graph *irg) {
  return irg->anchors[anchor_cstore];
}

static INLINE void
_set_irg_cstore(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_cstore] = node;
}

static INLINE ir_node *
_get_irg_frame(const ir_graph *irg) {
  return irg->anchors[anchor_frame];
}

static INLINE void
_set_irg_frame(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_frame] = node;
}

static INLINE ir_node *
_get_irg_globals(const ir_graph *irg) {
  return irg->anchors[anchor_globals];
}

static INLINE void
_set_irg_globals(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_globals] = node;
}

static INLINE ir_node *
_get_irg_initial_mem(const ir_graph *irg) {
  return irg->anchors[anchor_initial_mem];
}

static INLINE void
_set_irg_initial_mem(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_initial_mem] = node;
}

static INLINE ir_node *
_get_irg_args(const ir_graph *irg) {
  return irg->anchors[anchor_args];
}

static INLINE void
_set_irg_args(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_args] = node;
}

static INLINE ir_node **
_get_irg_proj_args(const ir_graph *irg) {
  return irg->proj_args;
}

static INLINE void
_set_irg_proj_args(ir_graph *irg, ir_node **nodes) {
  irg->proj_args = nodes;
}

static INLINE ir_node *
_get_irg_bad(const ir_graph *irg) {
  return irg->anchors[anchor_bad];
}

static INLINE void
_set_irg_bad(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_bad] = node;
}

static INLINE ir_node *
_get_irg_no_mem(const ir_graph *irg) {
  return irg->anchors[anchor_no_mem];
}

static INLINE void
_set_irg_no_mem(ir_graph *irg, ir_node *node) {
  irg->anchors[anchor_no_mem] = node;
}
static INLINE ir_node *
_get_irg_current_block(const ir_graph *irg) {
  return irg->current_block;
}

static INLINE void
_set_irg_current_block(ir_graph *irg, ir_node *node) {
  irg->current_block = node;
}

static INLINE entity *
_get_irg_entity(const ir_graph *irg) {
  assert(irg && irg->ent);
  return irg->ent;
}

static INLINE void
_set_irg_entity(ir_graph *irg, entity *ent) {
  irg->ent = ent;
}

static INLINE ir_type *
_get_irg_frame_type(ir_graph *irg) {
  assert(irg && irg->frame_type);
  return irg->frame_type = skip_tid(irg->frame_type);
}

static INLINE void
_set_irg_frame_type(ir_graph *irg, ir_type *ftp) {
  assert(is_frame_type(ftp));
  irg->frame_type = ftp;
}

static INLINE struct obstack *
_get_irg_obstack(const ir_graph *irg) {
  return irg->obst;
}


static INLINE irg_phase_state
_get_irg_phase_state(const ir_graph *irg) {
  return irg->phase_state;
}

static INLINE void
_set_irg_phase_state(ir_graph *irg, irg_phase_state state) {
  irg->phase_state = state;
}

static INLINE op_pin_state
_get_irg_pinned(const ir_graph *irg) {
  return irg->irg_pinned_state;
}

static INLINE irg_outs_state
_get_irg_outs_state(const ir_graph *irg) {
  return irg->outs_state;
}

static INLINE void
_set_irg_outs_inconsistent(ir_graph *irg) {
  if (irg->outs_state == outs_consistent)
    irg->outs_state = outs_inconsistent;
}

static INLINE irg_extblk_state
_get_irg_extblk_state(const ir_graph *irg) {
  return irg->extblk_state;
}

static INLINE void
_set_irg_extblk_inconsistent(ir_graph *irg) {
  if (irg->extblk_state == extblk_valid)
    irg->extblk_state = extblk_invalid;
}

static INLINE irg_dom_state
_get_irg_dom_state(const ir_graph *irg) {
  return irg->dom_state;
}

static INLINE irg_dom_state
_get_irg_postdom_state(const ir_graph *irg) {
  return irg->pdom_state;
}

static INLINE void
_set_irg_doms_inconsistent(ir_graph *irg) {
  if (irg->dom_state != dom_none)
    irg->dom_state = dom_inconsistent;
  if (irg->pdom_state != dom_none)
    irg->pdom_state = dom_inconsistent;
}

static INLINE irg_loopinfo_state
_get_irg_loopinfo_state(const ir_graph *irg) {
  return irg->loopinfo_state;
}

static INLINE void
_set_irg_loopinfo_state(ir_graph *irg, irg_loopinfo_state s) {
  irg->loopinfo_state = s;
}

static INLINE void
_set_irg_loopinfo_inconsistent(ir_graph *irg) {
  irg->loopinfo_state &= ~loopinfo_valid;
}

static INLINE void
_set_irg_pinned(ir_graph *irg, op_pin_state p) {
  irg->irg_pinned_state = p;
}

static INLINE irg_callee_info_state
_get_irg_callee_info_state(const ir_graph *irg) {
  return irg->callee_info_state;
}

static INLINE void
_set_irg_callee_info_state(ir_graph *irg, irg_callee_info_state s) {
  irg_callee_info_state irp_state = get_irp_callee_info_state();

  irg->callee_info_state = s;

  /* I could compare ... but who knows? */
  if ((irp_state == irg_callee_info_consistent)  ||
      ((irp_state == irg_callee_info_inconsistent) && (s == irg_callee_info_none)))
      set_irp_callee_info_state(s);
}

static INLINE irg_inline_property
_get_irg_inline_property(const ir_graph *irg) {
  return irg->inline_property;
}

static INLINE void
_set_irg_inline_property(ir_graph *irg, irg_inline_property s) {
  irg->inline_property = s;
}

static INLINE unsigned
_get_irg_additional_properties(const ir_graph *irg) {
  if (irg->additional_properties & mtp_property_inherited)
    return get_method_additional_properties(get_entity_type(irg->ent));
  return irg->additional_properties;
}

static INLINE void
_set_irg_additional_properties(ir_graph *irg, unsigned mask) {
  /* do not allow to set the mtp_property_inherited flag or
   * the automatic inheritance of flags will not work */
  irg->additional_properties = mask & ~mtp_property_inherited;
}

static INLINE void
_set_irg_additional_property(ir_graph *irg, mtp_additional_property flag) {
  unsigned prop = irg->additional_properties;

  if (prop & mtp_property_inherited)
    prop = get_method_additional_properties(get_entity_type(irg->ent));
  irg->additional_properties = prop | flag;
}

static INLINE void
_set_irg_link(ir_graph *irg, void *thing) {
  irg->link = thing;
}

static INLINE void *
_get_irg_link(const ir_graph *irg) {
  return irg->link;
}

static INLINE unsigned long
_get_irg_visited(const ir_graph *irg) {
  return irg->visited;
}

static INLINE unsigned long
_get_irg_block_visited(const ir_graph *irg) {
  return irg->block_visited;
}

static INLINE void
_set_irg_block_visited(ir_graph *irg, unsigned long visited) {
  irg->block_visited = visited;
}

static INLINE void
_inc_irg_block_visited(ir_graph *irg) {
  ++irg->block_visited;
}

static INLINE unsigned
_get_irg_estimated_node_cnt(const ir_graph *irg) {
  return irg->estimated_node_count;
}

#define get_interprocedural_view()            _get_interprocedural_view()
#define is_ir_graph(thing)                    _is_ir_graph(thing)
#define get_irg_start_block(irg)              _get_irg_start_block(irg)
#define set_irg_start_block(irg, node)        _set_irg_start_block(irg, node)
#define get_irg_start(irg)                    _get_irg_start(irg)
#define set_irg_start(irg, node)              _set_irg_start(irg, node)
#define get_irg_end_block(irg)                _get_irg_end_block(irg)
#define set_irg_end_block(irg, node)          _set_irg_end_block(irg, node)
#define get_irg_end(irg)                      _get_irg_end(irg)
#define set_irg_end(irg, node)                _set_irg_end(irg, node)
#define get_irg_end_reg(irg)                  _get_irg_end_reg(irg)
#define get_irg_end_except(irg)               _get_irg_end_except(irg)
#define get_irg_cstore(irg)                   _get_irg_cstore(irg)
#define set_irg_cstore(irg, node)             _set_irg_cstore(irg, node)
#define get_irg_frame(irg)                    _get_irg_frame(irg)
#define set_irg_frame(irg, node)              _set_irg_frame(irg, node)
#define get_irg_globals(irg)                  _get_irg_globals(irg)
#define set_irg_globals(irg, node)            _set_irg_globals(irg, node)
#define get_irg_initial_mem(irg)              _get_irg_initial_mem(irg)
#define set_irg_initial_mem(irg, node)        _set_irg_initial_mem(irg, node)
#define get_irg_args(irg)                     _get_irg_args(irg)
#define set_irg_args(irg, node)               _set_irg_args(irg, node)
#define get_irg_bad(irg)                      _get_irg_bad(irg)
#define set_irg_bad(irg, node)                _set_irg_bad(irg, node)
#define get_irg_no_mem(irg)                   _get_irg_no_mem(irg)
#define set_irg_no_mem(irg, node)             _set_irg_no_mem(irg, node)
#define get_irg_current_block(irg)            _get_irg_current_block(irg)
#define set_irg_current_block(irg, node)      _set_irg_current_block(irg, node)
#define get_irg_entity(irg)                   _get_irg_entity(irg)
#define set_irg_entity(irg, ent)              _set_irg_entity(irg, ent)
#define get_irg_frame_type(irg)               _get_irg_frame_type(irg)
#define set_irg_frame_type(irg, ftp)          _set_irg_frame_type(irg, ftp)
#define get_irg_obstack(irg)                  _get_irg_obstack(irg)
#define get_irg_phase_state(irg)              _get_irg_phase_state(irg)
#define set_irg_phase_state(irg, state)       _set_irg_phase_state(irg, state)
#define get_irg_pinned(irg)                   _get_irg_pinned(irg)
#define get_irg_outs_state(irg)               _get_irg_outs_state(irg)
#define set_irg_outs_inconsistent(irg)        _set_irg_outs_inconsistent(irg)
#define get_irg_extblk_state(irg)             _get_irg_extblk_state(irg)
#define set_irg_extblk_inconsistent(irg)      _set_irg_extblk_inconsistent(irg)
#define get_irg_dom_state(irg)                _get_irg_dom_state(irg)
#define get_irg_postdom_state(irg)            _get_irg_postdom_state(irg)
#define set_irg_doms_inconsistent(irg)        _set_irg_doms_inconsistent(irg)
#define get_irg_loopinfo_state(irg)           _get_irg_loopinfo_state(irg)
#define set_irg_loopinfo_state(irg, s)        _set_irg_loopinfo_state(irg, s)
#define set_irg_loopinfo_inconsistent(irg)    _set_irg_loopinfo_inconsistent(irg)
#define set_irg_pinned(irg, p)                _set_irg_pinned(irg, p)
#define get_irg_callee_info_state(irg)        _get_irg_callee_info_state(irg)
#define set_irg_callee_info_state(irg, s)     _set_irg_callee_info_state(irg, s)
#define get_irg_inline_property(irg)          _get_irg_inline_property(irg)
#define set_irg_inline_property(irg, s)       _set_irg_inline_property(irg, s)
#define get_irg_additional_properties(irg)    _get_irg_additional_properties(irg)
#define set_irg_additional_properties(irg, m) _set_irg_additional_properties(irg, m)
#define set_irg_additional_property(irg, f)   _set_irg_additional_property(irg, f)
#define set_irg_link(irg, thing)              _set_irg_link(irg, thing)
#define get_irg_link(irg)                     _get_irg_link(irg)
#define get_irg_visited(irg)                  _get_irg_visited(irg)
#define get_irg_block_visited(irg)            _get_irg_block_visited(irg)
#define set_irg_block_visited(irg, v)         _set_irg_block_visited(irg, v)
#define inc_irg_block_visited(irg)            _inc_irg_block_visited(irg)
#define get_irg_estimated_node_cnt(irg)       _get_irg_estimated_node_cnt(irg)

# endif /* _IRGRAPH_T_H_ */
