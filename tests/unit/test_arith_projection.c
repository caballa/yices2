/*
 * The Yices SMT Solver. Copyright 2014 SRI International.
 *
 * This program may only be used subject to the noncommercial end user
 * license agreement which is downloadable along with this program.
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "solvers/exists_forall/arith_projection.h"
#include "api/yices_globals.h"
#include "yices.h"


/*
 * Print variable i (i = index in proj)
 * - this assumes that all variables have a name (in the global term table)
 */
static void print_proj_var(FILE *f, aproj_vtbl_t *vtbl, int32_t i) {
  const char *name;
  term_t x;

  assert(0 < i && i < vtbl->nvars);
  x = vtbl->term_of[i];
  name = yices_get_term_name(x);
  assert(name != NULL);
  fputs(name, f);  
}

/*
 * Print monomial q * i:
 * - first; means first monomial in a constraint
 */
static void print_proj_mono(FILE *f, aproj_vtbl_t *vtbl, rational_t *q, int32_t i, bool first) {
  bool abs_one;

  if (q_is_neg(q)) {
    abs_one = q_is_minus_one(q);
    if (first) {
      fputc('-', f);
      if (i != const_idx) {
	fputc(' ', f);
      }
    } else {
      fputs(" - ", f);
    }
  } else {
    abs_one = q_is_one(q);
    if (! first) {
      fputs(" + ", f);
    }
  }

  if (i == const_idx) {
    q_print_abs(f, q);
  } else {
    if (! abs_one) {
      q_print_abs(f, q);
      fputc('*', f);
    }
    print_proj_var(f, vtbl, i);
  }
}

static void print_proj_poly(FILE *f, aproj_vtbl_t *vtbl, monomial_t *p, uint32_t n) {
  uint32_t i;
  bool first;

  if (n == 0) {
    fputc('0', f);
  } else {
    first = true;
    for (i=0; i<n; i++) {
      print_proj_mono(f, vtbl, &p[i].coeff, p[i].var, first);
      first = false;
    }
  }
}

static void print_proj_constraint(FILE *f, aproj_vtbl_t *vtbl, aproj_constraint_t *c) {
  print_proj_poly(f, vtbl, c->mono, c->nterms);
  switch (c->tag) {
  case APROJ_GT:
    fputs(" > 0", f);
    break;
  case APROJ_GE:
    fputs(" >= 0", f);
    break;
  case APROJ_EQ:
    fputs(" = 0", f);
    break;
  default:
    fprintf(stderr, "BUG: invalid constraint tag (%"PRId32")\n", (int32_t) c->tag);
    exit(1);
    break;
  }
}


/*
 * Print variables of index [k, ..., n-1]
 */
static void show_proj_var_array(FILE *f, aproj_vtbl_t *vtbl, uint32_t k, uint32_t n) {
  assert(0 < k && k <= n && n <= vtbl->nvars);

  if (k == n) {
    fputs("none", f);
  } else {
    fputc('[', f);
    for (;;) {
      print_proj_var(f, vtbl, k);
      k ++;
      if (k == n) break;
      fputc(' ', f);
    }
    fputc(']', f);
  }
}

static void show_proj_vars_to_elim(FILE *f, aproj_vtbl_t *vtbl) {
  show_proj_var_array(f, vtbl, 1, vtbl->nelims);
}

static void show_proj_vars_to_keep(FILE *f, aproj_vtbl_t *vtbl) {
  show_proj_var_array(f, vtbl, vtbl->nelims, vtbl->nvars);
}

/*
 * All constraints in ptr_set s
 */
static void show_constraint_set(FILE *f, aproj_vtbl_t *vtbl, ptr_set_t *s) {
  uint32_t i, n;
  aproj_constraint_t *c;

  if (s == NULL) {
    fputs("none\n", f);
  } else {
    n = s->size;
    for (i=0; i<n; i++) {
      c = s->data[i];
      if (c != NULL && c != DELETED_PTR_ELEM) {
	fputs("\n    ", f);
	print_proj_constraint(f, vtbl, c);
      }
    }
    fputc('\n', f);
  }
}

static void show_projector(FILE *f, arith_projector_t *proj) {
  fprintf(f, "Projector: %p\n", proj);
  fprintf(f, "  vars to elim: ");  
  show_proj_vars_to_elim(f, &proj->vtbl);
  fputc('\n', f);
  fprintf(f, "  vars to keep: ");
  show_proj_vars_to_keep(f, &proj->vtbl);
  fputc('\n', f);
  fprintf(f, "  constraints: ");
  show_constraint_set(f, &proj->vtbl, proj->constraints);
}


/*
 * Print and error and quit if something fails in term construction
 */
static void error_in_yices(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  yices_print_error(stderr);
  fflush(stderr);
  exit(1);
}


/*
 * GLOBAL TABLE: VARIABLES/MODEL
 */

/*
 * Each variable is defined by
 * - a name + a rational value given by a pair of int32: num/den
 */
typedef struct var_desc_s {
  const char *name;
  int32_t num;
  uint32_t den;
} var_desc_t;

#define NUM_VARS 10

static const var_desc_t var_desc[NUM_VARS] = {
  { "a",  0, 1 },
  { "b",  1, 1 },
  { "c", -1, 1 },
  { "d",  0, 1 },
  { "e",  1, 1 },
  { "v", -1, 1 },
  { "w",  1, 2 },
  { "x", -1, 2 },
  { "y",  2, 1 },
  { "z", -2, 3 },
};


/*
 * From the descriptors, we build two arrays:
 * - var[i] = term for variable i
 * - value[i] = rational value for i
 */
static term_t var[NUM_VARS];
static rational_t value[NUM_VARS];

static void init_variables(void) {
  uint32_t i;
  term_t x;
  type_t tau;
  int32_t code;

  tau = yices_real_type();
  for (i=0; i<NUM_VARS; i++) {
    x = yices_new_uninterpreted_term(tau);
    if (x < 0) error_in_yices("variable declaration");
    var[i] = x;
    code = yices_set_term_name(x, var_desc[i].name);
    if (code < 0) error_in_yices("set_term_name");
  }

  for (i=0; i<NUM_VARS; i++) {
    q_init(value + i);
    q_set_int32(value + i, var_desc[i].num, var_desc[i].den);
  }
}

static void cleanup_values(void) {
  uint32_t i;

  for (i=0; i<NUM_VARS; i++) {
    q_clear(value + i);
  }
}


/*
 * Add variables in var[i .. k-1] to proj
 * - to_elim = whether to mark them as eliminate/keep variables
 */
static void test_addvars(arith_projector_t *proj, uint32_t i, uint32_t k, bool to_elim) {
  assert(i <= k && k <= NUM_VARS);

  while (i < k) {
    aproj_add_var(proj, var[i], to_elim, &value[i]);
    i ++;    
  }
}
 

static void test_vars(void) {
  arith_projector_t proj;
  uint32_t n;

  init_arith_projector(&proj, __yices_globals.manager, 4, 2);
  printf("*** After init ***\n");
  show_projector(stdout, &proj);  
  printf("\n");

  n = NUM_VARS;

  test_addvars(&proj, 0, n, false);
  printf("*** After addvar 0/%"PRIu32" ***\n", n);
  show_projector(stdout, &proj);
  printf("\n");

  reset_arith_projector(&proj);
  printf("*** After reset ***\n");
  show_projector(stdout, &proj);  
  printf("\n");

  test_addvars(&proj, 0, n, true);
  printf("*** After addvar %"PRIu32"/0 ***\n", n);
  show_projector(stdout, &proj);
  printf("\n");

  reset_arith_projector(&proj);
  printf("*** After reset ***\n");
  show_projector(stdout, &proj);  
  printf("\n");

  test_addvars(&proj, 5, 10, true);
  test_addvars(&proj, 0, 5, false);
  printf("*** After addvar 5/5 ***\n");
  show_projector(stdout, &proj);
  printf("\n");


  reset_arith_projector(&proj);
  printf("*** After reset ***\n");
  show_projector(stdout, &proj);  
  printf("\n");

  test_addvars(&proj, 5, 7, true);
  test_addvars(&proj, 0, 3, false);
  test_addvars(&proj, 7, 8, true);
  test_addvars(&proj, 3, 5, false);
  test_addvars(&proj, 8, 10, true);
  printf("*** After addvar 5/5 ***\n");
  show_projector(stdout, &proj);
  printf("\n");


  delete_arith_projector(&proj);
}

int main(void) {
  yices_init();
  init_variables();

  test_vars();

  cleanup_values();
  yices_exit();

  return 0;
}