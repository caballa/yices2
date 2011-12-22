/*
 * BIT-VECTOR SOLVER (BASELINE)
 */

#include <assert.h>

#include "memalloc.h"
#include "bv64_constants.h"
#include "bv64_intervals.h"
#include "small_bvsets.h"
#include "rb_bvsets.h"
#include "int_powers.h"

#include "bvsolver.h"

// PROVISIONAL

#include <stdio.h>
#include <inttypes.h>
#include "bvsolver_printer.h"



/*****************
 *  BOUND QUEUE  *
 ****************/

/*
 * Initialize the queue: initial size = 0
 */
static void init_bv_bound_queue(bv_bound_queue_t *queue) {
  queue->data = NULL;
  queue->top = 0;
  queue->size = 0;  
  queue->bound = NULL;
  queue->bsize = 0;
}


/*
 * Allocate the data array of make it 50% larger
 */
static void bv_bound_queue_extend(bv_bound_queue_t *queue) {
  uint32_t n;

  n = queue->size;
  if (n == 0) {
    n = DEF_BV_BOUND_QUEUE_SIZE;
    assert(n <= MAX_BV_BOUND_QUEUE_SIZE);
    queue->data = (bv_bound_t *) safe_malloc(n * sizeof(bv_bound_t));
    queue->size = n;

  } else {
    n += (n >> 1); // 50% larger
    assert(n > queue->size);
    if (n > MAX_BV_BOUND_QUEUE_SIZE) {
      out_of_memory();
    }
    queue->data = (bv_bound_t *) safe_realloc(queue->data, n * sizeof(bv_bound_t));
    queue->size = n;
  }
}


/*
 * Make the bound array large enough to store bound[x]
 * - x must be a variable index (between 0 and MAX_BV_BOUND_NUM_LISTS)
 * - this should be called when x >= queue->bsize
 */
static void bv_bound_queue_resize(bv_bound_queue_t *queue, thvar_t x) {
  uint32_t i, n;
  int32_t *tmp;

  assert(x >= queue->bsize);

  n = queue->bsize;
  if (n == 0) {
    n = DEF_BV_BOUND_NUM_LISTS;
  } else {
    n += (n >> 1);
    assert(n > queue->bsize);
  }

  if (n <= (uint32_t) x) {
    n = x + 1;
  }

  if (n > MAX_BV_BOUND_NUM_LISTS) {
    out_of_memory();
  }

  tmp = (int32_t *) safe_realloc(queue->bound, n * sizeof(int32_t));
  for (i=queue->size; i<n; i++) {
    tmp[i] = -1;
  }

  queue->bound = tmp;
  queue->bsize = n;
}


/*
 * Add a bound for variable x:
 * - id = the atom index
 */
static void bv_bound_queue_push(bv_bound_queue_t *queue, thvar_t x, int32_t id) {
  int32_t k, i;

  if (x >= queue->bsize) {
    bv_bound_queue_resize(queue, x);
    assert(x < queue->bsize);
  }

  k = queue->bound[x];
  assert(-1 <= k && k < (int32_t) queue->top);

  i = queue->top;
  if (i == queue->size) {
    bv_bound_queue_extend(queue);
  }
  assert(i < queue->size);

  queue->data[i].atom_id = id;
  queue->data[i].pre = k;
  queue->bound[x] = i;

  queue->top = i+1;
}



/*
 * Get the index of the first bound on x
 * - return -1 if there's no bound on x
 */
static inline int32_t bv_bound_for_var(bv_bound_queue_t *queue, thvar_t x) {
  int32_t k;

  k = -1;
  if (x < queue->bsize) {
    k = queue->bound[x];
    assert(-1 <= k && k < (int32_t) queue->top);
  }

  return k;
}



/*
 * Delete the queue
 */
static void delete_bv_bound_queue(bv_bound_queue_t *queue) {
  safe_free(queue->data);
  safe_free(queue->bound);
  queue->data = NULL;
  queue->bound = NULL;
}


/*
 * Empty the queue
 */
static void reset_bv_bound_queue(bv_bound_queue_t *queue) {
  uint32_t i, n;

  n = queue->bsize;
  for (i=0; i<n; i++) {
    queue->bound[i] = -1;
  }
  queue->top = 0;
}


/**********************************
 *  INTERVAL-COMPUTATION BUFFERS  *
 *********************************/

/*
 * Initialize the stack
 */
static void init_bv_interval_stack(bv_interval_stack_t *stack) {
  stack->data = NULL;
  stack->buffers = NULL;
  stack->size = 0;
  stack->top = 0;
}


/*
 * Delete: free all memory
 */
static void delete_bv_interval_stack(bv_interval_stack_t *stack) {
  uint32_t i, n;

  n = stack->size;
  if (n != 0) {
    assert(stack->data != NULL && stack->buffers != NULL);
    delete_bv_aux_buffers(stack->buffers);
    for (i=0; i<n; i++) {
      delete_bv_interval(stack->data + i);
    }
    safe_free(stack->buffers);
    safe_free(stack->data);

    stack->buffers = NULL;
    stack->data = NULL;
    stack->top = 0;
    stack->size = 0;
  }
}



/*
 * Reset: same as delete
 */
static inline void reset_bv_interval_stack(bv_interval_stack_t *stack) {
  delete_bv_interval_stack(stack);
}


/*
 * Allocate the stack (if not allocated already)
 * - use the default size
 */
static void alloc_bv_interval_stack(bv_interval_stack_t *stack) {
  uint32_t i, n;
  bv_interval_t *d;
  bv_aux_buffers_t *b;

  n = stack->size;
  if (n == 0) {
    n = DEF_BV_INTV_STACK_SIZE;
    assert(n <= MAX_BV_INTV_STACK_SIZE);
    d = (bv_interval_t *) safe_malloc(n * sizeof(bv_interval_t));
    for (i=0; i<n; i++) {
      init_bv_interval(d + i);
    }

    b = (bv_aux_buffers_t *) safe_malloc(sizeof(bv_aux_buffers_t));
    init_bv_aux_buffers(b);

    stack->data = d;
    stack->buffers = b;
    stack->size = n;
  }
}



/*
 * Get an interval object
 * - this uses a FIFO allocation model
 * - return NULL if the stack is full (fails)
 */
static bv_interval_t *get_bv_interval(bv_interval_stack_t *stack) {
  bv_interval_t *d;
  uint32_t i;

  d = NULL;
  i = stack->top;
  if (i < stack->size) {
    d = stack->data + i;
    stack->top = i+1;
  }
  return d;
}


/*
 * Free the last allocated interval object (i.e., decrement top)
 */
static inline void release_bv_interval(bv_interval_stack_t *stack) {
  assert(stack->top > 0);
  stack->top --;
}


/*
 * Free all the allocated intervals
 */
static inline void release_all_bv_intervals(bv_interval_stack_t *stack) {
  stack->top = 0;
}

/*
 * Get the auxiliary buffer structure
 * - alloc_bv_interval_stack must be called before this function
 */
static inline bv_aux_buffers_t *get_bv_aux_buffers(bv_interval_stack_t *stack) {
  assert(stack->size != 0 && stack->buffers != NULL);
  return stack->buffers;
}







/********************
 *  PUSH/POP STACK  *
 *******************/

/*
 * Initialize the stack: initial size = 0
 */
static void init_bv_trail(bv_trail_stack_t *stack) {
  stack->size = 0;
  stack->top = 0;
  stack->data = NULL;
}



/*
 * Save a base level
 * - nv = number of variables
 * - na = number of atoms
 * - nb = number of bounds
 */
static void bv_trail_save(bv_trail_stack_t *stack, uint32_t nv, uint32_t na, uint32_t nb) {
  uint32_t i, n;

  i = stack->top;
  n = stack->size;
  if (i == n) {
    if (n == 0) {
      n = DEF_BV_TRAIL_SIZE;
      assert(0<n &&  n<MAX_BV_TRAIL_SIZE);
    } else {
      n += n;
      if (n >= MAX_BV_TRAIL_SIZE) {
	out_of_memory();
      }
    }
    stack->data = (bv_trail_t *) safe_realloc(stack->data, n * sizeof(bv_trail_t));
    stack->size = n;
  }
  assert(i < stack->size);

  stack->data[i].nvars = nv;
  stack->data[i].natoms = na;
  stack->data[i].nbounds = nb;

  stack->top = i+1;
}


/*
 * Get the top trail record
 */
static bv_trail_t *bv_trail_top(bv_trail_stack_t *stack) {
  assert(stack->top > 0);
  return stack->data + (stack->top - 1);
}


/*
 * Remove the top record
 */
static inline void bv_trail_pop(bv_trail_stack_t *stack) {
  assert(stack->top > 0);
  stack->top --;
}


/*
 * Delete the stack
 */
static inline void delete_bv_trail(bv_trail_stack_t *stack) {
  safe_free(stack->data);
  stack->data = NULL;
}


/*
 * Empty the stack
 */
static inline void reset_bv_trail(bv_trail_stack_t *stack) {
  stack->top = 0;
}





/*****************
 *  USED VALUES  *
 ****************/

/*
 * Initialize a used_vals array:
 * - initial size = 0
 */
static void init_used_vals(used_bvval_t *used_vals) {
  used_vals->data = NULL;
  used_vals->nsets = 0;
  used_vals->size = 0;
}


/*
 * Empty the array: delete all sets
 */
static void reset_used_vals(used_bvval_t *used_vals) {
  uint32_t i, n;

  n = used_vals->nsets;
  for (i=0; i<n; i++) {
    if (used_vals->data[i].bitsize <= SMALL_BVSET_LIMIT) {
      delete_small_bvset(used_vals->data[i].ptr);
    } else {
      delete_rb_bvset(used_vals->data[i].ptr);
    }
    safe_free(used_vals->data[i].ptr);
    used_vals->data[i].ptr = NULL;
  }

  used_vals->nsets = 0;
}


/*
 * Delete a used_vals array: free memory
 */
static void delete_used_vals(used_bvval_t *used_vals) {
  reset_used_vals(used_vals);
  safe_free(used_vals->data);
  used_vals->data = NULL;
}



#if 0

// NOT USED YET

/*
 * Allocate a set descriptor
 * - return its id
 */
static uint32_t used_vals_new_set(used_bvval_t *used_vals) {
  uint32_t i, n;

  i = used_vals->nsets;
  n = used_vals->size;
  if (i == n) {
    if (n == 0) {
      // first allocation: use the default size
      n = USED_BVVAL_DEF_SIZE;
      assert(n < USED_BVVAL_MAX_SIZE);
      used_vals->data = (bvset_t *) safe_malloc(n * sizeof(bvset_t));
      used_vals->size = n;
    } else {
      // make the array 50% larger
      n ++;
      n += n>>1;
      if (n >= USED_BVVAL_MAX_SIZE) {
	out_of_memory();
      }
      used_vals->data = (bvset_t *) safe_realloc(used_vals->data, n * sizeof(bvset_t));
      used_vals->size = n;
    }
  }

  assert(i < used_vals->size);
  used_vals->nsets = i+1;

  return i;
}


/*
 * Return the set descriptor for bit-vectors of size k
 * - allocate and initialize a new desciptor if necessary
 * - for a new descriptor, the internal small_set or red-black tree is NULL
 */ 
static bvset_t *used_vals_get_set(used_bvval_t *used_vals, uint32_t k) {
  uint32_t i, n;

  assert(k > 0);
  n = used_vals->nsets;
  for (i=0; i<n; i++) {
    if (used_vals->data[i].bitsize == k) {
      return used_vals->data + i;
    }
  }

  // allocate a new descriptor
  i = used_vals_new_set(used_vals);
  used_vals->data[i].bitsize = k;
  used_vals->data[i].ptr = NULL;

  return used_vals->data + i;
}


/*
 * Allocate a new small_bvset for size k and initialize it
 */
static small_bvset_t *new_small_bvset(uint32_t k) {
  small_bvset_t *tmp;

  assert(0 < k && k <= SMALL_BVSET_LIMIT);
  tmp = (small_bvset_t *) safe_malloc(sizeof(small_bvset_t));
  init_small_bvset(tmp, k);

  return tmp;
}


/*
 * Allocate a red-black tree for bitvectors of size k
 * and initialize it.
 */
static rb_bvset_t *new_rb_bvset(uint32_t k) {
  rb_bvset_t *tmp;

  assert(k > SMALL_BVSET_LIMIT);
  tmp = (rb_bvset_t *) safe_malloc(sizeof(rb_bvset_t));
  init_rb_bvset(tmp, k);

  return tmp;
}

#endif



/********************************
 *  MAPPING TO PSEUDO LITERALS  *
 *******************************/

/*
 * Return the remap table (allocate and initialize it if necessary)
 */
static remap_table_t *bv_solver_get_remap(bv_solver_t *solver) {
  remap_table_t *tmp;

  tmp = solver->remap;
  if (tmp == NULL) {
    tmp = (remap_table_t *) safe_malloc(sizeof(remap_table_t));
    init_remap_table(tmp);
    solver->remap = tmp;
  }

  return tmp;
}


/*
 * Return the pseudo literal array mapped to x
 * - allocate a new array of n literals if x is not mapped yet
 */
static literal_t *bv_solver_get_pseudo_map(bv_solver_t *solver, thvar_t x) {
  remap_table_t *rmap;
  literal_t *tmp;
  uint32_t n;

  tmp = bvvar_get_map(&solver->vtbl, x);
  if (tmp == NULL) {
    n = bvvar_bitsize(&solver->vtbl, x);
    rmap = bv_solver_get_remap(solver);
    tmp = remap_table_fresh_array(rmap, n);
    bvvar_set_map(&solver->vtbl, x, tmp);
  }

  return tmp;
}



/******************
 *  BOUND ATOMS   *
 *****************/

/*
 * Check whether x is a constant
 */
static inline bool is_constant(bv_vartable_t *table, thvar_t x) {
  bvvar_tag_t tag;

  tag = bvvar_tag(table, x);
  return (tag == BVTAG_CONST64) | (tag == BVTAG_CONST);
}


/*
 * Check wether x or y is a constant
 */
static inline bool is_bv_bound_pair(bv_vartable_t *table, thvar_t x, thvar_t y) {
  bvvar_tag_t tag_x, tag_y;

  tag_x = bvvar_tag(table, x);
  tag_y = bvvar_tag(table, y);

  return (tag_x == BVTAG_CONST64) | (tag_x == BVTAG_CONST)
    | (tag_y == BVTAG_CONST64) | (tag_y == BVTAG_CONST);
}



/*
 * Check whether atom i is a 'bound atom' (i.e., inequality between
 * a constant and a non-constant).
 * - all atoms are of the form (op x y), we just check whether x
 *   or y is a bitvector constant.
 * - this is fine since no atom should involve two constants
 *   (constraints between constants are always simplified to true or false)
 */
static inline bool is_bound_atom(bv_solver_t *solver, int32_t i) {
  bvatm_t *a;
  
  a = bvatom_desc(&solver->atbl, i);
  return is_constant(&solver->vtbl, a->left) || is_constant(&solver->vtbl, a->right);
}


/*
 * Get the variable in a bound atom
 */
static thvar_t bound_atom_var(bv_solver_t *solver, int32_t i) {
  bvatm_t *a;
  thvar_t x;
  
  a = bvatom_desc(&solver->atbl, i);
  x = a->left;
  if (is_constant(&solver->vtbl, x)) {
    x = a->right;
  }

  assert(! is_constant(&solver->vtbl, x));

  return x;
}


/*
 * Add (bvge x y) to the bound queue
 * - either x or y must be a constant
 * - the atom (bvge x y) must exist in the atom table
 *   and (bvge x y) must not be a trivial atom
 *   (i.e., not of the form (bvge x 0b00000) or (bvge 0b11111 x))
 */
static void push_bvuge_bound(bv_solver_t *solver, thvar_t x, thvar_t y) {
  int32_t i;

  assert(is_bv_bound_pair(&solver->vtbl, x, y));

  i = find_bvuge_atom(&solver->atbl, x, y);
  assert(i >= 0 && is_bound_atom(solver, i));
  if (is_constant(&solver->vtbl, x)) {
    x = y;
  }

  assert(! is_constant(&solver->vtbl, x));
  bv_bound_queue_push(&solver->bqueue, x, i);
}


/*
 * Same thing for (bvsge x y)
 * - the atom must not be of the form (bvsge x 0b100000)
 *   or (bvsge 0b011111 x)
 */
static void push_bvsge_bound(bv_solver_t *solver, thvar_t x, thvar_t y) {
  int32_t i;

  assert(is_bv_bound_pair(&solver->vtbl, x, y));

  i = find_bvsge_atom(&solver->atbl, x, y);
  assert(i >= 0 && is_bound_atom(solver, i));
  if (is_constant(&solver->vtbl, x)) {
    x = y;
  }

  assert(! is_constant(&solver->vtbl, x));
  bv_bound_queue_push(&solver->bqueue, x, i);
}


/*
 * Same thing for (eq x y) 
 */
static void push_bvdiseq_bound(bv_solver_t *solver, thvar_t x, thvar_t y) {
  int32_t i;

  assert(is_bv_bound_pair(&solver->vtbl, x, y));

  i = find_bveq_atom(&solver->atbl, x, y);
  assert(i >= 0 && is_bound_atom(solver, i));
  if (is_constant(&solver->vtbl, x)) {
    x = y;
  }

  assert(! is_constant(&solver->vtbl, x));
  bv_bound_queue_push(&solver->bqueue, x, i);
}



/*
 * Remove all bounds of index >= n
 */
static void bv_solver_remove_bounds(bv_solver_t *solver, uint32_t n) {
  bv_bound_queue_t *queue;
  bv_bound_t *d;
  uint32_t i;
  thvar_t x;

  queue = &solver->bqueue;
  assert(0 <= n && n <= queue->top);

  i = queue->top;
  d = queue->data + i;
  while (i > n) {
    i --;
    d --;
    x = bound_atom_var(solver, d->atom_id);
    assert(0 <= x && x < queue->bsize);
    queue->bound[x] = d->pre;
  }

  queue->top = n;
}



/*
 * SEARCH FOR BOUNDS
 */

/*
 * Check whether a bound atom on x is a lower or upper bound
 * - x must be the variable in bound_atom i
 */
static inline bool lit_is_true(smt_core_t *core, literal_t l) {
  return literal_base_value(core, l) == VAL_TRUE;
}

static inline bool lit_is_false(smt_core_t *core, literal_t l) {
  return literal_base_value(core, l) == VAL_FALSE;
}

// upper bound unsigned
static bool bound_is_ub(bv_solver_t *solver, thvar_t x, int32_t i) {
  bvatm_t *a;

  assert(is_bound_atom(solver, i) && bound_atom_var(solver, i) == x);
  a = bvatom_desc(&solver->atbl, i);
  if (bvatm_is_ge(a)) {
    // either (bvge x c) or (bvge c x) 
    return (x == a->left && lit_is_false(solver->core, a->lit)) // (bvge x c) is false so x <= c-1
      || (x == a->right && lit_is_true(solver->core, a->lit));  // (bvge c x) is true so  x <= c
  }

  return false;
}

// lower bound unsigned
static bool bound_is_lb(bv_solver_t *solver, thvar_t x, int32_t i) {
  bvatm_t *a;

  assert(is_bound_atom(solver, i) && bound_atom_var(solver, i) == x);
  a = bvatom_desc(&solver->atbl, i);
  if (bvatm_is_ge(a)) {
    // either (bvge x c) or (bvge c x) 
    return (x == a->left && lit_is_true(solver->core, a->lit))  // (bvge x c) is true so x >= c
      || (x == a->right && lit_is_false(solver->core, a->lit)); // (bvge c x) is false so x >= c+1
  }

  return false;
}

// upper bound signed
static bool bound_is_signed_ub(bv_solver_t *solver, thvar_t x, int32_t i) {
  bvatm_t *a;

  assert(is_bound_atom(solver, i) && bound_atom_var(solver, i) == x);
  a = bvatom_desc(&solver->atbl, i);
  if (bvatm_is_sge(a)) {
    // either (bvsge x c) or (bvsge c x) 
    return (x == a->left && lit_is_false(solver->core, a->lit)) // (bvge x c) is false so x <= c-1
      || (x == a->right && lit_is_true(solver->core, a->lit));  // (bvge c x) is true so  x <= c
  }

  return false;
}

// lower bound signed
static bool bound_is_signed_lb(bv_solver_t *solver, thvar_t x, int32_t i) {
  bvatm_t *a;

  assert(is_bound_atom(solver, i) && bound_atom_var(solver, i) == x);
  a = bvatom_desc(&solver->atbl, i);
  if (bvatm_is_sge(a)) {
    // either (bvsge x c) or (bvsge c x) 
    return (x == a->left && lit_is_true(solver->core, a->lit))  // (bvsge x c) is true so x >= c
      || (x == a->right && lit_is_false(solver->core, a->lit)); // (bvsge c x) is false so x >= c+1
  }

  return false;
}



/*
 * Extract the unsigned or signed upper bound on x from atom i
 * - x must be a bitvector of 64bits or less
 * - for (bvge x c) false, we know (c > 0b00..0)
 * - for (bvsge x c) false, we know (c > 0b1000)
 * In either case, (c-1) can't cause underflow
 * The result is normalized modulo (2 ^ n) where n = size of x
 */
static uint64_t get_upper_bound64(bv_solver_t *solver, thvar_t x, int32_t i) {
  bvatm_t *a;
  uint64_t c;

  a = bvatom_desc(&solver->atbl, i);
  if (x == a->left) {
    // (bvge x c) false: x <= c-1
    c = bvvar_val64(&solver->vtbl, a->right) - 1;    
  } else {
    // (bvge c x) true: x <= c
    assert(x == a->right);
    c = bvvar_val64(&solver->vtbl, a->left);
  }

  assert(c == norm64(c, bvvar_bitsize(&solver->vtbl, x)));

  return c;
}


/*
 * Extract the unsigned or signed lower bound
 * - x must be a bitvector of 64bits or less
 * - for (bvge c x) false, we know (c < 0b11111)
 * - for (bvsge c x) false, we know (c < 0b01111)
 * In either case, (c+1) can't cause overflow
 * The result is normalized modulo (2 ^ n)
 */
static uint64_t get_lower_bound64(bv_solver_t *solver, thvar_t x, int32_t i) {
  bvatm_t *a;
  uint64_t c;

  a = bvatom_desc(&solver->atbl, i);
  if (x == a->left) {
    //(bvge x c) true: x >= c
    c = bvvar_val64(&solver->vtbl, a->right); 
  } else {
    // (bvge c x) false: x >= c+1
    assert(x == a->right);
    c = bvvar_val64(&solver->vtbl, a->left) + 1;
  }

  assert(c == norm64(c, bvvar_bitsize(&solver->vtbl, x)));

  return c;
}



/*
 * Extract the unsigned or signed upper bound on x from atom i
 * - x must be a bitvector of more than 64bits
 * - n = size of x
 * - for (bvge x c) false, we know (c > 0b00..0)
 * - for (bvsge x c) false, we know (c > 0b1000)
 * In either case, (c-1) can't cause underflow
 * The result is returned in c and it's normalized modulo (2 ^ n)
 */
static void get_upper_bound(bv_solver_t *solver, thvar_t x, uint32_t n, int32_t i, bvconstant_t *c) {
  bvatm_t *a;

  a = bvatom_desc(&solver->atbl, i);
  if (x == a->left) {
    // (bvge x c) false: x <= c-1
    bvconstant_copy(c, n, bvvar_val(&solver->vtbl, a->right));
    bvconstant_sub_one(c);
    bvconstant_normalize(c);
  } else {
    // (bvge c x) true: x <= c
    assert(x == a->right);
    bvconstant_copy(c, n, bvvar_val(&solver->vtbl, a->left));
  }
}


/*
 * Extract the unsigned or signed lower bound on x from atom i
 * - x must be a bitvector of more than 64bits
 * - n = size of x
 * - for (bvge x c) false, we know (c > 0b00..0)
 * - for (bvsge x c) false, we know (c > 0b1000)
 * In either case, (c-1) can't cause underflow
 * The result is returned in c and it's normalized modulo (2 ^ n)
 */
static void get_lower_bound(bv_solver_t *solver, thvar_t x, uint32_t n, int32_t i, bvconstant_t *c) {
  bvatm_t *a;

  a = bvatom_desc(&solver->atbl, i);
  if (x == a->left) {
    // (bvge x c) true: x >= c
    bvconstant_copy(c, n, bvvar_val(&solver->vtbl, a->right));
  } else {
    // (bvge c x) false: x >= c+1
    assert(x == a->right);
    bvconstant_copy(c, n, bvvar_val(&solver->vtbl, a->left));
    bvconstant_add_one(c);
    bvconstant_normalize(c);
  }
}



/*
 * Check whether there's a bound asserted on variable x
 * - if so, return true and store the bound in *c
 * - otherwise, return false
 */

// upper bound, 64bits, unsigned
static bool bvvar_has_upper_bound64(bv_solver_t *solver, thvar_t x, uint64_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_ub(solver, x, b->atom_id)) {
      *c = get_upper_bound64(solver, x, b->atom_id);
      return true;
    }
    k = b->pre;
  }

  return false;
}

// lower bound, 64bits, unsigned
static bool bvvar_has_lower_bound64(bv_solver_t *solver, thvar_t x, uint64_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_lb(solver, x, b->atom_id)) {
      *c = get_lower_bound64(solver, x, b->atom_id);
      return true;
    }
    k = b->pre;
  }

  return false;
}

// upper bound, 64bits, signed
static bool bvvar_has_signed_upper_bound64(bv_solver_t *solver, thvar_t x, uint64_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_signed_ub(solver, x, b->atom_id)) {
      *c = get_upper_bound64(solver, x, b->atom_id);
      return true;
    }
    k = b->pre;
  }

  return false;
}


// lower bound, 64bits, signed
static bool bvvar_has_signed_lower_bound64(bv_solver_t *solver, thvar_t x, uint64_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_signed_lb(solver, x, b->atom_id)) {
      *c = get_lower_bound64(solver, x, b->atom_id);
      return true;
    }
    k = b->pre;
  }

  return false;
}


/*
 * Same thing for bitvectors with more than 64bits
 * - n = number of bits in x
 */

// upper bound, unsigned
static bool bvvar_has_upper_bound(bv_solver_t *solver, thvar_t x, uint32_t n, bvconstant_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_ub(solver, x, b->atom_id)) {
      get_upper_bound(solver, x, n, b->atom_id, c);
      return true;
    }
    k = b->pre;
  }

  return false;
}

// lower bound, unsigned
static bool bvvar_has_lower_bound(bv_solver_t *solver, thvar_t x, uint32_t n, bvconstant_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_lb(solver, x, b->atom_id)) {
      get_lower_bound(solver, x, n, b->atom_id, c);
      return true;
    }
    k = b->pre;
  }

  return false;
}

// upper bound, signed
static bool bvvar_has_signed_upper_bound(bv_solver_t *solver, thvar_t x, uint32_t n, bvconstant_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_signed_ub(solver, x, b->atom_id)) {
      get_upper_bound(solver, x, n, b->atom_id, c);
      return true;
    }
    k = b->pre;
  }

  return false;
}

// lower bound, signed
static bool bvvar_has_signed_lower_bound(bv_solver_t *solver, thvar_t x, uint32_t n, bvconstant_t *c) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    if (bound_is_signed_lb(solver, x, b->atom_id)) {
      get_lower_bound(solver, x, n, b->atom_id, c);
      return true;
    }
    k = b->pre;
  }

  return false;
}


/*
 * Check whether there's a constraint not (x == 0) on x
 * - this just search for a bound atom of the form (eq x c) or (eq c x)
 *   in the queue
 */
static bool bvvar_is_nonzero(bv_solver_t *solver, thvar_t x) {
  bv_bound_queue_t *queue;
  bv_bound_t *b;
  bvatm_t *a;
  int32_t k;

  queue = &solver->bqueue;
  k = bv_bound_for_var(queue, x);
  while (k >= 0) {
    assert(k < queue->top);
    b = queue->data + k;
    a = bvatom_desc(&solver->atbl, b->atom_id);
    assert(a->left == x || a->right == x);
    if (bvatm_is_eq(a)) {
      return true;
    }
    k = b->pre;
  }

  return false;
}




/**********************
 *  VARIABLE MERGING  *
 *********************/

/*
 * We attempt to keep the simplest element of the class as
 * root of its class, using the following ranking:
 * - constants are simplest:       rank 0
 * - bvarray are next              rank 1
 * - polynomials                   rank 2
 * - power products                rank 3
 * - other non-variable terms:     rank 4
 * - variables are last            rank 5
 *
 * The following functions checks whether a is striclty simpler than b
 * based on this ranking.
 */
static const uint8_t bvtag2rank[NUM_BVTAGS] = {
  5,      // BVTAG_VAR
  0,      // BVTAG_CONST64
  0,      // BVTAG_CONST
  1,      // BVTAG_POLY64
  1,      // BVTAG_POLY
  3,      // BVTAG_PPROD
  2,      // BVTAG_BIT_ARRAY
  4,      // BVTAG_ITE
  4,      // BVTAG_UDIV
  4,      // BVTAG_UREM
  4,      // BVTAG_SDIV
  4,      // BVTAG_SREM
  4,      // BVTAG_SMOD
  4,      // BVTAG_SHL
  4,      // BVTAG_LSHR
  4,      // BVTAG_ASHR
};

static inline bool simpler_bvtag(bvvar_tag_t a, bvvar_tag_t b) {
  return bvtag2rank[a] < bvtag2rank[b];
}


/*
 * Check whether tag is for a constant
 */
static inline bool constant_bvtag(bvvar_tag_t a) {
  return a == BVTAG_CONST64 || a == BVTAG_CONST;
}



/*
 * Merge the equivalence classes of x and y
 * - both x and y must be root of their class
 * - x and y must be distinct variables
 */
static void bv_solver_merge_vars(bv_solver_t *solver, thvar_t x, thvar_t y) {
  mtbl_t *mtbl;
  bvvar_tag_t tag_x, tag_y;
  thvar_t aux;

  mtbl = &solver->mtbl;

  assert(x != y && mtbl_is_root(mtbl, x) && mtbl_is_root(mtbl, y));

  tag_x = bvvar_tag(&solver->vtbl, x);
  tag_y = bvvar_tag(&solver->vtbl, y);

  if (simpler_bvtag(tag_y, tag_x)) {
    aux = x; x = y; y = aux;
  }

  // x is simpler than y, we set map[y] := x
  mtbl_map(mtbl, y, x);
}




/********************************
 *  SUPPORT FOR SIMPLIFICATION  *
 *******************************/


/*
 * Check whether the root of x's class is a 64bit constant
 * - if so return the root, otherwise return x
 */
static thvar_t bvvar_root_if_const64(bv_solver_t *solver, thvar_t x) {
  thvar_t y;

  y = mtbl_get_root(&solver->mtbl, x);
  if (bvvar_is_const64(&solver->vtbl, y)) {
    x = y;
  }

  return x;
}


/*
 * Check whether the root of x's class is a generic constant
 * - if so return the root, otherwise return x
 */
static thvar_t bvvar_root_if_const(bv_solver_t *solver, thvar_t x) {
  thvar_t y;

  y = mtbl_get_root(&solver->mtbl, x);
  if (bvvar_is_const(&solver->vtbl, y)) {
    x = y;
  }

  return x;
}




/*
 * Check whether x is equal to 0b0000...
 */
static bool bvvar_is_zero(bv_vartable_t *vtbl, thvar_t x) {  
  uint32_t n, k;

  switch (bvvar_tag(vtbl, x)) {
  case BVTAG_CONST64:
    return bvvar_val64(vtbl, x) == 0;

  case BVTAG_CONST:
    n = bvvar_bitsize(vtbl, x);
    k = (n + 31) >> 5;
    return bvconst_is_zero(bvvar_val(vtbl, x), k);

  default:
    return false;
  }
}


/*
 * Check whether x is equal to 0b1111...
 */
static bool bvvar_is_minus_one(bv_vartable_t *vtbl, thvar_t x) {  
  uint32_t n;

  switch (bvvar_tag(vtbl, x)) {
  case BVTAG_CONST64:
    n = bvvar_bitsize(vtbl, x);
    return bvconst64_is_minus_one(bvvar_val64(vtbl, x), n);

  case BVTAG_CONST:
    n = bvvar_bitsize(vtbl, x);
    return bvconst_is_minus_one(bvvar_val(vtbl, x), n);

  default:
    return false;
  }
}


/*
 * Check whether x is equal to 0b1000...
 */
static bool bvvar_is_min_signed(bv_vartable_t *vtbl, thvar_t x) {
  uint32_t n;

  switch (bvvar_tag(vtbl, x)) {
  case BVTAG_CONST64:
    n = bvvar_bitsize(vtbl, x);
    return bvvar_val64(vtbl, x) == min_signed64(n);

  case BVTAG_CONST:
    n = bvvar_bitsize(vtbl, x);
    return bvconst_is_min_signed(bvvar_val(vtbl, x), n);

  default:
    return false;
  }
}


/*
 * Check whether x is equal to 0b0111...
 */
static bool bvvar_is_max_signed(bv_vartable_t *vtbl, thvar_t x) {
  uint32_t n;

  switch (bvvar_tag(vtbl, x)) {
  case BVTAG_CONST64:
    n = bvvar_bitsize(vtbl, x);
    return bvvar_val64(vtbl, x) == max_signed64(n);

  case BVTAG_CONST:
    n = bvvar_bitsize(vtbl, x);
    return bvconst_is_max_signed(bvvar_val(vtbl, x), n);

  default:
    return false;
  }
}


/*
 * Build the zero constant of n bits
 */
static thvar_t get_zero(bv_solver_t *solver, uint32_t nbits) {
  thvar_t z;

  if (nbits > 64) {
    bvconstant_set_all_zero(&solver->aux1, nbits);
    z = get_bvconst(&solver->vtbl, nbits, solver->aux1.data);
  } else {
    z = get_bvconst64(&solver->vtbl, nbits, 0);
  }

  return z;
}


/*
 * Check whether p can be written as c0 + a0 t or c0
 * - return true if p has one of these special forms
 * - if p is a constant polynomial, then t is set to null_term
 */
static bool bvpoly64_is_simple(bv_solver_t *solver, bvpoly64_t *p, uint64_t *c0, uint64_t *a0, thvar_t *t) {
  bv_vartable_t *vtbl;
  uint32_t i, n, nbits;
  uint64_t cnst;
  thvar_t x, u;

  vtbl = &solver->vtbl;

  *a0 = 0; // otherwise GCC gives a bogus warning
  
  n = p->nterms;
  nbits = p->bitsize;
  u = null_thvar;

  cnst = 0;
  i = 0;

  // deal with the constant term of p if any
  if (p->mono[0].var == const_idx) {
    cnst = p->mono[0].coeff;
    i ++;
  }

  // rest of p
  while (i < n) {
    assert(p->mono[i].var != const_idx);

    x = mtbl_get_root(&solver->mtbl, p->mono[i].var);
    if (bvvar_is_const64(vtbl, x)) {
      cnst += p->mono[i].coeff * bvvar_val64(vtbl, x);
    } else if (u == null_thvar) {
      u = x;
      *a0 = p->mono[i].coeff;
    } else if (u == x) {
      // two terms of p have the same root u
      *a0 = norm64(*a0 + p->mono[i].coeff, nbits);
    } else {
      // p has more than one non-constant term
      return false;
    }
    i ++;    
  }

  // store the result
  *c0 = norm64(cnst, nbits);
  *t = u;

  return true;
}



/*
 * Same thing for p with large coefficients
 */
static bool bvpoly_is_simple(bv_solver_t *solver, bvpoly_t *p, bvconstant_t *c0, bvconstant_t *a0, thvar_t *t) {
  bv_vartable_t *vtbl;
  uint32_t i, n, nbits, w;
  thvar_t x, u;

  vtbl = &solver->vtbl;

  n = p->nterms;
  nbits = p->bitsize;
  w = p->width;
  u = null_thvar;

  i = 0;

  // c0 := 0 (also make sure c0 has the right size
  bvconstant_set_all_zero(c0, nbits);

  // deal with the constant of p if any
  if (p->mono[0].var == const_idx) {
    bvconst_set(c0->data, w, p->mono[0].coeff);
    i ++;
  }

  // check the rest of p
  while (i < n) {
    assert(p->mono[i].var != const_idx);

    x = mtbl_get_root(&solver->mtbl, p->mono[i].var);
    if (bvvar_is_const(vtbl, x)) {
      bvconst_addmul(c0->data, w, p->mono[i].coeff, bvvar_val(vtbl, x));
    } else if (u == null_thvar) {
      u = x;
      bvconstant_copy(a0, nbits, p->mono[i].coeff);
    } else if (u == x) {
      assert(a0->bitsize == nbits);
      bvconst_add(a0->data, a0->width, p->mono[i].coeff);
      bvconstant_normalize(a0);
    } else {
      // p has more than one non-constant term
      return false;
    }

    i++;
  }

  bvconstant_normalize(c0);
  *t = u;

  return true;
}



/*********************
 *  EQUALITY CHECKS  *
 ********************/

/*
 * Check whether coefficient a is one or minus one, n = number of bits
 * - a must be normalized
 */
static bool bvconst_is_pm_one(uint32_t *a, uint32_t n) {
  uint32_t w;

  w = (n + 31) >> 5;
  return bvconst_is_one(a, w) || bvconst_is_minus_one(a, n);
}


/*
 * Attempt to simplify an equality between two polynomials.
 * This uses the following rules:
 *  p + t == p + t' iff  t == t'
 *  p - t == p - t' iff  t == t'
 *  p + t == p      iff  t == 0
 *  p - t == p      iff  t == 0
 *
 * If one of these rules is applicable, the function stores t and t' in *vx and *vy. 
 * Otherwise, it leaves *vx and *vy unchanged.
 */
static void simplify_bvpoly64_eq(bv_solver_t *solver, bvpoly64_t *p, bvpoly64_t *q, thvar_t *vx, thvar_t *vy) {
  uint32_t i, n, ki;
  uint32_t j, m, kj;
  thvar_t x, y;

  assert(p->bitsize == q->bitsize);

  n = p->nterms;
  m = q->nterms;

  if (n <= m+1 && m <= n+1) {
    i = 0;
    j = 0;
    ki = n;
    kj = m;

    while (i<n && j<m) {
      x = p->mono[i].var;
      y = q->mono[j].var;
      if (x < y) {
	// x does not occur in q
	if (ki < n) return;
	ki = i;
	i ++;
      } else if (y < x) {
	// y does not occur in p
	if (kj < m) return;
	kj = j;
	j ++;
      } else {
	// same variable in p and q
	if (p->mono[i].coeff != q->mono[j].coeff) return;
	i++;
	j++;
      }
    }

    /*
     * ki = index of a variable of p not in q (or ki = n)
     * kj = index of a variable of q not in p (or kj = m)
     * all other monomials are the same in p and q
     */
    if (ki < n && kj < m) {
      assert(n == m);
      if (p->mono[ki].coeff != q->mono[kj].coeff) return;
      if (p->mono[ki].coeff != 1 && p->mono[ki].coeff != 1) return;
      // either p = r + x and q = r + y, or p = r - x and q = r - y
      // so p == q iff x == y
      x = p->mono[ki].var;
      y = q->mono[kj].var;
    } else if (kj < m) {
      assert(m == n+1 && ki == n);
      if (q->mono[kj].coeff != 1 && q->mono[kj].coeff != -1) return;	
      // q is p + x or p - x: (p == q) iff x == 0
      x = q->mono[kj].var;
      y = get_zero(solver, q->bitsize);
    } else if (ki < n) {
      assert(n == m+1 && kj == m);
      if (p->mono[ki].coeff != 1 && p->mono[ki].coeff != -1) return;
      // p is q + x or q - x: (p == q) iff x == 0
      x = p->mono[ki].var;
      y = get_zero(solver, p->bitsize); 
    }

    *vx = x;
    *vy = y;
  }
}

// same thing: large coefficients
static void simplify_bvpoly_eq(bv_solver_t *solver, bvpoly_t *p, bvpoly_t *q, thvar_t *vx, thvar_t *vy) {
  uint32_t i, n, ki;
  uint32_t j, m, kj;
  uint32_t w, nbits;
  thvar_t x, y;

  assert(p->bitsize == q->bitsize);

  n = p->nterms;
  nbits = p->bitsize;
  w = p->width;

  m = q->nterms;

  if (n <= m+1 && m <= n+1) {
    i = 0;
    j = 0;
    ki = n;
    kj = m;

    while (i<n && j<m) {
      x = p->mono[i].var;
      y = q->mono[j].var;
      if (x < y) {
	// x does not occur in q
	if (ki < n) return;
	ki = i;
	i ++;
      } else if (y < x) {
	// y does not occur in p
	if (kj < m) return;
	kj = j;
	j ++;
      } else {
	// same variable in p and q
	if (bvconst_neq(p->mono[i].coeff, q->mono[j].coeff, w)) return;
	i++;
	j++;
      }
    }

    /*
     * ki = index of a variable of p not in q (or ki = n)
     * kj = index of a variable of q not in p (or kj = m)
     * all other monomials are the same in p and q
     */
    if (ki < n && kj < m) {
      assert(n == m);
      if (bvconst_neq(p->mono[ki].coeff, q->mono[kj].coeff, w)) return;
      if (!bvconst_is_pm_one(p->mono[ki].coeff, nbits)) return;
      // either p = r + x and q = r + y, or p = r - x and q = r - y
      // so p == q iff x == y
      x = p->mono[ki].var;
      y = q->mono[kj].var;
    } else if (kj < m) {
      assert(m == n+1 && ki == n);
      if (!bvconst_is_pm_one(q->mono[kj].coeff, nbits)) return;
      // q is p + x or p - x: (p == q) iff x == 0
      x = q->mono[kj].var;
      y = get_zero(solver, nbits);
    } else if (ki < n) {
      assert(n == m+1 && kj == m);
      if (!bvconst_is_pm_one(p->mono[ki].coeff, nbits)) return;
      // p is q + x or q - x: (p == q) iff x == 0
      x = p->mono[ki].var;
      y = get_zero(solver, nbits); 
    }

    *vx = mtbl_get_root(&solver->mtbl, x);
    *vy = mtbl_get_root(&solver->mtbl, y);
  }
}


/*
 * Attempt to simplify (eq x y) to an equivalent equality (eq x' y')
 * - on entry, x and y must be stored in *vx and *vy
 * - x and y must be root in solver->mtbl
 * - on exit, the simplified form is stored in *vx and *vy
 */
static void simplify_eq(bv_solver_t *solver, thvar_t *vx, thvar_t *vy) {
  bv_vartable_t *vtbl;
  thvar_t x, y;
  bvvar_tag_t tag_x, tag_y;

  x = *vx;
  y = *vy;

  assert(x != y && mtbl_is_root(&solver->mtbl, x) && mtbl_is_root(&solver->mtbl, y));

  vtbl = &solver->vtbl;

  tag_x = bvvar_tag(vtbl, x);
  tag_y = bvvar_tag(vtbl, y);


  if (tag_x == tag_y) {
    if (tag_x == BVTAG_POLY64) {
      simplify_bvpoly64_eq(solver, bvvar_poly64_def(vtbl, x), bvvar_poly64_def(vtbl, y), vx, vy);
    } else if (tag_x == BVTAG_POLY) {
      simplify_bvpoly_eq(solver, bvvar_poly_def(vtbl, x), bvvar_poly_def(vtbl, y), vx, vy);
    }
  }

  if (x != *vx || y != *vy) {
#if 1
    printf("---> bv simplify (bveq u_%"PRId32" u_%"PRId32")\n", x, y);
    printf("     ");
    print_bv_solver_vardef(stdout, solver, x);
    printf("     ");
    print_bv_solver_vardef(stdout, solver, y);
    printf("     simplified to (bveq u_%"PRId32" u_%"PRId32")\n\n", *vx, *vy);
#endif
    *vx = mtbl_get_root(&solver->mtbl, *vx);
    *vy = mtbl_get_root(&solver->mtbl, *vy);
  }
  
}


/*
 * Check whether two variables x and y are equal
 * - x and y must be the roots of their equivalence class in the merge table
 */
static inline bool equal_bvvar(bv_solver_t *solver, thvar_t x, thvar_t y) {
  assert(bvvar_bitsize(&solver->vtbl, x) == bvvar_bitsize(&solver->vtbl, y));
  assert(mtbl_is_root(&solver->mtbl, x) && mtbl_is_root(&solver->mtbl, y));

  /*
   * TODO: check for equality between arithmetic expressions
   * (using associativity, distributivity)
   */

  return x == y;
}



/************************
 *  DISEQUALITY CHECKS  *
 ***********************/

/*
 * Check whether two bitarrays a and b are distinct
 * - n = size of both arrays
 * - for now, this returns true if there's an 
 *   index i such that a[i] = not b[i]
 */
static bool diseq_bitarrays(literal_t *a, literal_t *b, uint32_t n) {
  uint32_t i;

  for (i=0; i<n; i++) {
    if (opposite(a[i], b[i])) {
      return true;
    }
  }

  return false;
}


/*
 * Check whether bit array a and small constant c must differ.
 * - n = number of bits in a (and c)
 */
static bool diseq_bitarray_const64(literal_t *a, uint64_t c, uint32_t n) {
  uint32_t i;

  assert(n <= 64);

  /*
   * We use the fact that true_literal = 0 and false_literal = 1
   * So (bit i of c) == a[i] implies a != c
   */
  assert(true_literal == 0 && false_literal == 1);
  
  for (i=0; i<n; i++) {
    if (((int32_t) (c & 1)) == a[i]) {
      return true;
    }
    c >>= 1;
  }

  return false;  
}


/*
 * Same thing for a constant c with more than 64bits
 */
static bool diseq_bitarray_const(literal_t *a, uint32_t *c, uint32_t n) {
  uint32_t i;

  assert(n >= 64);

  /*
   * Same trick as above:
   * - bvconst_tst_bit(c, i) = bit i of c = either 0 or 1
   */
  assert(true_literal == 0 && false_literal == 1);

  for (i=0; i<n; i++) {
    if (bvconst_tst_bit(c, i) == a[i]) {
      return true;
    }
  }

  return false;
}



/*
 * Check whether x and constant c can't be equal
 * - n = number of bits
 * - d = recursion limit
 */
static bool diseq_bvvar_const64(bv_solver_t *solver, thvar_t x, uint64_t c, uint32_t n, uint32_t d) {
  bv_vartable_t *vtbl;
  uint64_t c0, a0;
  thvar_t t;

  assert(bvvar_bitsize(&solver->vtbl, x) == n && 1 <= n && n <= 64);
  assert(c == norm64(c, n));

  vtbl = &solver->vtbl;

  while (d > 0) {
    /*
     * In this loop, we rewrite (x != c) to 
     * true or false, or to an equivalent disequality (x' != c')
     */

    // x should be a root in mtbl
    assert(x == mtbl_get_root(&solver->mtbl, x));

    switch (bvvar_tag(vtbl, x)) {
    case BVTAG_CONST64:
      return bvvar_val64(vtbl, x) != c;

    case BVTAG_BIT_ARRAY:
      return diseq_bitarray_const64(bvvar_bvarray_def(vtbl, x), c, n);

    case BVTAG_POLY64:
      if (bvpoly64_is_simple(solver, bvvar_poly64_def(vtbl, x), &c0, &a0, &t)) {
	if (t == null_thvar || a0 == 0) {
	  // x is equal to c0
	  return (c0 != c);
	} else if (a0 == 1) {
	  // x is equal to c0 + t 
	  // so (x != c) is equivalent to (t != c - c0);
	  x = t;
	  c = norm64(c - c0, n);
	  continue;
	} else if (a0 == mask64(n)) { // a0 = -1
	  // x is equal to c0 - t
	  // so (x != c) is equivalent to t != c0 - c
	  x = t;
	  c = norm64(c0 - c, n);
	  continue;
	}
      }

      /*
       * Fall through intended: if x is a poly of the wrong form
       */
    default:
      return false;
    }

    d --;
  }

  // default answer if d == 0: don't know
  return false;
}



/*
 * Same thing for size > 64
 * - the function uses aux2 and aux3 as buffers so c must not be one of them
 */
static bool diseq_bvvar_const(bv_solver_t *solver, thvar_t x, bvconstant_t *c, uint32_t n, uint32_t d) {
  bv_vartable_t *vtbl;
  bvconstant_t *c0, *a0;
  thvar_t t;

  assert(bvvar_bitsize(&solver->vtbl, x) == n && n > 64);
  assert(c->bitsize == n &&  bvconstant_is_normalized(c));
  assert(c != &solver->aux2 && c != &solver->aux3);

  vtbl = &solver->vtbl;

  while (d > 0) {
    /*
     * In this loop, we rewrite (x != c) to 
     * true or false, or to an equivalent disequality (x' != c')
     */

    // x should be a root in mtbl
    assert(x == mtbl_get_root(&solver->mtbl, x));

    switch (bvvar_tag(vtbl, x)) {
    case BVTAG_CONST:
      return bvconst_neq(bvvar_val(vtbl, x), c->data, c->width);

    case BVTAG_BIT_ARRAY:
      return diseq_bitarray_const(bvvar_bvarray_def(vtbl, x), c->data, n);

    case BVTAG_POLY:
      c0 = &solver->aux2;
      a0 = &solver->aux3;
      if (bvpoly_is_simple(solver, bvvar_poly_def(vtbl, x), c0, a0, &t)) {
	assert(c0->bitsize == n && bvconstant_is_normalized(c0));
	assert(t == null_thvar || (a0->bitsize == n && bvconstant_is_normalized(a0)));

	if (t == null_thvar || bvconstant_is_zero(a0)) {
	  // x is equal to c0
	  return bvconst_neq(c0->data, c->data, c->width);
	} else if (bvconstant_is_one(a0)) {
	  // x is equal to c0 + t 
	  // so (x != c) is equivalent to (t != c - c0);
	  x = t;
	  // compute c := c - c0
	  bvconst_sub(c->data, c->width, c0->data);
	  bvconstant_normalize(c);  
	  continue;
	} else if (bvconstant_is_minus_one(a0)) {
	  // x is equal to c0 - t
	  // so (x != c) is equivalent to t != c0 - c
	  x = t;
	  // compute c:= c0 - c, normalized
	  bvconst_sub(c->data, c->width, c0->data);
	  bvconst_negate(c->data, c->width);
	  bvconstant_normalize(c);
	  continue;
	}	  
      }

      /*
       * Fall through intended: if x is a poly of the wrong form
       */
    default:
      return false;
    }

    d --;
  }

  // default answer if d == 0: don't know
  return false;
}




/*
 * Top-level disequality check
 * - x and y must be roots of their equivalence class in the merge table
 */
static bool diseq_bvvar(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *c;
  bvvar_tag_t tag_x, tag_y;
  uint32_t n;

  assert(bvvar_bitsize(&solver->vtbl, x) == bvvar_bitsize(&solver->vtbl, y));
  assert(mtbl_is_root(&solver->mtbl, x) && mtbl_is_root(&solver->mtbl, y));

  if (x == y) return false;

  vtbl = &solver->vtbl;
  tag_x = bvvar_tag(vtbl, x);
  tag_y = bvvar_tag(vtbl, y);

  n = bvvar_bitsize(vtbl, x);
  if (n <= 64) {
    if (tag_x == BVTAG_CONST64) {
      return diseq_bvvar_const64(solver, y, bvvar_val64(vtbl, x), n, 4); // recursion limit = 4
    }

    if (tag_y == BVTAG_CONST64) {
      return diseq_bvvar_const64(solver, x, bvvar_val64(vtbl, y), n, 4); // recursion limit = 4
    }

    if (tag_x == BVTAG_POLY64 && tag_y == BVTAG_POLY64) {
      return disequal_bvpoly64(bvvar_poly64_def(vtbl, x), bvvar_poly64_def(vtbl, y));      
    }
    
    if (tag_x == BVTAG_BIT_ARRAY && tag_y == BVTAG_BIT_ARRAY) {
      return diseq_bitarrays(bvvar_bvarray_def(vtbl, x), bvvar_bvarray_def(vtbl, y), n);
    }

    if (tag_x == BVTAG_POLY64 && tag_y != BVTAG_CONST64) {
      return bvpoly64_is_const_plus_var(bvvar_poly64_def(vtbl, x), y);
    }

    if (tag_y == BVTAG_POLY64 && tag_x != BVTAG_CONST64) {
      return bvpoly64_is_const_plus_var(bvvar_poly64_def(vtbl, y), x);
    }
    
  } else {

    // More than 64bits
    if (tag_x == BVTAG_CONST) {
      c = &solver->aux1;
      bvconstant_copy(c, n, bvvar_val(vtbl, x)); 
      return diseq_bvvar_const(solver, y, c, n, 4);  // recursion limit = 4
    }

    if (tag_y == BVTAG_CONST) {
      c = &solver->aux1;
      bvconstant_copy(c, n, bvvar_val(vtbl, y)); 
      return diseq_bvvar_const(solver, x, c, n, 4);  // recursion limit = 4
    }

    if (tag_x == BVTAG_POLY && tag_y == BVTAG_POLY) {
      return disequal_bvpoly(bvvar_poly_def(vtbl, x), bvvar_poly_def(vtbl, y));
    }

    if (tag_x == BVTAG_BIT_ARRAY && tag_y == BVTAG_BIT_ARRAY) {
      return diseq_bitarrays(bvvar_bvarray_def(vtbl, x), bvvar_bvarray_def(vtbl, y), n);
    }

    if (tag_x == BVTAG_POLY && tag_y != BVTAG_CONST) {
      return bvpoly_is_const_plus_var(bvvar_poly_def(vtbl, x), y);
    }

    if (tag_y == BVTAG_POLY && tag_x != BVTAG_CONST) {
      return bvpoly_is_const_plus_var(bvvar_poly_def(vtbl, y), x);
    }
      
  }
  
  return false;
}



/*******************
 *  INEQUALITIES   *
 ******************/

/*
 * BOUNDS FOR BIT ARRAYS
 */

/*
 * Compute a lower and upper bound on a bitarray a
 * - n = number of bits in a. n must be no more than 64
 * - the bounds are returned in intv
 * - both are normalized modulo 2^n
 */
static void bitarray_bounds_unsigned64(literal_t *a, uint32_t n, bv64_interval_t *intv) {
  uint64_t low, high;
  uint32_t i;

  assert(0 < n && n <= 64);
  low = 0;
  high = mask64(n);    // all bits equal to 1
  for (i=0; i<n; i++) {
    if (a[i] == false_literal) {
      high = clr_bit64(high, i);
    } else if (a[i] == true_literal) {
      low = set_bit64(low, i);
    }
  }

  assert(low <= high && low == norm64(low, n) && high == norm64(high, n));

  intv->low = low;
  intv->high = high;
  intv->nbits = n;
}

static void bitarray_bounds_signed64(literal_t *a, uint32_t n, bv64_interval_t *intv) {
  uint64_t low, high;
  uint32_t i;

  assert(0 < n && n <= 64);

  low = 0;
  high = mask64(n);   // all bits equal to 1
  for (i=0; i<n-1; i++) {
    if (a[i] == false_literal) {
      high = clr_bit64(high, i);
    } else if (a[i] == true_literal) {
      low = set_bit64(low, i);
    }
  }

  // test the sign bit
  if (a[i] != true_literal) { // sign bit may be 0
    high = clr_bit64(high, i);
  }
  if (a[i] != false_literal) { // sign bit may be 1
    low = set_bit64(low, i);
  }

  assert(signed64_ge(high, low, n) && low == norm64(low, n) && high == norm64(high, n));

  intv->low = low;
  intv->high = high;
  intv->nbits = n;
}


/*
 * Lower/upper bounds for a bit array of more than 64bits
 * - both are normalized modulo 2^n and stored in intv
 */
static void bitarray_bounds_unsigned(literal_t *a, uint32_t n, bv_interval_t *intv) {
  uint32_t i;

  assert(n > 64);

  bv_triv_interval_u(intv, n); // intv->low = 0b00..0, intv->high = 0b111..1
  for (i=0; i<n; i++) {
    if (a[i] == true_literal) {
      bvconst_set_bit(intv->low, i);
    } else if (a[i] == false_literal) {
      bvconst_clr_bit(intv->high, i);
    }
  }
}

static void bitarray_bounds_signed(literal_t *a, uint32_t n, bv_interval_t *intv) {
  uint32_t i;

  assert(n > 64);

  bv_triv_interval_s(intv, n); // intv->low = 0b100000, intv->high = 0b011111

  for (i=0; i<n-1; i++) {
    if (a[i] == true_literal) {
      bvconst_set_bit(intv->low, i);
    } else if (a[i] == false_literal) {
      bvconst_clr_bit(intv->high, i);
    }
  }

  // sign bit
  if (a[i] == false_literal) {   // the sign bit is 0
    bvconst_clr_bit(intv->low, i);
  } else if (a[i] == true_literal) {  // the sign bit is 1
    bvconst_set_bit(intv->high, i);
  }
}


/*
 * GENERAL CASE
 */

/*
 * Recursive computation of bounds on a variable x
 * - d = limit on recursion depth
 * - n = number of bits in x
 * - the functions exist in two versions: one for bitvectors of 64bits at most,
 *   the other for bitvectors of more than 64bits.
 * - the lower bound is returned in *lb
 * - the upper bound is returned in *ub
 */

static void bvvar_bounds_u64(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv64_interval_t *intv);
static void bvvar_bounds_s64(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv64_interval_t *intv);

static void bvvar_bounds_u(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv_interval_t *intv);
static void bvvar_bounds_s(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv_interval_t *intv);



/*
 * Bounds on polynomials
 * - d = recursion limit
 */
static void bvpoly64_bounds_u(bv_solver_t *solver, bvpoly64_t *p, uint32_t d, bv64_interval_t *intv) {
  bv64_interval_t aux;
  uint32_t i, n, nbits;
  thvar_t x;

  n = p->nterms;
  nbits = p->bitsize;
  i = 0;

  // constant term if any
  if (p->mono[i].var == const_idx) {
    bv64_point_interval(intv, p->mono[i].coeff, nbits);
    i ++;
  } else {
    bv64_zero_interval(intv, nbits);
  }

  while (i < n) {
    x = mtbl_get_root(&solver->mtbl, p->mono[i].var);
    bvvar_bounds_u64(solver, x, nbits, d, &aux);
    bv64_interval_addmul_u(intv, &aux, p->mono[i].coeff);
    if (bv64_interval_is_triv_u(intv)) break;      
    i ++;
  }
}

static void bvpoly64_bounds_s(bv_solver_t *solver, bvpoly64_t *p, uint32_t d, bv64_interval_t *intv) {
  bv64_interval_t aux;
  uint32_t i, n, nbits;
  thvar_t x;

  n = p->nterms;
  nbits = p->bitsize;
  i = 0;

  // constant term
  if (p->mono[i].var == const_idx) {
    bv64_point_interval(intv, p->mono[i].coeff, nbits);
    i ++;
  } else {
    bv64_zero_interval(intv, nbits);
  }

  while (i < n) {
    x = mtbl_get_root(&solver->mtbl, p->mono[i].var);
    bvvar_bounds_s64(solver, x, nbits, d, &aux);
    bv64_interval_addmul_s(intv, &aux, p->mono[i].coeff);
    if (bv64_interval_is_triv_s(intv)) break;      
    i ++;
  }
}



/*
 * Bounds on polynomials (more than 64bits)
 * - d = recursion limit
 * - intv = where the bounds are stored
 * - solver->intv_stack must be allocated before this is called
 */
static void bvpoly_bounds_u(bv_solver_t *solver, bvpoly_t *p, uint32_t d, bv_interval_t *intv) {
  bv_interval_t *aux;
  bv_aux_buffers_t *buffers;
  uint32_t i, n, nbits;
  thvar_t x;

  n = p->nterms;
  nbits = p->bitsize;

  aux = get_bv_interval(&solver->intv_stack);
  if (aux == NULL) {
    // no buffer available: return [0b000.., 0b11...]
    bv_triv_interval_u(intv, n);    
  } else {

    buffers = get_bv_aux_buffers(&solver->intv_stack);

    i = 0;
    if (p->mono[i].var == const_idx) {
      bv_point_interval(intv, p->mono[i].coeff, nbits);
      i ++;
    } else {
      bv_zero_interval(intv, nbits);
    }

    while (i < n) {
      x = mtbl_get_root(&solver->mtbl, p->mono[i].var);
      bvvar_bounds_u(solver, x, nbits, d, aux);
      bv_interval_addmul_u(intv, aux, p->mono[i].coeff, buffers);
      if (bv_interval_is_triv_u(intv)) break;
      i ++;
    }

    // cleanup
    release_bv_interval(&solver->intv_stack);
  }
}


static void bvpoly_bounds_s(bv_solver_t *solver, bvpoly_t *p, uint32_t d, bv_interval_t *intv) {
  bv_interval_t *aux;
  bv_aux_buffers_t *buffers;
  uint32_t i, n, nbits;
  thvar_t x;

  n = p->nterms;
  nbits = p->bitsize;

  aux = get_bv_interval(&solver->intv_stack);
  if (aux == NULL) {
    // no buffer available: return [0b100.., 0b011...]
    bv_triv_interval_s(intv, n);    
  } else {

    buffers = get_bv_aux_buffers(&solver->intv_stack);

    i = 0;
    if (p->mono[i].var == const_idx) {
      bv_point_interval(intv, p->mono[i].coeff, nbits);
      i ++;
    } else {
      bv_zero_interval(intv, nbits);
    }

    while (i < n) {
      x = mtbl_get_root(&solver->mtbl, p->mono[i].var);
      bvvar_bounds_s(solver, x, nbits, d, aux);
      bv_interval_addmul_s(intv, aux, p->mono[i].coeff, buffers);
      if (bv_interval_is_triv_s(intv)) break;
      i ++;
    }

    // cleanup
    release_bv_interval(&solver->intv_stack);
  }
}


/*
 * Lower/upper bounds on (bvurem x y)
 * - x is in op[0]
 * - y is in op[1]
 * - n = number of bits in x and y
 */
static void bvurem64_bounds_u(bv_solver_t *solver, thvar_t op[2], uint32_t n, bv64_interval_t *intv) {
  bv_vartable_t *vtbl;
  uint64_t b;
  thvar_t y;

  assert(1 <= n && n <= 64);

  // store default bounds: improve the upper bound if y is a constant
  bv64_triv_interval_u(intv, n);

  vtbl = &solver->vtbl;
  y = op[1];
  if (bvvar_is_const64(vtbl, y)) {
    b = bvvar_val64(vtbl, y);
    if (b != 0) {
      // 0 <= (bvurem x y) <= b-1
      intv->high = norm64(b-1, n);
    }
  }  
}

static void bvurem_bounds_u(bv_solver_t *solver, thvar_t op[2], uint32_t n, bv_interval_t *intv) {
  bv_vartable_t *vtbl;
  uint32_t *b;
  uint32_t k;
  thvar_t y;

  assert(n > 64);

  bv_triv_interval_u(intv, n);
  vtbl = &solver->vtbl;
  y = op[1];
  if (bvvar_is_const(vtbl, y)) {
    k = (n + 31) >> 5;
    b = bvvar_val(vtbl, y);
    if (bvconst_is_nonzero(b, k)) {
      // 0 <= (bvurem x y) <= b-1
      bvconst_set(intv->high, k, b);
      bvconst_sub_one(intv->high, k);
      assert(bvconst_is_normalized(intv->high, n));
    }
  }
}


/*
 * Lower/upper bounds on (bvsrem x y) or (bvsmod x y)
 * - x is in op[0]
 * - y is in op[1]
 * - n = number of bits in x and y
 */
static void bvsrem64_bounds_s(bv_solver_t *solver, thvar_t op[2], uint32_t n, bv64_interval_t *intv) {
  bv_vartable_t *vtbl;
  uint64_t b;
  thvar_t y;

  assert(1 <= n && n <= 64);

  // store default bounds: improve the upper bound if y is a constant
  bv64_triv_interval_s(intv, n);

  vtbl = &solver->vtbl;
  y = op[1];
  if (bvvar_is_const64(vtbl, y)) {
    b = bvvar_val64(vtbl, y);
    if (b > 0) {
      // -b+1 <= (bvsrem x y) <= b-1
      intv->low = norm64(-b+1, n);
      intv->high = norm64(b-1, n);
    } else if (b < 0) {
      // b + 1 <= (bvsrem x y) <= - b - 1
      intv->low = norm64(b + 1, n);
      intv->high = norm64(-b -1, n);
    }
    assert(bv64_interval_is_normalized(intv) && signed64_le(intv->low, intv->high, n));
  }
}

static void bvsrem_bounds_s(bv_solver_t *solver, thvar_t op[2], uint32_t n, bv_interval_t *intv) {
  bv_vartable_t *vtbl;
  uint32_t *b;
  uint32_t k;
  thvar_t y;

  assert(n > 64);

  bv_triv_interval_s(intv, n);
  vtbl = &solver->vtbl;
  y = op[1];
  if (bvvar_is_const(vtbl, y)) {
    k = (n + 31) >> 5;
    b = bvvar_val(vtbl, y);
    if (bvconst_is_nonzero(b, k)) {
      if (bvconst_tst_bit(b, n-1)) {
	// negative divider
	// b + 1 <= (bvserm x y) <= -(b+1)
	bvconst_set(intv->low, k, b);
	bvconst_add_one(intv->low, k);
	bvconst_set(intv->high, k, intv->low);
	bvconst_negate(intv->high, k);
	bvconst_normalize(intv->high, n);
      } else {
	// positive divider
	// -(b-1) <= (bvsrem x y) <= b-1
	bvconst_set(intv->high, k, b);
	bvconst_sub_one(intv->high, k);
	bvconst_set(intv->low, k, intv->high);
	bvconst_negate(intv->low, k);
	bvconst_normalize(intv->low, n);
      }
    }
    assert(bv_interval_is_normalized(intv) && bvconst_le(intv->low, intv->high, n));
  }
}




/*
 * Lower/upper bound for a bitvector variable x
 * - n = bitsize of x: must be between 1 and 64
 */
static void bvvar_bounds_u64(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv64_interval_t *intv) {
  bv_vartable_t *vtbl;
  bvvar_tag_t tag_x;
  uint64_t c;

  vtbl = &solver->vtbl;

  assert(valid_bvvar(vtbl, x) && n == bvvar_bitsize(vtbl, x) && 1 <= n && n <= 64);

  tag_x = bvvar_tag(vtbl, x);

  if (tag_x == BVTAG_CONST64) {
    bv64_point_interval(intv, bvvar_val64(vtbl, x), n);
    return;
  }

  if (tag_x == BVTAG_BIT_ARRAY) {
    bitarray_bounds_unsigned64(bvvar_bvarray_def(vtbl, x), n, intv);
  } else if (tag_x == BVTAG_POLY64 && d>0) {
    bvpoly64_bounds_u(solver, bvvar_poly64_def(vtbl, x), d-1, intv);
  } else if (tag_x == BVTAG_UREM) {
    bvurem64_bounds_u(solver, bvvar_binop(vtbl, x), n, intv);
  } else {
    // default bounds
    bv64_triv_interval_u(intv, n);
  }


  /*
   * Check for better bounds in the bound queue 
   *
   * Note: if an asserted lower bound on c is greater than intv->high
   * then the constraints are unsat (but this will be detected later).
   */
  if (bvvar_has_lower_bound64(solver, x, &c) && c > intv->low && c <= intv->high) {
    intv->low = c;
  }
  if (bvvar_has_upper_bound64(solver, x, &c) && c < intv->high && c >= intv->low) {
    intv->high = c;
  }

  assert(bv64_interval_is_normalized(intv) && intv->low <= intv->high);
}


/*
 * Same thing: for bitvectors interpreted as signed integers
 */
static void bvvar_bounds_s64(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv64_interval_t *intv) {
  bv_vartable_t *vtbl;
  bvvar_tag_t tag_x;
  uint64_t c;

  vtbl = &solver->vtbl;

  assert(valid_bvvar(vtbl, x) && n == bvvar_bitsize(vtbl, x) && 1 <= n && n <= 64);

  tag_x = bvvar_tag(vtbl, x);  

  if (tag_x == BVTAG_CONST64) {
    bv64_point_interval(intv, bvvar_val64(vtbl, x), n);
    return;
  } 

  if (tag_x == BVTAG_BIT_ARRAY) {
    bitarray_bounds_signed64(bvvar_bvarray_def(vtbl, x), n, intv);
  } else if (tag_x == BVTAG_POLY64 && d>0) {
    bvpoly64_bounds_s(solver, bvvar_poly64_def(vtbl, x), d-1, intv);
  } else if (tag_x == BVTAG_SREM || tag_x == BVTAG_SMOD) {
    bvsrem64_bounds_s(solver, bvvar_binop(vtbl, x), n, intv);
  } else {
    // default bounds
    bv64_triv_interval_s(intv, n);
  }
  
  /*
   * Check for better bounds in the queue
   */
  if (bvvar_has_signed_lower_bound64(solver, x, &c) && signed64_gt(c, intv->low, n) && 
      signed64_le(c, intv->high, n)) {
    intv->low = c;
  }
  if (bvvar_has_signed_upper_bound64(solver, x, &c) && signed64_lt(c, intv->high, n) &&
      signed64_ge(c, intv->low, n)) {
    intv->high = c;
  }

  assert(bv64_interval_is_normalized(intv) && signed64_le(intv->low, intv->high, n));
}


/*
 * Lower/upper bound for a bitvector variable x
 * - n = bitsize of x: must be more than 64
 * - the result is stored in intv
 */
static void bvvar_bounds_u(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv_interval_t *intv) {
  bv_vartable_t *vtbl;
  bvconstant_t *c;
  bvvar_tag_t tag_x;

  vtbl = &solver->vtbl;

  assert(valid_bvvar(vtbl, x) && n == bvvar_bitsize(vtbl, x) && 64 < n);

  tag_x = bvvar_tag(vtbl, x);

  if (tag_x == BVTAG_CONST) {
    bv_point_interval(intv, bvvar_val(vtbl, x), n);
    return;
  }

  if (tag_x == BVTAG_BIT_ARRAY) {
    bitarray_bounds_unsigned(bvvar_bvarray_def(vtbl, x), n, intv);
  } else if (tag_x == BVTAG_POLY && d > 0) {
    bvpoly_bounds_u(solver, bvvar_poly_def(vtbl, x), d-1, intv);
  } else if (tag_x == BVTAG_UREM) {
    bvurem_bounds_u(solver, bvvar_binop(vtbl, x), n, intv);
  } else {
    // default bounds
    bv_triv_interval_u(intv, n);
  }

  /*
   * Check for better bounds in the queue
   */
  c = &solver->aux1;

  if (bvvar_has_lower_bound(solver, x, n, c) && 
      bvconst_gt(c->data, intv->low, n) &&
      bvconst_le(c->data, intv->high, n)) {
    // c: lower bound on x with c <= intv->high and c > intv->low
    // replace intv->low by c
    assert(bvconstant_is_normalized(c));
    bvconst_set(intv->low, c->width, c->data);
  }

  if (bvvar_has_upper_bound(solver, x, n, c) &&
      bvconst_lt(c->data, intv->high, n) &&
      bvconst_ge(c->data, intv->low, n)) {
    // c: upper bound on x with c >= intv->low and c < intv->high
    // replace intv->high by c
    assert(bvconstant_is_normalized(c));
    bvconst_set(intv->high, c->width, c->data);
  }

  assert(bv_interval_is_normalized(intv) && bvconst_le(intv->low, intv->high, n));
}


static void bvvar_bounds_s(bv_solver_t *solver, thvar_t x, uint32_t n, uint32_t d, bv_interval_t *intv) {
  bv_vartable_t *vtbl;
  bvconstant_t *c;
  bvvar_tag_t tag_x;

  vtbl = &solver->vtbl;

  assert(valid_bvvar(vtbl, x) && n == bvvar_bitsize(vtbl, x) && 64 < n);

  tag_x = bvvar_tag(vtbl, x);

  if (tag_x == BVTAG_CONST) {
    bv_point_interval(intv, bvvar_val(vtbl, x), n);
    return;
  }

  if (tag_x == BVTAG_BIT_ARRAY) {
    bitarray_bounds_signed(bvvar_bvarray_def(vtbl, x), n, intv);
  } else if (tag_x == BVTAG_POLY && d > 0) {
    bvpoly_bounds_s(solver, bvvar_poly_def(vtbl, x), d-1, intv);
  } else if (tag_x == BVTAG_SREM || tag_x == BVTAG_SMOD) {
    bvsrem_bounds_s(solver, bvvar_binop(vtbl, x), n, intv);
  } else {
    // default bounds
    bv_triv_interval_s(intv, n);
  }

  /*
   * Check for better bounds in the queue
   */
  c = &solver->aux1;

  if (bvvar_has_signed_lower_bound(solver, x, n, c) && 
      bvconst_sgt(c->data, intv->low, n) &&
      bvconst_sle(c->data, intv->high, n)) {
    // c: lower bound on x with c <= intv->high and c > intv->low
    // replace intv->low by c
    assert(bvconstant_is_normalized(c));
    bvconst_set(intv->low, c->width, c->data);
  }

  if (bvvar_has_signed_upper_bound(solver, x, n, c) &&
      bvconst_slt(c->data, intv->high, n) &&
      bvconst_sge(c->data, intv->low, n)) {
    // c: upper bound on x with c >= intv->low and c < intv->high
    // replace intv->high by c
    assert(bvconstant_is_normalized(c));
    bvconst_set(intv->high, c->width, c->data);
  }

  assert(bv_interval_is_normalized(intv) && bvconst_sle(intv->low, intv->high, n));
}




/*
 * SIMPLIFY INEQUALITIES
 */

/*
 * Three possible codes returned by the  'check_bvuge' and 'check_bvsge' functions
 * - the order matters: we want BVTEST_FALSE = 0 = false and BVTEST_TRUE = 1= true
 */
typedef enum {
  BVTEST_FALSE = 0,
  BVTEST_TRUE,
  BVTEST_UNKNOWN,
} bvtest_code_t;


/*
 * Check whether (x >= y) simplifies (unsigned)
 * - n = number of bits in x and y
 * - both x and y must be 64bits or less
 */
static bvtest_code_t check_bvuge64_core(bv_solver_t *solver, thvar_t x, thvar_t y, uint32_t n) {
  bv64_interval_t intv_x, intv_y;

  bvvar_bounds_u64(solver, x, n, 4, &intv_x);  // intv_x.low <= x <= intv_x.high
  bvvar_bounds_u64(solver, y, n, 4, &intv_y);  // intv_y.low <= y <= intv_y.high

  if (intv_x.low >= intv_y.high) {
    return BVTEST_TRUE;
  }

  if (intv_x.high < intv_y.low) {
    return BVTEST_FALSE;
  }

  return BVTEST_UNKNOWN;
}


/*
 * Check whether (x >= y) simplifies (unsigned)
 * - n = number of bits in x and y
 * - both x and y must be more than 64bits
 */
static bvtest_code_t check_bvuge_core(bv_solver_t *solver, thvar_t x, thvar_t y, uint32_t n) {
  bv_interval_t *bounds_x, *bounds_y;

  // prepare interval stack
  alloc_bv_interval_stack(&solver->intv_stack);
  bounds_x = get_bv_interval(&solver->intv_stack);
  bounds_y = get_bv_interval(&solver->intv_stack);

  assert(bounds_x != NULL && bounds_y != NULL);

  bvvar_bounds_u(solver, x, n, 4, bounds_x);  // bounds_x.low <= x <= bounds_x.high
  bvvar_bounds_u(solver, y, n, 4, bounds_y);  // bounds_y.low <= y <= bounds_y.high


  /*
   * hack: empty the interval stack here
   * bounds_x and bounds_y are still good pointers in stack->data
   */
  assert(solver->intv_stack.top == 2);
  release_all_bv_intervals(&solver->intv_stack);

  if (bvconst_ge(bounds_x->low, bounds_y->high, n)) {
    return BVTEST_TRUE;
  }

  if (bvconst_lt(bounds_x->high, bounds_y->low, n)) {
    return BVTEST_FALSE;
  }

  return BVTEST_UNKNOWN;
}


/*
 * Check whether (x >= y) simplifies (unsigned)
 * - x and y must be roots in the merge table
 * - Return BVTEST_FALSE if (x > y) is known to hold
 * - return BVTEST_TRUE  if (x >= y) is known to hold
 * - return BVTEST_UNKNOWN otherwise
 */
static bvtest_code_t check_bvuge(bv_solver_t *solver, thvar_t x, thvar_t y) {
  uint32_t n;
  bvtest_code_t code;

  assert(bvvar_bitsize(&solver->vtbl, x) == bvvar_bitsize(&solver->vtbl, y));
  assert(mtbl_is_root(&solver->mtbl, x) && mtbl_is_root(&solver->mtbl, y));

  if (x == y) return BVTEST_TRUE;

  n = bvvar_bitsize(&solver->vtbl, x);

  if (n <= 64) {
    code = check_bvuge64_core(solver, x, y, n);
    
  } else {
    code = check_bvuge_core(solver, x, y, n);
  }

  return code;
}


/*
 * Check whether (x >= y) simplifies (signed comparison)
 * - n = number of bits in x and y
 * - both x and y must be 64 bits or less
 */
static bvtest_code_t check_bvsge64_core(bv_solver_t *solver, thvar_t x, thvar_t y, uint32_t n) {
  bv64_interval_t intv_x, intv_y;

  bvvar_bounds_s64(solver, x, n, 1, &intv_x);  // intv_x.low <= x <= intv_x.high
  bvvar_bounds_s64(solver, y, n, 1, &intv_y);  // intv_y.low <= y <= intv_y.high

  if (signed64_ge(intv_x.low, intv_y.high, n)) { // lx >= uy
    return BVTEST_TRUE;
  }

  if (signed64_lt(intv_x.high, intv_y.low, n)) { // ux < ly
    return BVTEST_FALSE;
  }

  return BVTEST_UNKNOWN;
}

/*
 * Check whether (x >= y) simplifies (signed comparison)
 * - n = number of bits in x and y
 * - both x and y must be more than 64 bits
 */
static bvtest_code_t check_bvsge_core(bv_solver_t *solver, thvar_t x, thvar_t y, uint32_t n) {
  bv_interval_t *bounds_x, *bounds_y;

  // prepare interval stack
  alloc_bv_interval_stack(&solver->intv_stack);
  bounds_x = get_bv_interval(&solver->intv_stack);
  bounds_y = get_bv_interval(&solver->intv_stack);

  assert(bounds_x != NULL && bounds_y != NULL);

  bvvar_bounds_s(solver, x, n, 4, bounds_x);  // bounds_x.low <= x <= bounds_x.high
  bvvar_bounds_s(solver, y, n, 4, bounds_y);  // bounds_y.low <= y <= bounds_y.high

  /*
   * hack: empty the interval stack here
   * bounds_x and bounds_y are still good pointers in stack->data
   */
  assert(solver->intv_stack.top == 2);
  release_all_bv_intervals(&solver->intv_stack);

  if (bvconst_sge(bounds_x->low, bounds_y->high, n)) {
    return BVTEST_TRUE;
  }

  if (bvconst_slt(bounds_x->high, bounds_y->low, n)) {
    return BVTEST_FALSE;
  }

  return BVTEST_UNKNOWN;
}


/*
 * Check whether (x <= y) simplifies (signed)
 * - x and y must be roots in the merge table
 * - return BVTEST_FALSE if (x > y) is known to hold
 * - return BVTEST_TRUE  if (x >= y) is known to hold
 * - return BVTEST_UNKNOWN otherwise
 */
static bvtest_code_t check_bvsge(bv_solver_t *solver, thvar_t x, thvar_t y) {
  uint32_t n;
  bvtest_code_t code;

  assert(bvvar_bitsize(&solver->vtbl, x) == bvvar_bitsize(&solver->vtbl, y));
  assert(mtbl_is_root(&solver->mtbl, x) && mtbl_is_root(&solver->mtbl, y));

  if (x == y) return BVTEST_TRUE;
  
  n = bvvar_bitsize(&solver->vtbl, x);

  if (n <= 64) {
    code = check_bvsge64_core(solver, x, y, n);
  } else {
    code = check_bvsge_core(solver, x, y, n);
  }

  return code;
}




/*****************************************
 *  SIMPLIFICATION + TERM CONSTRUCTION   *
 ****************************************/

/*
 * Heuristic: attempt to simplify a polynomial
 * p = a0 + a1 t1 + ... + an tn
 * - if t_i is itself a polynomial, we replace t_i by its 
 *   definition q, and check whether p[t_i/q] is simpler than p
 */


/*
 * Add a * x to buffer b
 * - replace x by its value if it's a constant
 * - also replace x by its root value if the root is a constant
 * - b, a, and x must all have the same bitsize
 */
static void bvbuffer_add_mono64(bv_solver_t *solver, bvpoly_buffer_t *b, thvar_t x, uint64_t a) {
  bv_vartable_t *vtbl;
  thvar_t y;

  vtbl = &solver->vtbl;
  y = bvvar_root_if_const64(solver, x);
  if (bvvar_is_const64(vtbl, y)) {
    bvpoly_buffer_add_const64(b, a * bvvar_val64(vtbl, y));
  } else {
    bvpoly_buffer_add_mono64(b, y, a);
  }
}

// same thing for bitsize > 64
static void bvbuffer_add_mono(bv_solver_t *solver, bvpoly_buffer_t *b, thvar_t x, uint32_t *a) {
  bv_vartable_t *vtbl;
  thvar_t y;

  vtbl = &solver->vtbl;
  y = bvvar_root_if_const(solver, x);
  if (bvvar_is_const(vtbl, y)) {
    // add const_idx * a * value of y
    bvpoly_buffer_addmul_monomial(b, const_idx, a, bvvar_val(vtbl, y));
  } else {
    bvpoly_buffer_add_monomial(b, y, a);
  }
}


/*
 * Build the variable for a polynomial stored in buffer:
 * - check whether buffer is reduced to a constant or a variable
 */
static thvar_t map_bvpoly(bv_solver_t *solver, bvpoly_buffer_t *b) {
  bv_vartable_t *vtbl;
  thvar_t x;
  uint32_t n, nbits;

  vtbl = &solver->vtbl;

  n = bvpoly_buffer_num_terms(b);
  nbits = bvpoly_buffer_bitsize(b);

  if (n == 0) {
    // return the constant 0b000...0 
    bvconstant_set_all_zero(&solver->aux1, nbits);
    x = get_bvconst(vtbl, nbits, solver->aux1.data);
    return x;
  }

  if (n == 1) {
    x = bvpoly_buffer_var(b, 0);
    if (x == const_idx) {
      // p is a constant 
      x = get_bvconst(vtbl, nbits, bvpoly_buffer_coeff(b, 0));
      return x;
    }

    if (bvconst_is_one(bvpoly_buffer_coeff(b, 0), (nbits + 31) >> 5)) {
      // p is equal to 1 . x
      return x;
    }
  }
   
  // no simplification
  x = get_bvpoly(vtbl, b);

  return x;
}


/*
 * Same thing for a polynomial with 64bit coefficients
 */
static thvar_t map_bvpoly64(bv_solver_t *solver, bvpoly_buffer_t *b) {
  bv_vartable_t *vtbl;
  thvar_t x;
  uint32_t n, nbits;

  vtbl = &solver->vtbl;

  n = bvpoly_buffer_num_terms(b);
  nbits = bvpoly_buffer_bitsize(b);
    
  if (n == 0) {
    // return the constant 0b000 ..0
    x = get_bvconst64(vtbl, nbits, 0);
    return x;
  }

  if (n == 1) {
    x = bvpoly_buffer_var(b, 0);
    if (x == const_idx) {
      // constant
      x = get_bvconst64(vtbl, nbits, bvpoly_buffer_coeff64(b, 0));
      return x;
    }

    if (bvpoly_buffer_coeff64(b, 0) == 1) {
      return x;
    }
  }

  // no simplification
  x = get_bvpoly64(vtbl, b);

  return x;
}



/*
 * Map a power product p to a variable
 * - nbits = number of bits in all variables of p
 * - return null_thvar if p is the empty product
 */
static thvar_t map_product(bv_vartable_t *table, uint32_t nbits, pp_buffer_t *p) {
  uint32_t n;
  thvar_t x;

  n = p->len;
  if (n == 0) {
    x = null_thvar;
  } else if (n == 1 && p->prod[0].exp == 1) {
    x = p->prod[0].var;
  } else {
    x = get_bvpprod(table, nbits, p);
  }

  return x;
}


/*
 * Build the term c * x (as a polynomial)
 * - nbits = number of bits in c and x
 */
static thvar_t make_mono64(bv_solver_t *solver, uint32_t nbits, uint64_t c, thvar_t x) {
  bvpoly_buffer_t *b;

  b = &solver->buffer;
  reset_bvpoly_buffer(b, nbits);
  bvpoly_buffer_add_mono64(b, x, c);

  return get_bvpoly64(&solver->vtbl, b);
}


/*
 * Build the term (c * p)
 * - c is a 64bit constants
 * - p is a power product
 * - nbits = number of bits in c (and in all variables of p)
 */
static thvar_t map_const64_times_product(bv_solver_t *solver, uint32_t nbits, pp_buffer_t *p, uint64_t c) {
  bv_vartable_t *vtbl;
  thvar_t x;

  assert(c == norm64(c, nbits));

  vtbl = &solver->vtbl;

  if (c == 0) {
    x = get_bvconst64(vtbl, nbits, 0);
  } else {
    x = map_product(vtbl, nbits, p);
    if (x == null_thvar) { 
      // empty product: p = 1
      x = get_bvconst64(vtbl, nbits, c);
    } else if (c != 1) {
      x = make_mono64(solver, nbits, c, x);
    }
  }

  return x;
}



/*
 * Build the term c * x (as a polynomial)
 * - nbits = number of bits in c and x (nbits > 64)
 * - c must be normalized
 */
static thvar_t make_mono(bv_solver_t *solver, uint32_t nbits, uint32_t *c, thvar_t x) {
  bvpoly_buffer_t *b;

  b = &solver->buffer;
  reset_bvpoly_buffer(b, nbits);
  bvpoly_buffer_add_monomial(b, x, c);

  return get_bvpoly(&solver->vtbl, b);
}


/*
 * Build the term (c * p)
 * - c is a constants with more than 64bits
 * - p is a power product
 * - nbits = number of bits in c (and in all variables of p)
 */
static thvar_t map_const_times_product(bv_solver_t *solver, uint32_t nbits, pp_buffer_t *p, uint32_t *c) {
  bv_vartable_t *vtbl;
  uint32_t w;
  thvar_t x;

  vtbl = &solver->vtbl;
  w = (nbits + 31) >> 5;

  if (bvconst_is_zero(c, w)) {
    x = get_bvconst(vtbl, nbits, c); // constant 0b0000...0
  } else {
    x = map_product(vtbl, nbits, p);
    if (x == null_thvar) { 
      // empty product: p = 1
      x = get_bvconst(vtbl, nbits, c);
    } else if (! bvconst_is_one(c, w)) {
      x = make_mono(solver, nbits, c, x);
    }
  }

  return x;
}



/*
 * Check whether all literals in a[0 ... n-1] are constant
 */
static bool bvarray_is_constant(literal_t *a, uint32_t n) {
  uint32_t i;

  for (i=0; i<n; i++) {
    if (var_of(a[i]) != const_bvar) return false;
  }

  return true;
}


/*
 * Convert constant array a[0 ... n-1] to a 64bit unsigned integer
 * - a[0] = low order bit
 * - a[n-1] = high order bit
 */
static uint64_t bvarray_to_uint64(literal_t *a, uint32_t n) {
  uint64_t c;

  assert(1 <= n && n <= 64);
  c = 0;
  while (n > 0) {
    n --;
    assert(a[n] == true_literal || a[n] == false_literal);
    c = (c << 1) | is_pos(a[n]);
  }

  return c;
}


/*
 * Convert constant array a[0 ... n-1] to a bitvecotr constant
 * - copy the result in c
 */
static void bvarray_to_bvconstant(literal_t *a, uint32_t n, bvconstant_t *c) {
  uint32_t i;

  assert(n > 64);

  bvconstant_set_all_zero(c, n);
  for (i=0; i<n; i++) {
    assert(a[i] == true_literal || a[i] == false_literal);
    if (a[i] == true_literal) {
      bvconst_set_bit(c->data, i);
    }
  }
}



/*
 * Extract bit i of a 64bit constant x
 * - convert to true_literal or false_literal
 */
static literal_t bvconst64_get_bit(bv_vartable_t *vtbl, thvar_t x, uint32_t i) {
  literal_t l;
  
  l = false_literal;
  if (tst_bit64(bvvar_val64(vtbl, x), i)) {
    l= true_literal;
  } 

  return l;
}


/*
 * Extract bit i of a general constant x
 */
static literal_t bvconst_get_bit(bv_vartable_t *vtbl, thvar_t x, uint32_t i) {
  literal_t l;

  l = false_literal;
  if (bvconst_tst_bit(bvvar_val(vtbl, x), i)) {
    l = true_literal;
  }

  return l;
}


/*
 * Extract bit i of a bvarray variable x
 */
static literal_t bvarray_get_bit(bv_vartable_t *vtbl, thvar_t x, uint32_t i) {
  literal_t *a;

  assert(i < bvvar_bitsize(vtbl, x));

  a = bvvar_bvarray_def(vtbl, x);
  return a[i];
}


/*
 * Extract bit i of variable x: 
 * - get it from the pseudo literal array mapped to x
 */
static literal_t bvvar_get_bit(bv_solver_t *solver, thvar_t x, uint32_t i) {
  remap_table_t *rmap;
  literal_t *map;
  literal_t r, l;

  map = bv_solver_get_pseudo_map(solver, x);

  rmap = solver->remap;
  r = remap_table_find_root(rmap, map[i]); // r := root of map[i] 
  l = remap_table_find(rmap, r); // l := real literal for r
  if (l == null_literal) {
    // nothing attached to r: create a new literal and attach it to r
    l = pos_lit(create_boolean_variable(solver->core));
    remap_table_assign(rmap, r, l);
  }

  return l;
}


/*
 * Bounds on (urem z y): if y != 0 then 0 <= (urem z y) < y
 * - x must be equal to (urem z y) for some z
 */
static void assert_urem_bounds(bv_solver_t *solver, thvar_t x, thvar_t y) {
  literal_t l0, l1;
  thvar_t zero;
  uint32_t n;

  assert(bvvar_tag(&solver->vtbl, x) == BVTAG_UREM && solver->vtbl.def[x].op[1] == y);

  n = bvvar_bitsize(&solver->vtbl, y);
  zero = get_zero(solver, n);
  l0 = bv_solver_create_eq_atom(solver, y, zero); // (y == 0)
  l1 = bv_solver_create_ge_atom(solver, x, y);    // (bvurem z y) >= y
  add_binary_clause(solver->core, l0, not(l1));
}


/*
 * Bounds on (srem z y) or  (smod z y):
 * - if y > 0 then - y < (srem z y) < y
 * - if y < 0 then   y < (srem z y) < - y
 * Same thing for (smod z y)
 *
 * We don't want to create the term -y so we add only half
 * of these constraints:
 *  y>0 ==> (srem z y) < y
 *  y<0 ==> y < (srem z y)
 */
static void assert_srem_bounds(bv_solver_t *solver, thvar_t x, thvar_t y) {
  literal_t l0, l1;
  thvar_t zero;
  uint32_t n;

  assert(bvvar_tag(&solver->vtbl, x) == BVTAG_SREM || bvvar_tag(&solver->vtbl, x) == BVTAG_SMOD);
  assert(solver->vtbl.def[x].op[1] == y);

  n = bvvar_bitsize(&solver->vtbl, y);
  zero = get_zero(solver, n);

  l0 = bv_solver_create_sge_atom(solver, zero, y); // (y <= 0)
  l1 = bv_solver_create_sge_atom(solver, x, y);    // (bvsrem z y) >= y
  add_binary_clause(solver->core, l0, not(l1));    // (y > 0) ==> (bvsrem z y) < y

  l0 = bv_solver_create_sge_atom(solver, y, zero); // (y >= 0)
  l1 = bv_solver_create_sge_atom(solver, y, x);    // y >= (bvsrem z y)
  add_binary_clause(solver->core, l0, not(l1));    // (y < 0) ==> y < (bvsrem z y)
}


/**********************
 *  SOLVER INTERFACE  *
 *********************/

void bv_solver_start_internalization(bv_solver_t *solver) {
}

void bv_solver_start_search(bv_solver_t *solver) {
}

bool bv_solver_propagate(bv_solver_t *solver) {
  return true;
}

fcheck_code_t bv_solver_final_check(bv_solver_t *solver) {
  return FCHECK_SAT;
}

void bv_solver_increase_decision_level(bv_solver_t *solver) {
  solver->decision_level ++;
}

void bv_solver_backtrack(bv_solver_t *solver, uint32_t backlevel) {
  assert(solver->base_level <= backlevel && backlevel < solver->decision_level);
  reset_eassertion_queue(&solver->egraph_queue);
  solver->decision_level = backlevel;
}

bool bv_solver_assert_atom(bv_solver_t *solver, void *a, literal_t l) {
  return true;
}

void bv_solver_expand_explanation(bv_solver_t *solver, literal_t l, void *expl, ivector_t *v) {
  assert(false);
}

literal_t bv_solver_select_polarity(bv_solver_t *solver, void *a, literal_t l) {
  return l;
}



/**********************
 *  EGRAPH INTERFACE  *
 *********************/

void bv_solver_assert_var_eq(bv_solver_t *solver, thvar_t x, thvar_t y) {
}

void bv_solver_assert_var_diseq(bv_solver_t *solver, thvar_t x, thvar_t y, composite_t *hint) {
}

void bv_solver_assert_var_distinct(bv_solver_t *solver, uint32_t n, thvar_t *a, composite_t *hint) {
}


bool bv_solver_check_disequality(bv_solver_t *solver, thvar_t x, thvar_t y) {
  return false;
}


uint32_t bv_solver_reconcile_model(bv_solver_t *solver, uint32_t max_eq) {
  return 0;
}

literal_t bv_solver_select_eq_polarity(bv_solver_t *solver, thvar_t x, thvar_t y, literal_t l) {
  return l;
}

bool bv_solver_value_in_model(bv_solver_t *solver, thvar_t x, bvconstant_t *v) {
  return false;
}

bool bv_solver_fresh_value(bv_solver_t *solver, bvconstant_t *v, uint32_t n) {
  return false;
}


/**********************
 *  MAIN OPERATIONS   *
 *********************/

/*
 * Initialize a bit-vector solver
 * - core = the attached smt core
 * - egraph = the attached egraph (or NULL)
 */
void init_bv_solver(bv_solver_t *solver, smt_core_t *core, egraph_t *egraph) {
  solver->core = core;
  solver->egraph = egraph;
  solver->base_level = 0;
  solver->decision_level = 0;

  init_bv_vartable(&solver->vtbl);
  init_bv_atomtable(&solver->atbl);
  init_mtbl(&solver->mtbl);
  init_bv_bound_queue(&solver->bqueue);

  solver->blaster = NULL;
  solver->remap = NULL;

  init_eassertion_queue(&solver->egraph_queue);

  init_bv_trail(&solver->trail_stack);

  init_bvpoly_buffer(&solver->buffer);
  init_pp_buffer(&solver->prod_buffer, 10);
  init_ivector(&solver->aux_vector, 0);
  init_bvconstant(&solver->aux1);
  init_bvconstant(&solver->aux2);
  init_bvconstant(&solver->aux3);
  init_bv_interval_stack(&solver->intv_stack);
  init_ivector(&solver->a_vector, 0);
  init_ivector(&solver->b_vector, 0);

  init_used_vals(&solver->used_vals);

  solver->env = NULL;
}


/*
 * Attach a jump buffer for exception handling
 */
void bv_solver_init_jmpbuf(bv_solver_t *solver, jmp_buf *buffer) {
  solver->env = buffer;
}


/*
 * Delete solver
 */
void delete_bv_solver(bv_solver_t *solver) {
  delete_bv_vartable(&solver->vtbl);
  delete_bv_atomtable(&solver->atbl);
  delete_mtbl(&solver->mtbl);
  delete_bv_bound_queue(&solver->bqueue);

  if (solver->blaster != NULL) {
    delete_bit_blaster(solver->blaster);
    safe_free(solver->blaster);
    solver->blaster = NULL;
  }

  if (solver->remap != NULL) {
    delete_remap_table(solver->remap);
    safe_free(solver->remap);
    solver->remap = NULL;
  }

  delete_eassertion_queue(&solver->egraph_queue);

  delete_bv_trail(&solver->trail_stack);

  delete_bvpoly_buffer(&solver->buffer);
  delete_pp_buffer(&solver->prod_buffer);
  delete_ivector(&solver->aux_vector);
  delete_bvconstant(&solver->aux1);
  delete_bvconstant(&solver->aux2);
  delete_bvconstant(&solver->aux3);
  delete_bv_interval_stack(&solver->intv_stack);
  delete_ivector(&solver->a_vector);
  delete_ivector(&solver->b_vector);

  delete_used_vals(&solver->used_vals);
}


/********************
 *  PUSH/POP/RESET  *
 *******************/

/*
 * Start a new base level
 */
void bv_solver_push(bv_solver_t *solver) {
  uint32_t na, nv, nb;

  assert(solver->decision_level == solver->base_level);

  nv = solver->vtbl.nvars;
  na = solver->atbl.natoms;
  nb = solver->bqueue.top;
  bv_trail_save(&solver->trail_stack, nv, na, nb);

  mtbl_push(&solver->mtbl);

  solver->base_level ++;
  bv_solver_increase_decision_level(solver);
}



/*
 * Remove all eterms whose id >= number of terms in the egraph
 * - if a bitvector variable x is kept after pop but the
 *   eterm[x] is removed from the egraph then we must clear
 *   solver->vtbl.eterm[x]
 */
static void bv_solver_remove_dead_eterms(bv_solver_t *solver) {
  uint32_t nterms;

  if (solver->egraph != NULL) {
    nterms = egraph_num_terms(solver->egraph);
    bv_vartable_remove_eterms(&solver->vtbl, nterms);
  }
}


/*
 * Return to the previous base level
 */
void bv_solver_pop(bv_solver_t *solver) {
  bv_trail_t *top;

  assert(solver->base_level > 0 &&
	 solver->base_level == solver->decision_level);

  solver->base_level --;
  bv_solver_backtrack(solver, solver->base_level);

  top = bv_trail_top(&solver->trail_stack);

  bv_solver_remove_bounds(solver, top->natoms);
  bv_vartable_remove_vars(&solver->vtbl, top->nvars);
  bv_atomtable_remove_atoms(&solver->atbl, top->natoms);
  bv_solver_remove_dead_eterms(solver);

  mtbl_pop(&solver->mtbl);

  bv_trail_pop(&solver->trail_stack);
}


/*
 * Reset: remove all variables and atoms
 * and reset base_level to 0.
 */
void bv_solver_reset(bv_solver_t *solver) {
  reset_bv_vartable(&solver->vtbl);
  reset_bv_atomtable(&solver->atbl);
  reset_mtbl(&solver->mtbl);
  reset_bv_bound_queue(&solver->bqueue);

  if (solver->blaster != NULL) {
    delete_bit_blaster(solver->blaster);
    safe_free(solver->blaster);
    solver->blaster = NULL;
  }

  if (solver->remap != NULL) {
    delete_remap_table(solver->remap);
    safe_free(solver->remap);
    solver->remap = NULL;
  }

  reset_eassertion_queue(&solver->egraph_queue);

  reset_bv_trail(&solver->trail_stack);

  reset_bvpoly_buffer(&solver->buffer, 32);
  pp_buffer_reset(&solver->prod_buffer);
  ivector_reset(&solver->aux_vector);
  reset_bv_interval_stack(&solver->intv_stack);
  ivector_reset(&solver->a_vector);
  ivector_reset(&solver->b_vector);

  reset_used_vals(&solver->used_vals);

  solver->base_level = 0;
  solver->decision_level = 0;
}




/********************************
 *  INTERNALIZATION FUNCTIONS   *
 *******************************/

/*
 * TERM CONSTRUCTORS
 */

/*
 * Create a new variable of n bits
 */
thvar_t bv_solver_create_var(bv_solver_t *solver, uint32_t n) {
  assert(n > 0);
  return make_bvvar(&solver->vtbl, n);
}


/*
 * Create a variable equal to constant c
 */
thvar_t bv_solver_create_const(bv_solver_t *solver, bvconst_term_t *c) {
  return get_bvconst(&solver->vtbl, c->bitsize, c->data);
}


thvar_t bv_solver_create_const64(bv_solver_t *solver, bvconst64_term_t *c) {
  return get_bvconst64(&solver->vtbl, c->bitsize, c->value);
}


/*
 * Internalize a polynomial p:
 * - map = variable renaming
 *   if p is of the form a_0 t_0 + ... + a_n t_n
 *   then map containts n+1 variables, and map[i] is the internalization of t_i
 * - exception: if t_0 is const_idx then map[0] = null_thvar
 */
thvar_t bv_solver_create_bvpoly(bv_solver_t *solver, bvpoly_t *p, thvar_t *map) {
  bvpoly_buffer_t *buffer;
  uint32_t i, n, nbits;

  n = p->nterms;
  nbits = p->bitsize;
  i = 0;

  buffer = &solver->buffer;
  reset_bvpoly_buffer(buffer, nbits);

  // deal with constant term if any
  if (p->mono[0].var == const_idx) {
    assert(map[0] == null_thvar);
    bvpoly_buffer_add_constant(buffer, p->mono[i].coeff);
    i ++;
  }

  // rest of p
  while (i < n) {
    assert(valid_bvvar(&solver->vtbl, map[i]));
    bvbuffer_add_mono(solver, buffer, map[i], p->mono[i].coeff);
    i ++;
  }

  normalize_bvpoly_buffer(buffer);
  return map_bvpoly(solver, buffer);
}

// Same thing: coefficients stored as 64bit integers
thvar_t bv_solver_create_bvpoly64(bv_solver_t *solver, bvpoly64_t *p, thvar_t *map) {
  bvpoly_buffer_t *buffer;
  uint32_t i, n, nbits;

  n = p->nterms;
  nbits = p->bitsize;
  i = 0;

  buffer = &solver->buffer;
  reset_bvpoly_buffer(buffer, nbits);

  // deal with constant term if any
  if (p->mono[0].var == const_idx) {
    assert(map[0] == null_thvar);
    bvpoly_buffer_add_const64(buffer, p->mono[i].coeff);
    i ++;
  }

  // rest of p
  while (i < n) {
    assert(valid_bvvar(&solver->vtbl, map[i]));
    bvbuffer_add_mono64(solver, buffer, map[i], p->mono[i].coeff);
    i ++;
  }

  normalize_bvpoly_buffer(buffer);
  return map_bvpoly64(solver, buffer);
}



/*
 * Product p = t_0^d_0 x ... x t_n ^d_n
 * - map = variable renaming: t_i is replaced by map[i]
 */
thvar_t bv_solver_create_pprod(bv_solver_t *solver, pprod_t *p, thvar_t *map) {
  bv_vartable_t *vtbl;
  pp_buffer_t *buffer;
  uint32_t *a;
  uint64_t c;
  uint32_t i, n, nbits, w;
  thvar_t x;

  vtbl = &solver->vtbl;
  buffer = &solver->prod_buffer;
  pp_buffer_reset(buffer);

  assert(p->len > 0);
  nbits = bvvar_bitsize(vtbl, map[0]);

  /*
   * We build a term (* constant (x_1 ^ d_1 ... x_k^d_k))
   * by replacing any constant map[i] by its value
   */
  if (nbits <= 64) {
    c = 1;
    n = p->len;
    for (i=0; i<n; i++) {
      x = map[i];
      if (bvvar_is_const64(vtbl, x)) {
	c *= upower64(bvvar_val64(vtbl, x), p->prod[i].exp);
      } else {
	pp_buffer_mul_varexp(buffer, x, p->prod[i].exp);
      }
    }

    // normalize c then build the term (c * p)
    c = norm64(c, nbits);
    x = map_const64_times_product(solver, nbits, buffer, c);

  } else {
    // use aux1 to store the constant
    w = (nbits + 31) >> 5;
    bvconstant_set_bitsize(&solver->aux1, nbits);
    a = solver->aux1.data;
    bvconst_set_one(a, w);

    n = p->len;
    for (i=0; i<n; i++) {
      x = map[i];
      if (bvvar_is_const(vtbl, x)) {
	bvconst_mulpower(a, w, bvvar_val(vtbl, x), p->prod[i].exp);
      } else {
	pp_buffer_mul_varexp(buffer, x, p->prod[i].exp);
      }
    }

    // normalize a then build the term (a * p)
    bvconst_normalize(a, nbits);
    x = map_const_times_product(solver, nbits, buffer, a);
  }

  return x;
}


/*
 * Internalize the bit array a[0 ... n-1]
 * - each element a[i] is a literal in the core
 */
thvar_t bv_solver_create_bvarray(bv_solver_t *solver, literal_t *a, uint32_t n) {
  bvconstant_t *aux;
  uint64_t c;
  thvar_t x;

  if (bvarray_is_constant(a, n)) {
    if (n <= 64) {
      c = bvarray_to_uint64(a, n);
      assert(c == norm64(c, n));
      x = get_bvconst64(&solver->vtbl, n, c);
    } else {
      aux = &solver->aux1;
      bvarray_to_bvconstant(a, n, aux);
      x = get_bvconst(&solver->vtbl, n, aux->data);
    }
  } else {
    x = get_bvarray(&solver->vtbl, n, a);
  }

  return x;
}



/*
 * Internalization of (ite c x y)
 */
thvar_t bv_solver_create_ite(bv_solver_t *solver, literal_t c, thvar_t x, thvar_t y) {
  uint32_t n;
  thvar_t aux;

  n = bvvar_bitsize(&solver->vtbl, x);
  assert(bvvar_bitsize(&solver->vtbl, y) == n);

  /*
   * Normalize: rewrite ((ite (not b) x y) to (ite b y x)
   */
  if (is_neg(c)) {
    aux = x; x = y; y = aux;
    c = not(c);
  }

  assert(c != false_literal);
    
  if (c == true_literal) {
    return x; 
  } else {
    return get_bvite(&solver->vtbl, n, c, x, y);
  }
}


/*
 * Binary operators
 */

/*
 * Quotient x/y: unsigned, rounding toward 0
 * - simplify if x and y are both constants
 */
thvar_t bv_solver_create_bvdiv(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag;
  uint64_t c;
  uint32_t n;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(n == bvvar_bitsize(vtbl, y));

  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_udiv2z(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }

    if (xtag == BVTAG_CONST) {
      // large constants
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_udiv2z(aux->data, n, bvvar_val(vtbl, x), bvvar_val(vtbl, y));
      bvconstant_normalize(aux);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  // no simplification
  return get_bvdiv(vtbl, n, x, y);
}


/*
 * Remainder of x/y: unsigned division, rounding toward 0
 */
thvar_t bv_solver_create_bvrem(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag;
  uint64_t c;
  uint32_t n;
  thvar_t r;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(n == bvvar_bitsize(vtbl, y));

  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_urem2z(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }

    if (xtag == BVTAG_CONST) {
      // large constants
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_urem2z(aux->data, n, bvvar_val(vtbl, x), bvvar_val(vtbl, y));
      bvconstant_normalize(aux);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  // no simplification
  r = get_bvrem(&solver->vtbl, n, x, y);
  assert_urem_bounds(solver, r, y);  // EXPERIMENTAL

  return r;
}


/*
 * Quotient x/y: signed division, rounding toward 0
 */
thvar_t bv_solver_create_bvsdiv(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag;
  uint64_t c;
  uint32_t n;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(n == bvvar_bitsize(vtbl, y));
  
  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_sdiv2z(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }

    if (xtag == BVTAG_CONST) {
      // large constants
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_sdiv2z(aux->data, n, bvvar_val(vtbl, x), bvvar_val(vtbl, y));
      bvconstant_normalize(aux);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  // no simplification
  return get_bvsdiv(&solver->vtbl, n, x, y);
}


/*
 * Remainder of x/y: signed division, rounding toward 0
 */
thvar_t bv_solver_create_bvsrem(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag;
  uint64_t c;
  uint32_t n;
  thvar_t r;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(n == bvvar_bitsize(vtbl, y));
  
  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_srem2z(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }

    if (xtag == BVTAG_CONST) {
      // large constants
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_srem2z(aux->data, n, bvvar_val(vtbl, x), bvvar_val(vtbl, y));
      bvconstant_normalize(aux);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  // no simplification
  r = get_bvsrem(&solver->vtbl, n, x, y);
  assert_srem_bounds(solver, r, y);

  return r;
}


/*
 * Remainder in x/y: signed division, rounding toward -infinity
 */
thvar_t bv_solver_create_bvsmod(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag;
  uint64_t c;
  uint32_t n;
  thvar_t r;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(n == bvvar_bitsize(vtbl, y));

  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_smod2z(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }

    if (xtag == BVTAG_CONST) {
      // large constants
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_smod2z(aux->data, n, bvvar_val(vtbl, x), bvvar_val(vtbl, y));
      bvconstant_normalize(aux);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  // no simplification
  r = get_bvsmod(&solver->vtbl, n, x, y);
  assert_srem_bounds(solver, r, y);

  return r; 
}


/*
 * Left shift, padding with zeros
 */
thvar_t bv_solver_create_bvshl(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag; 
  uint32_t n;
  uint64_t c;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(bvvar_bitsize(vtbl, y) == n);

  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_lshl(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }
    
    if (xtag == BVTAG_CONST) {
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_lshl(aux->data, bvvar_val(vtbl, x), bvvar_val(vtbl, y), n);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  if (bvvar_is_zero(vtbl, x)) {
    // 0b000..0 unchanged by logical shift
    return x;
  }
  
  return get_bvshl(vtbl, n, x, y);
}


/*
 * Right shift, padding with zeros
 */
thvar_t bv_solver_create_bvlshr(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag; 
  uint32_t n;
  uint64_t c;


  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(bvvar_bitsize(vtbl, y) == n);

  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_lshr(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }
   
    if (xtag == BVTAG_CONST) {
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_lshr(aux->data, bvvar_val(vtbl, x), bvvar_val(vtbl, y), n);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  if (bvvar_is_zero(vtbl, x)) {
    // 0b000..0 unchanged by logical shift
    return x;
  }
  
  return get_bvlshr(vtbl, n, x, y);
}


/*
 * Arithmetic right shift
 */
thvar_t bv_solver_create_bvashr(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_vartable_t *vtbl;
  bvconstant_t *aux;
  bvvar_tag_t xtag, ytag; 
  uint32_t n;
  uint64_t c;

  vtbl = &solver->vtbl;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  n = bvvar_bitsize(vtbl, x);
  assert(bvvar_bitsize(vtbl, y) == n);

  xtag = bvvar_tag(vtbl, x);
  ytag = bvvar_tag(vtbl, y);

  // deal with constants
  if (xtag == ytag) {
    if (xtag == BVTAG_CONST64) {
      // small constants
      assert(n <= 64);
      c = bvconst64_ashr(bvvar_val64(vtbl, x), bvvar_val64(vtbl, y), n);
      return get_bvconst64(vtbl, n, c);
    }

    if (xtag == BVTAG_CONST) {
      assert(n > 64);
      aux = &solver->aux1;
      bvconstant_set_bitsize(aux, n);
      bvconst_ashr(aux->data, bvvar_val(vtbl, x), bvvar_val(vtbl, y), n);
      return get_bvconst(vtbl, n, aux->data);
    }
  }

  if (bvvar_is_zero(vtbl, x) || bvvar_is_minus_one(vtbl, x)) {
    // 0b000..0 and 0b11...1 are unchanged by any arithmetic shift
    return x;
  }
  
  return get_bvashr(vtbl, n, x, y);
}


/*
 * Return the i-th bit of theory variable x as a literal
 */
literal_t bv_solver_select_bit(bv_solver_t *solver, thvar_t x, uint32_t i) {
  bv_vartable_t *vtbl;
  literal_t l;

  assert(valid_bvvar(&solver->vtbl, x) && i < bvvar_bitsize(&solver->vtbl, x));

  // apply substitutions
  x = mtbl_get_root(&solver->mtbl, x);

  vtbl = &solver->vtbl;
  switch (bvvar_tag(vtbl, x)) {
  case BVTAG_CONST64:
    l = bvconst64_get_bit(vtbl, x, i);
    break;

  case BVTAG_CONST:
    l = bvconst_get_bit(vtbl, x, i);
    break;

  case BVTAG_BIT_ARRAY:
    l = bvarray_get_bit(vtbl, x, i);
    break;

  default:
    l = bvvar_get_bit(solver, x, i);
    break;
  }

  return l;
}




/*
 * ATOM CONSTRUCTORS
 */


/*
 * Atom (eq x y): no simplification 
 */
static literal_t bv_solver_make_eq_atom(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_atomtable_t *atbl;
  int32_t i;
  literal_t l;
  bvar_t v;

  atbl = &solver->atbl;
  i = get_bveq_atom(atbl, x, y);
  l = atbl->data[i].lit;

  if (l == null_literal) {
    /*
     * New atom: assign a fresh boolean variable for it
     */
    v = create_boolean_variable(solver->core);
    l = pos_lit(v);
    atbl->data[i].lit = l;
    attach_atom_to_bvar(solver->core, v, bvatom_idx2tagged_ptr(i));
  }

  return l;  
}

/*
 * Atom (eq x y): try to simplify
 */
literal_t bv_solver_create_eq_atom(bv_solver_t *solver, thvar_t x, thvar_t y) {
  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  if (equal_bvvar(solver, x, y)) {
    return true_literal;
  }

  if (diseq_bvvar(solver, x, y)) {
    return false_literal;
  }

  simplify_eq(solver, &x, &y);
  return bv_solver_make_eq_atom(solver, x, y);
}



/*
 * Create (bvge x y): no simplification
 */
static literal_t bv_solver_make_ge_atom(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_atomtable_t *atbl;
  int32_t i;
  literal_t l;
  bvar_t v;

  atbl = &solver->atbl;
  i = get_bvuge_atom(atbl, x, y);
  l = atbl->data[i].lit;
  if (l == null_literal) {
    /*
     * New atom: assign a fresh boolean variable for it
     */
    v = create_boolean_variable(solver->core);
    l = pos_lit(v);
    atbl->data[i].lit = l;
    attach_atom_to_bvar(solver->core, v, bvatom_idx2tagged_ptr(i));
  }
 
  return l;
}


/*
 * Atom (bvge x y) (unsigned comparison)
 */
literal_t bv_solver_create_ge_atom(bv_solver_t *solver, thvar_t x, thvar_t y) {
  literal_t l;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  /*
   * Rewrite rules:
   * (bvge 0b000...0 y)  <-->  (bveq 0b000...0 y)
   * (bvge x 0b111...1)  <-->  (bveq x 0b111...1)
   */
  if (bvvar_is_zero(&solver->vtbl, x) || 
      bvvar_is_minus_one(&solver->vtbl, y)) {
    return bv_solver_create_eq_atom(solver, x, y);
  }

  switch (check_bvuge(solver, x, y)) {
  case BVTEST_FALSE:
    l = false_literal;
    break;

  case BVTEST_TRUE:
    l = true_literal;
    break;

  default:
    l = bv_solver_make_ge_atom(solver, x, y);
    break;
  }

  return l;
}


/*
 * Create (bvsge x y): no simplification
 */
static literal_t bv_solver_make_sge_atom(bv_solver_t *solver, thvar_t x, thvar_t y) {
  bv_atomtable_t *atbl;
  int32_t i;
  literal_t l;
  bvar_t v;

  atbl = &solver->atbl;
  i = get_bvsge_atom(atbl, x, y);
  l = atbl->data[i].lit;
  if (l == null_literal) {
    /*
     * New atom: assign a fresh boolean variable for it
     */
    v = create_boolean_variable(solver->core);
    l = pos_lit(v);
    atbl->data[i].lit = l;
    attach_atom_to_bvar(solver->core, v, bvatom_idx2tagged_ptr(i));
  }
 
  return l;
}


/*
 * Atom (bvsge x y) (signed comparison)
 */
literal_t bv_solver_create_sge_atom(bv_solver_t *solver, thvar_t x, thvar_t y) {
  literal_t l;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);
  
  /*
   * Rewrite rules:
   * (bvsge 0b100...0 y)  <-->  (bveq 0b100...0 y)
   * (bvsge x 0b011...1)  <-->  (bveq x 0b011...1)
   */
  if (bvvar_is_min_signed(&solver->vtbl, x) || 
      bvvar_is_max_signed(&solver->vtbl, y)) {
    return bv_solver_create_eq_atom(solver, x, y);
  }

  switch (check_bvsge(solver, x, y)) {
  case BVTEST_FALSE:
    l = false_literal;
    break;

  case BVTEST_TRUE:
    l = true_literal;
    break;

  default:
    l = bv_solver_make_sge_atom(solver, x, y);
    break;
  }

  return l;
}



/*
 * ATOM ASSERTIONS
 */

/*
 * Assert (x != y) where y is the constant 0b0000..0
 * - this special case is handled separately since we want to add the
 *   constraint (x != y) to the bound queue (unless it's already there).
 */
static void bv_solver_assert_neq0(bv_solver_t *solver, thvar_t x, thvar_t y) {  
  bv_atomtable_t *atbl;
  int32_t i;
  literal_t l;
  bvar_t v;

  assert(bvvar_is_zero(&solver->vtbl, y));
  atbl = &solver->atbl;
  i = get_bveq_atom(atbl, x, y);
  l = atbl->data[i].lit;
  if (l == null_literal) {
    /*
     * New atom: (x != 0) can't be in the bound queue
     */
    v = create_boolean_variable(solver->core);
    l = pos_lit(v);
    atbl->data[i].lit = l;
    attach_atom_to_bvar(solver->core, v, bvatom_idx2tagged_ptr(i));
    push_bvdiseq_bound(solver, x, y);

  } else if (! bvvar_is_nonzero(solver, x)) {
    /*
     * The bound (x != 0) is not in the queue yet: add it
     */
    push_bvdiseq_bound(solver, x, y);
  }

  add_unit_clause(solver->core, not(l));
}


/*
 * Assert (x == y) if tt is true
 * assert (x != y) if tt is false
 */
void bv_solver_assert_eq_axiom(bv_solver_t *solver, thvar_t x, thvar_t y, bool tt) {
  literal_t l;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  if (equal_bvvar(solver, x, y)) {
    if (! tt) add_empty_clause(solver->core);     // Contradiction
  } else if (diseq_bvvar(solver, x, y)) {
    if (tt) add_empty_clause(solver->core);       // Contradiction
  } else {
    simplify_eq(solver, &x, &y);  // try to simplify
    if (tt) {
      // x == y: merge the classes of x and y
      bv_solver_merge_vars(solver, x, y);
    } else if (bvvar_is_zero(&solver->vtbl, x)) {
      // y != 0
      bv_solver_assert_neq0(solver, y, x); 
    } else if (bvvar_is_zero(&solver->vtbl, y)) {
      // x != 0
      bv_solver_assert_neq0(solver, x, y);
    } else {
      // Add the constraint (x != y)
      l = bv_solver_make_eq_atom(solver, x, y);
      add_unit_clause(solver->core, not(l));
    }
  }
}


/*
 * Assert (bvge x y) if tt is true
 * Assert (not (bvge x y)) if tt is false
 */
void bv_solver_assert_ge_axiom(bv_solver_t *solver, thvar_t x, thvar_t y, bool tt) {
  literal_t l;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  /*
   * Rewrite rules:
   * (bvge 0b000...0 y)  <-->  (bveq 0b000...0 y)
   * (bvge x 0b111...1)  <-->  (bveq x 0b111...1)
   */
  if (bvvar_is_zero(&solver->vtbl, x) || 
      bvvar_is_minus_one(&solver->vtbl, y)) {
    bv_solver_assert_eq_axiom(solver, x, y, tt);

  } else {
    switch (check_bvuge(solver, x, y)) {
    case BVTEST_FALSE:
      if (tt) add_empty_clause(solver->core); // x < y holds
      break;

    case BVTEST_TRUE:
      if (!tt) add_empty_clause(solver->core); // x >= y holds
      break;

    case BVTEST_UNKNOWN:
      l = bv_solver_make_ge_atom(solver, x, y);
      add_unit_clause(solver->core, signed_literal(l, tt));
      // push the bound into the queue
      if (is_bv_bound_pair(&solver->vtbl, x, y)) {
	push_bvuge_bound(solver, x, y);
      }
      break;
    }
  }
}


/*
 * Assert (bvsge x y) if tt is true
 * Assert (not (bvsge x y)) if tt is false
 */
void bv_solver_assert_sge_axiom(bv_solver_t *solver, thvar_t x, thvar_t y, bool tt) {
  literal_t l;

  x = mtbl_get_root(&solver->mtbl, x);
  y = mtbl_get_root(&solver->mtbl, y);

  /*
   * Rewrite rules:
   * (bvsge 0b100...0 y)  <-->  (bveq 0b100...0 y)
   * (bvsge x 0b011...1)  <-->  (bveq x 0b011...1)
   */
  if (bvvar_is_min_signed(&solver->vtbl, x) || 
      bvvar_is_max_signed(&solver->vtbl, y)) {
    bv_solver_assert_eq_axiom(solver, x, y, tt);

  } else {
    switch (check_bvsge(solver, x, y)) {
    case BVTEST_FALSE:
      if (tt) add_empty_clause(solver->core); // x < y holds
      break;

    case BVTEST_TRUE:
      if (!tt) add_empty_clause(solver->core); // x >= y holds
      break;

    case BVTEST_UNKNOWN:
      l = bv_solver_make_sge_atom(solver, x, y);
      add_unit_clause(solver->core, signed_literal(l, tt));
      // push the bound into the queue
      if (is_bv_bound_pair(&solver->vtbl, x, y)) {
	push_bvsge_bound(solver, x, y);
      }
      break;
    }
  }
}


/*
 * Assert that bit i of x is equal to tt
 */
void bv_solver_set_bit(bv_solver_t *solver, thvar_t x, uint32_t i, bool tt) {
  literal_t l;

  l = bv_solver_select_bit(solver, x, i);
  add_unit_clause(solver->core, signed_literal(l, tt));
}



/*
 * EGRAPH TERMS
 */

/*
 * Attach egraph term t to variable x
 */
void bv_solver_attach_eterm(bv_solver_t *solver, thvar_t x, eterm_t t) {
  attach_eterm_to_bvvar(&solver->vtbl, x, t);
}


/*
 * Get the egraph term attached to x
 * - return null_eterm if x has no eterm attached
 */
eterm_t bv_solver_eterm_of_var(bv_solver_t *solver, thvar_t x) {
  return bvvar_get_eterm(&solver->vtbl, x);
}



/******************************
 *  NUMBER OF ATOMS PER TYPE  *
 *****************************/

uint32_t bv_solver_num_eq_atoms(bv_solver_t *solver) {
  bv_atomtable_t *atbl;
  uint32_t i, n, c;

  c = 0;
  atbl = &solver->atbl;
  n = atbl->natoms;
  for (i=0; i<n; i++) {
    if (bvatm_is_eq(atbl->data + i)) {
      c ++;
    }
  }

  return c;
}

uint32_t bv_solver_num_ge_atoms(bv_solver_t *solver) {
  bv_atomtable_t *atbl;
  uint32_t i, n, c;

  c = 0;
  atbl = &solver->atbl;
  n = atbl->natoms;
  for (i=0; i<n; i++) {
    if (bvatm_is_ge(atbl->data + i)) {
      c ++;
    }
  }

  return c;
}


uint32_t bv_solver_num_sge_atoms(bv_solver_t *solver) {
  bv_atomtable_t *atbl;
  uint32_t i, n, c;

  c = 0;
  atbl = &solver->atbl;
  n = atbl->natoms;
  for (i=0; i<n; i++) {
    if (bvatm_is_sge(atbl->data + i)) {
      c ++;
    }
  }

  return c;
}





/*******************************
 *  INTERNALIZATION INTERFACE  *
 ******************************/

static bv_interface_t bv_solver_context = {
  (create_bv_var_fun_t) bv_solver_create_var,
  (create_bv_const_fun_t) bv_solver_create_const,
  (create_bv64_const_fun_t) bv_solver_create_const64,
  (create_bv_poly_fun_t) bv_solver_create_bvpoly,
  (create_bv64_poly_fun_t) bv_solver_create_bvpoly64,
  (create_bv_pprod_fun_t) bv_solver_create_pprod,
  (create_bv_array_fun_t) bv_solver_create_bvarray,
  (create_bv_ite_fun_t) bv_solver_create_ite,
  (create_bv_binop_fun_t) bv_solver_create_bvdiv,
  (create_bv_binop_fun_t) bv_solver_create_bvrem,
  (create_bv_binop_fun_t) bv_solver_create_bvsdiv,
  (create_bv_binop_fun_t) bv_solver_create_bvsrem,
  (create_bv_binop_fun_t) bv_solver_create_bvsmod,
  (create_bv_binop_fun_t) bv_solver_create_bvshl,
  (create_bv_binop_fun_t) bv_solver_create_bvlshr,
  (create_bv_binop_fun_t) bv_solver_create_bvashr,

  (select_bit_fun_t) bv_solver_select_bit,
  (create_bv_atom_fun_t) bv_solver_create_eq_atom, 
  (create_bv_atom_fun_t) bv_solver_create_ge_atom, 
  (create_bv_atom_fun_t) bv_solver_create_sge_atom, 

  (assert_bv_axiom_fun_t) bv_solver_assert_eq_axiom,
  (assert_bv_axiom_fun_t) bv_solver_assert_ge_axiom,
  (assert_bv_axiom_fun_t) bv_solver_assert_sge_axiom,
  (set_bit_fun_t) bv_solver_set_bit,

  (attach_eterm_fun_t) bv_solver_attach_eterm,
  (eterm_of_var_fun_t) bv_solver_eterm_of_var,

  NULL,
  NULL,
  NULL,  
};


/*
 * Return the interface descriptor
 */
bv_interface_t *bv_solver_bv_interface(bv_solver_t *solver) {
  return &bv_solver_context;
}



/********************************
 *  SMT AND CONTROL INTERFACES  *
 *******************************/

static th_ctrl_interface_t bv_solver_control = {
  (start_intern_fun_t) bv_solver_start_internalization,
  (start_fun_t) bv_solver_start_search,
  (propagate_fun_t) bv_solver_propagate,
  (final_check_fun_t) bv_solver_final_check,
  (increase_level_fun_t) bv_solver_increase_decision_level,
  (backtrack_fun_t) bv_solver_backtrack,
  (push_fun_t) bv_solver_push,
  (pop_fun_t) bv_solver_pop,
  (reset_fun_t) bv_solver_reset,
};

static th_smt_interface_t bv_solver_smt = {
  (assert_fun_t) bv_solver_assert_atom,
  (expand_expl_fun_t) bv_solver_expand_explanation,
  (select_pol_fun_t) bv_solver_select_polarity,
  NULL,
  NULL,
};


/*
 * Get the control and smt interfaces
 */
th_ctrl_interface_t *bv_solver_ctrl_interface(bv_solver_t *solver) {
  return &bv_solver_control;
}

th_smt_interface_t *bv_solver_smt_interface(bv_solver_t *solver) {
  return &bv_solver_smt;
}



/*********************************************
 *  SATELLITE SOLVER INTERFACE (FOR EGRAPH)  *
 ********************************************/

static th_egraph_interface_t bv_solver_egraph = {
  (assert_eq_fun_t) bv_solver_assert_var_eq,
  (assert_diseq_fun_t) bv_solver_assert_var_diseq,
  (assert_distinct_fun_t) bv_solver_assert_var_distinct,
  (check_diseq_fun_t) bv_solver_check_disequality,
  NULL, // no need for expand_th_explanation
  (reconcile_model_fun_t) bv_solver_reconcile_model,
  (attach_to_var_fun_t) bv_solver_attach_eterm,
  (get_eterm_fun_t) bv_solver_eterm_of_var,
  (select_eq_polarity_fun_t) bv_solver_select_eq_polarity,
};


static bv_egraph_interface_t bv_solver_bv_egraph = {
  (make_bv_var_fun_t) bv_solver_create_var,
  (bv_val_fun_t) bv_solver_value_in_model,
  (bv_fresh_val_fun_t) bv_solver_fresh_value,
};


/*
 * Get the egraph interfaces
 */
th_egraph_interface_t *bv_solver_egraph_interface(bv_solver_t *solver) {
  return &bv_solver_egraph;
}

bv_egraph_interface_t *bv_solver_bv_egraph_interface(bv_solver_t *solver) {
  return &bv_solver_bv_egraph;
}




