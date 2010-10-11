/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		analyst.c						 *
* Last rev:								 *
* mods:									 *
* comments:	Tracing the abstract machine				 *
*									 *
*************************************************************************/
#ifdef SCCS
static char SccsId[] = "%W% %G%";

#endif

#include "Yap.h"

#ifdef ANALYST
#include "Yatom.h"
#include "yapio.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif

YAP_ULONG_LONG Yap_opcount[_std_top + 1];

YAP_ULONG_LONG Yap_2opcount[_std_top + 1][_std_top + 1];


STATIC_PROTO(Int p_reset_op_counters, (void));
STATIC_PROTO(Int p_show_op_counters, (void));
STATIC_PROTO(Int p_show_ops_by_group, (void));

static Int 
p_reset_op_counters()
{
  int i;

  for (i = 0; i <= _std_top; ++i)
    Yap_opcount[i] = 0;
  return TRUE;
}

static void 
print_instruction(int inst)
{
  int j;

  fprintf(Yap_stderr, "%s", Yap_op_names[inst]);
  for (j = strlen(Yap_op_names[inst]); j < 25; j++)
    putc(' ', Yap_stderr);
  j = Yap_opcount[inst];
  if (j < 100000000) {
    putc(' ', Yap_stderr);
    if (j < 10000000) {
      putc(' ', Yap_stderr);
      if (j < 1000000) {
	putc(' ', Yap_stderr);
	if (j < 100000) {
	  putc(' ', Yap_stderr);
	  if (j < 10000) {
	    putc(' ', Yap_stderr);
	    if (j < 1000) {
	      putc(' ', Yap_stderr);
	      if (j < 100) {
		putc(' ', Yap_stderr);
		if (j < 10) {
		  putc(' ', Yap_stderr);
		}
	      }
	    }
	  }
	}
      }
    }
  }
  fprintf(Yap_stderr, "%llu\n", Yap_opcount[inst]);
}

static Int 
p_show_op_counters()
{
  int i;
  Term t1 = Deref(ARG1);

  if (IsVarTerm(t1) || !IsAtomTerm(t1)) {
    return FALSE;
  } else {
    Atom at1 = AtomOfTerm(t1);

    if (IsWideAtom(at1)) {
      wchar_t *program;

      program = RepAtom(at1)->WStrOfAE;
      fprintf(Yap_stderr, "\n Instructions Executed in %S\n", program);
    } else {
      char *program;

      program = RepAtom(at1)->StrOfAE;
      fprintf(Yap_stderr, "\n Instructions Executed in %s\n", program);
    }
  }

  for (i = 0; i <= _std_top; ++i)
    print_instruction(i);
  fprintf(Yap_stderr, "\n Control Instructions \n");
  print_instruction(_op_fail);
  print_instruction(_execute);
  print_instruction(_dexecute);
  print_instruction(_call);
  print_instruction(_fcall);
  print_instruction(_call_cpred);
  print_instruction(_call_c_wfail);
  print_instruction(_procceed);
  print_instruction(_safe_procceed);
  print_instruction(_allocate);
  print_instruction(_deallocate);

  fprintf(Yap_stderr, "\n Choice Point Manipulation Instructions\n");
  print_instruction(_try_me);
  print_instruction(_retry_me);
  print_instruction(_trust_me);
  print_instruction(_try_clause);
  print_instruction(_try_in);
  print_instruction(_retry);
  print_instruction(_trust);

  fprintf(Yap_stderr, "\n Disjunction Instructions\n");
  print_instruction(_either);
  print_instruction(_or_else);
  print_instruction(_or_last);
  print_instruction(_jump);
  print_instruction(_move_back);

  fprintf(Yap_stderr, "\n Dynamic Predicates Choicepoint Instructions\n");
  print_instruction(_try_and_mark);
  print_instruction(_retry_and_mark);

  fprintf(Yap_stderr, "\n C Predicates Choicepoint Instructions\n");
  print_instruction(_try_c);
  print_instruction(_retry_c);

  fprintf(Yap_stderr, "\n Indexing Instructions\n");
  fprintf(Yap_stderr, "\n  Switch on Type\n");
  print_instruction(_switch_on_type);
  print_instruction(_switch_list_nl);
  print_instruction(_switch_on_arg_type);
  print_instruction(_switch_on_sub_arg_type);
  fprintf(Yap_stderr, "\n  Switch on Value\n");
  print_instruction(_if_cons);
  print_instruction(_go_on_cons);
  print_instruction(_switch_on_cons);
  print_instruction(_if_func);
  print_instruction(_go_on_func);
  print_instruction(_switch_on_func);
  fprintf(Yap_stderr, "\n  Other Switches\n");
  print_instruction(_if_not_then);

  fprintf(Yap_stderr, "\n Get Instructions\n");
  print_instruction(_get_x_var);
  print_instruction(_get_y_var);
  print_instruction(_get_x_val);
  print_instruction(_get_y_val);
  print_instruction(_get_atom);
  print_instruction(_get_2atoms);
  print_instruction(_get_3atoms);
  print_instruction(_get_4atoms);
  print_instruction(_get_5atoms);
  print_instruction(_get_6atoms);
  print_instruction(_get_list);
  print_instruction(_get_struct);
  fprintf(Yap_stderr, "\n   Optimised Get Instructions\n");
  print_instruction(_glist_valx);
  print_instruction(_glist_valy);
  print_instruction(_gl_void_varx);
  print_instruction(_gl_void_vary);
  print_instruction(_gl_void_valx);
  print_instruction(_gl_void_valy);

  fprintf(Yap_stderr, "\n Unify Read Instructions\n");
  print_instruction(_unify_x_var);
  print_instruction(_unify_x_var2);
  print_instruction(_unify_y_var);
  print_instruction(_unify_x_val);
  print_instruction(_unify_y_val);
  print_instruction(_unify_x_loc);
  print_instruction(_unify_y_loc);
  print_instruction(_unify_atom);
  print_instruction(_unify_n_atoms);
  print_instruction(_unify_n_voids);
  print_instruction(_unify_list);
  print_instruction(_unify_struct);
  fprintf(Yap_stderr, "\n   Unify Last Read Instructions\n");
  print_instruction(_unify_l_x_var);
  print_instruction(_unify_l_x_var2);
  print_instruction(_unify_l_y_var);
  print_instruction(_unify_l_x_val);
  print_instruction(_unify_l_y_val);
  print_instruction(_unify_l_x_loc);
  print_instruction(_unify_l_y_loc);
  print_instruction(_unify_l_atom);
  print_instruction(_unify_l_n_voids);
  print_instruction(_unify_l_list);
  print_instruction(_unify_l_struc);

  fprintf(Yap_stderr, "\n Unify Write Instructions\n");
  print_instruction(_unify_x_var_write);
  print_instruction(_unify_x_var2_write);
  print_instruction(_unify_y_var_write);
  print_instruction(_unify_x_val_write);
  print_instruction(_unify_y_val_write);
  print_instruction(_unify_x_loc_write);
  print_instruction(_unify_y_loc_write);
  print_instruction(_unify_atom_write);
  print_instruction(_unify_n_atoms_write);
  print_instruction(_unify_n_voids_write);
  print_instruction(_unify_list_write);
  print_instruction(_unify_struct_write);
  fprintf(Yap_stderr, "\n   Unify Last Read Instructions\n");
  print_instruction(_unify_l_x_var_write);
  print_instruction(_unify_l_x_var2_write);
  print_instruction(_unify_l_y_var_write);
  print_instruction(_unify_l_x_val_write);
  print_instruction(_unify_l_y_val_write);
  print_instruction(_unify_l_x_loc_write);
  print_instruction(_unify_l_y_loc_write);
  print_instruction(_unify_l_atom_write);
  print_instruction(_unify_l_n_voids_write);
  print_instruction(_unify_l_list_write);
  print_instruction(_unify_l_struc_write);

  fprintf(Yap_stderr, "\n Put Instructions\n");
  print_instruction(_put_x_var);
  print_instruction(_put_y_var);
  print_instruction(_put_x_val);
  print_instruction(_put_xx_val);
  print_instruction(_put_y_val);
  print_instruction(_put_unsafe);
  print_instruction(_put_atom);
  print_instruction(_put_list);
  print_instruction(_put_struct);

  fprintf(Yap_stderr, "\n Write Instructions\n");
  print_instruction(_write_x_var);
  print_instruction(_write_y_var);
  print_instruction(_write_x_val);
  print_instruction(_write_y_val);
  print_instruction(_write_x_loc);
  print_instruction(_write_y_loc);
  print_instruction(_write_atom);
  print_instruction(_write_n_atoms);
  print_instruction(_write_n_voids);
  print_instruction(_write_list);
  print_instruction(_write_struct);
  fprintf(Yap_stderr, "\n   Last Write Instructions\n");
  print_instruction(_write_l_list);
  print_instruction(_write_l_struc);

  fprintf(Yap_stderr, "\n Miscellaneous Instructions\n");
  print_instruction(_cut);
  print_instruction(_cut_t);
  print_instruction(_cut_e);
  print_instruction(_skip);
  print_instruction(_pop);
  print_instruction(_pop_n);
  print_instruction(_trust_fail);
  print_instruction(_index_pred);
  print_instruction(_lock_pred);
#if THREADS
  print_instruction(_thread_local);
#endif
  print_instruction(_save_b_x);
  print_instruction(_save_b_y);
  print_instruction(_save_pair_x);
  print_instruction(_save_pair_y);
  print_instruction(_save_pair_x_write);
  print_instruction(_save_pair_y_write);
  print_instruction(_save_appl_x);
  print_instruction(_save_appl_y);
  print_instruction(_save_appl_x_write);
  print_instruction(_save_appl_y_write);
  print_instruction(_Ystop);
  print_instruction(_Nstop);

  return TRUE;
}

typedef struct {
  int nxvar, nxval, nyvar, nyval, ncons, nlist, nstru, nmisc;
} uYap_opcount;

typedef struct {
  int ncalls, nexecs, nproceeds, ncallbips, ncuts, nallocs, ndeallocs;
} cYap_opcount;

typedef struct {
  int ntries, nretries, ntrusts;
} ccpcount;

static Int 
p_show_ops_by_group(void)
{

  uYap_opcount c_get, c_unify, c_put, c_write;
  cYap_opcount c_control;
  ccpcount c_cp;
  int gets, unifies, puts, writes, controls, choice_pts, indexes, misc,
    total;
  Term t1;
  Atom at1;

  t1 = Deref(ARG1);
  if (IsVarTerm(t1) || !IsAtomTerm(t1))
    return (FALSE);
  at1 = AtomOfTerm(t1);
  if (IsWideAtom(at1)) {
    wchar_t *program;

    program = RepAtom(at1)->WStrOfAE;
    fprintf(Yap_stderr, "\n Instructions Executed in %S\n", program);
  } else {
    char *program;

    program = RepAtom(at1)->StrOfAE;
    fprintf(Yap_stderr, "\n Instructions Executed in %s\n", program);
  }

  c_get.nxvar =
    Yap_opcount[_get_x_var];
  c_get.nyvar =
    Yap_opcount[_get_y_var];
  c_get.nxval =
    Yap_opcount[_get_x_val];
  c_get.nyval =
    Yap_opcount[_get_y_val];
  c_get.ncons =
    Yap_opcount[_get_atom]+
    Yap_opcount[_get_2atoms]+
    Yap_opcount[_get_3atoms]+
    Yap_opcount[_get_4atoms]+
    Yap_opcount[_get_5atoms]+
    Yap_opcount[_get_6atoms];
  c_get.nlist =
    Yap_opcount[_get_list] +
    Yap_opcount[_glist_valx] +
    Yap_opcount[_glist_valy] +
    Yap_opcount[_gl_void_varx] +
    Yap_opcount[_gl_void_vary] +
    Yap_opcount[_gl_void_valx] +
    Yap_opcount[_gl_void_valy];
  c_get.nstru =
    Yap_opcount[_get_struct];

  gets = c_get.nxvar + c_get.nyvar + c_get.nxval + c_get.nyval +
    c_get.ncons + c_get.nlist + c_get.nstru;

  c_unify.nxvar =
    Yap_opcount[_unify_x_var] +
    Yap_opcount[_unify_void] +
    Yap_opcount[_unify_n_voids] +
    2 * Yap_opcount[_unify_x_var2] +
    2 * Yap_opcount[_gl_void_varx] +
    Yap_opcount[_gl_void_vary] +
    Yap_opcount[_gl_void_valx] +
    Yap_opcount[_unify_l_x_var] +
    Yap_opcount[_unify_l_void] +
    Yap_opcount[_unify_l_n_voids] +
    2 * Yap_opcount[_unify_l_x_var2] +
    Yap_opcount[_unify_x_var_write] +
    Yap_opcount[_unify_void_write] +
    Yap_opcount[_unify_n_voids_write] +
    2 * Yap_opcount[_unify_x_var2_write] +
    Yap_opcount[_unify_l_x_var_write] +
    Yap_opcount[_unify_l_void_write] +
    Yap_opcount[_unify_l_n_voids_write] +
    2 * Yap_opcount[_unify_l_x_var2_write];
  c_unify.nyvar =
    Yap_opcount[_unify_y_var] +
    Yap_opcount[_gl_void_vary] +
    Yap_opcount[_unify_l_y_var] +
    Yap_opcount[_unify_y_var_write] +
    Yap_opcount[_unify_l_y_var_write];
  c_unify.nxval =
    Yap_opcount[_unify_x_val] +
    Yap_opcount[_unify_x_loc] +
    Yap_opcount[_glist_valx] +
    Yap_opcount[_gl_void_valx] +
    Yap_opcount[_unify_l_x_val] +
    Yap_opcount[_unify_l_x_loc] +
    Yap_opcount[_unify_x_val_write] +
    Yap_opcount[_unify_x_loc_write] +
    Yap_opcount[_unify_l_x_val_write] +
    Yap_opcount[_unify_l_x_loc_write];
  c_unify.nyval =
    Yap_opcount[_unify_y_val] +
    Yap_opcount[_unify_y_loc] +
    Yap_opcount[_glist_valy] +
    Yap_opcount[_gl_void_valy] +
    Yap_opcount[_unify_l_y_val] +
    Yap_opcount[_unify_l_y_loc] +
    Yap_opcount[_unify_y_val_write] +
    Yap_opcount[_unify_y_loc_write] +
    Yap_opcount[_unify_l_y_val_write] +
    Yap_opcount[_unify_l_y_loc_write];
  c_unify.ncons =
    Yap_opcount[_unify_atom] +
    Yap_opcount[_unify_n_atoms] +
    Yap_opcount[_unify_l_atom] +
    Yap_opcount[_unify_atom_write] +
    Yap_opcount[_unify_n_atoms_write] +
    Yap_opcount[_unify_l_atom_write];
  c_unify.nlist =
    Yap_opcount[_unify_list] +
    Yap_opcount[_unify_l_list] +
    Yap_opcount[_unify_list_write] +
    Yap_opcount[_unify_l_list_write];
  c_unify.nstru =
    Yap_opcount[_unify_struct] +
    Yap_opcount[_unify_l_struc] +
    Yap_opcount[_unify_struct_write] +
    Yap_opcount[_unify_l_struc_write];
  c_unify.nmisc =
    Yap_opcount[_pop] +
    Yap_opcount[_pop_n];

  unifies = c_unify.nxvar + c_unify.nyvar + c_unify.nxval + c_unify.nyval +
    c_unify.ncons + c_unify.nlist + c_unify.nstru + c_unify.nmisc;

  c_put.nxvar =
    Yap_opcount[_put_x_var];
  c_put.nyvar =
    Yap_opcount[_put_y_var];
  c_put.nxval =
    Yap_opcount[_put_x_val]+
    2*Yap_opcount[_put_xx_val];
  c_put.nyval =
    Yap_opcount[_put_y_val];
  c_put.ncons =
    Yap_opcount[_put_atom];
  c_put.nlist =
    Yap_opcount[_put_list];
  c_put.nstru =
    Yap_opcount[_put_struct];

  puts = c_put.nxvar + c_put.nyvar + c_put.nxval + c_put.nyval +
    c_put.ncons + c_put.nlist + c_put.nstru;

  c_write.nxvar =
    Yap_opcount[_write_x_var] +
    Yap_opcount[_write_void] +
    Yap_opcount[_write_n_voids];
  c_write.nyvar =
    Yap_opcount[_write_y_var];
  c_write.nxval =
    Yap_opcount[_write_x_val];
  c_write.nyval =
    Yap_opcount[_write_y_val];
  c_write.ncons =
    Yap_opcount[_write_atom];
  c_write.nlist =
    Yap_opcount[_write_list];
  c_write.nstru =
    Yap_opcount[_write_struct];

  writes = c_write.nxvar + c_write.nyvar + c_write.nxval + c_write.nyval +
    c_write.ncons + c_write.nlist + c_write.nstru;

  c_control.nexecs =
    Yap_opcount[_execute] +
    Yap_opcount[_dexecute];

  c_control.ncalls =
    Yap_opcount[_call] +
    Yap_opcount[_fcall];

  c_control.nproceeds =
    Yap_opcount[_procceed]+
    Yap_opcount[_safe_procceed];

  c_control.ncallbips =
    Yap_opcount[_call_cpred] +
    Yap_opcount[_call_c_wfail] +
    Yap_opcount[_try_c] +
    Yap_opcount[_retry_c] +
    Yap_opcount[_op_fail] +
    Yap_opcount[_trust_fail] +
    Yap_opcount[_p_atom_x] +
    Yap_opcount[_p_atom_y] +
    Yap_opcount[_p_atomic_x] +
    Yap_opcount[_p_atomic_y] +
    Yap_opcount[_p_compound_x] +
    Yap_opcount[_p_compound_y] +
    Yap_opcount[_p_float_x] +
    Yap_opcount[_p_float_y] +
    Yap_opcount[_p_integer_x] +
    Yap_opcount[_p_integer_y] +
    Yap_opcount[_p_nonvar_x] +
    Yap_opcount[_p_nonvar_y] +
    Yap_opcount[_p_number_x] +
    Yap_opcount[_p_number_y] +
    Yap_opcount[_p_var_x] +
    Yap_opcount[_p_var_y] +
    Yap_opcount[_p_db_ref_x] +
    Yap_opcount[_p_db_ref_y] +
    Yap_opcount[_p_cut_by_x] +
    Yap_opcount[_p_cut_by_y] +
    Yap_opcount[_p_primitive_x] +
    Yap_opcount[_p_primitive_y] +
    Yap_opcount[_p_equal] +
    Yap_opcount[_p_plus_vv] +
    Yap_opcount[_p_plus_vc] +
    Yap_opcount[_p_plus_y_vv] +
    Yap_opcount[_p_plus_y_vc] +
    Yap_opcount[_p_minus_vv] +
    Yap_opcount[_p_minus_cv] +
    Yap_opcount[_p_minus_y_vv] +
    Yap_opcount[_p_minus_y_cv] +
    Yap_opcount[_p_times_vv] +
    Yap_opcount[_p_times_vc] +
    Yap_opcount[_p_times_y_vv] +
    Yap_opcount[_p_times_y_vc] +
    Yap_opcount[_p_div_vv] +
    Yap_opcount[_p_div_vc] +
    Yap_opcount[_p_div_cv] +
    Yap_opcount[_p_div_y_vv] +
    Yap_opcount[_p_div_y_vc] +
    Yap_opcount[_p_div_y_cv] +
    Yap_opcount[_p_or_vv] +
    Yap_opcount[_p_or_vc] +
    Yap_opcount[_p_or_y_vv] +
    Yap_opcount[_p_or_y_vc] +
    Yap_opcount[_p_and_vv] +
    Yap_opcount[_p_and_vc] +
    Yap_opcount[_p_and_y_vv] +
    Yap_opcount[_p_and_y_vc] +
    Yap_opcount[_p_sll_vv] +
    Yap_opcount[_p_sll_vc] +
    Yap_opcount[_p_sll_y_vv] +
    Yap_opcount[_p_sll_y_vc] +
    Yap_opcount[_p_slr_vv] +
    Yap_opcount[_p_slr_vc] +
    Yap_opcount[_p_slr_y_vv] +
    Yap_opcount[_p_slr_y_vc] +
    Yap_opcount[_p_dif] +
    Yap_opcount[_p_eq] +
    Yap_opcount[_p_arg_vv] +
    Yap_opcount[_p_arg_cv] +
    Yap_opcount[_p_arg_y_vv] +
    Yap_opcount[_p_arg_y_cv] +
    Yap_opcount[_p_functor] +
    Yap_opcount[_p_func2s_vv] +
    Yap_opcount[_p_func2s_cv] +
    Yap_opcount[_p_func2s_vc] +
    Yap_opcount[_p_func2s_y_vv] +
    Yap_opcount[_p_func2s_y_cv] +
    Yap_opcount[_p_func2s_y_vc] +
    Yap_opcount[_p_func2f_xx] +
    Yap_opcount[_p_func2f_xy] +
    Yap_opcount[_p_func2f_yx] +
    Yap_opcount[_p_func2f_yy];

  c_control.ncuts =
    Yap_opcount[_cut] +
    Yap_opcount[_cut_t] +
    Yap_opcount[_cut_e] +
    Yap_opcount[_commit_b_x] +
    Yap_opcount[_commit_b_y];

  c_control.nallocs =
    Yap_opcount[_allocate] +
    Yap_opcount[_fcall];

  c_control.ndeallocs =
    Yap_opcount[_dexecute] +
    Yap_opcount[_deallocate];

  controls =
    c_control.nexecs +
    c_control.ncalls +
    c_control.nproceeds +
    c_control.ncuts +
    c_control.nallocs +
    c_control.ndeallocs +
    Yap_opcount[_jump] +
    Yap_opcount[_move_back] +
    Yap_opcount[_try_in];



  c_cp.ntries =
    Yap_opcount[_try_me] +
    Yap_opcount[_try_and_mark] +
    Yap_opcount[_try_c] +
    Yap_opcount[_try_clause] +
    Yap_opcount[_either];

  c_cp.nretries =
    Yap_opcount[_retry_me] +
    Yap_opcount[_retry_and_mark] +
    Yap_opcount[_retry_c] +
    Yap_opcount[_retry] +
    Yap_opcount[_or_else];

  c_cp.ntrusts =
    Yap_opcount[_trust_me] +
    Yap_opcount[_trust] +
    Yap_opcount[_or_last];

  choice_pts =
    c_cp.ntries +
    c_cp.nretries +
    c_cp.ntrusts;

  indexes =
    Yap_opcount[_jump_if_var] +
    Yap_opcount[_switch_on_type] +
    Yap_opcount[_switch_list_nl] +
    Yap_opcount[_switch_on_arg_type] +
    Yap_opcount[_switch_on_sub_arg_type] +
    Yap_opcount[_switch_on_cons] +
    Yap_opcount[_go_on_cons] +
    Yap_opcount[_if_cons] +
    Yap_opcount[_switch_on_func] +
    Yap_opcount[_go_on_func] +
    Yap_opcount[_if_func] +
    Yap_opcount[_if_not_then];
  misc =
    c_control.ncallbips +
    Yap_opcount[_Ystop] +
    Yap_opcount[_Nstop] +
    Yap_opcount[_index_pred] +
    Yap_opcount[_lock_pred] +
#if THREADS
    Yap_opcount[_thread_local] +
#endif
    Yap_opcount[_save_b_x] +
    Yap_opcount[_save_b_y] +
    Yap_opcount[_undef_p] +
    Yap_opcount[_spy_pred] +
    Yap_opcount[_spy_or_trymark] +
    Yap_opcount[_save_pair_x] +
    Yap_opcount[_save_pair_y] +
    Yap_opcount[_save_pair_x_write] +
    Yap_opcount[_save_pair_y_write] +
    Yap_opcount[_save_appl_x] +
    Yap_opcount[_save_appl_y] +
    Yap_opcount[_save_appl_x_write] +
    Yap_opcount[_save_appl_y_write];
  total = gets + unifies + puts + writes + controls + choice_pts + indexes + misc;

  /*  for (i = 0; i <= _std_top; ++i)
   * print_instruction(i);
   */

  fprintf(Yap_stderr, "Groups are\n\n");
  fprintf(Yap_stderr, "  GET               instructions: %8d (%3d%%)\n", gets,
	     (gets * 100) / total);
  fprintf(Yap_stderr, "  UNIFY             instructions: %8d (%3d%%)\n", unifies,
	     (unifies * 100) / total);
  fprintf(Yap_stderr, "  PUT               instructions: %8d (%3d%%)\n", puts,
	     (puts * 100) / total);
  fprintf(Yap_stderr, "  WRITE             instructions: %8d (%3d%%)\n", writes,
	     (writes * 100) / total);
  fprintf(Yap_stderr, "  CONTROL           instructions: %8d (%3d%%)\n", controls,
	     (controls * 100) / total);
  fprintf(Yap_stderr, "  CHOICE POINT      instructions: %8d (%3d%%)\n", choice_pts,
	     (choice_pts * 100) / total);
  fprintf(Yap_stderr, "  INDEXING          instructions: %8d (%3d%%)\n", indexes,
	     (indexes * 100) / total);
  fprintf(Yap_stderr, "  MISCELLANEOUS     instructions: %8d (%3d%%)\n", misc,
	     (misc * 100) / total);
  fprintf(Yap_stderr, "_______________________________________________\n");
  fprintf(Yap_stderr, "   TOTAL            instructions: %8d (%3d%%)\n\n", total,
	     (total * 100) / total);

  fprintf(Yap_stderr, "\n Analysis of Unification Instructions in %s \n", program);
  fprintf(Yap_stderr, "           XVAR,   YVAR,    XVAL,    YVAL,     CONS,     LIST,  STRUCT\n");
  fprintf(Yap_stderr, "  GET: %8d %8d %8d %8d %8d %8d %8d\n",
	     c_get.nxvar,
	     c_get.nyvar,
	     c_get.nxval,
	     c_get.nyval,
	     c_get.ncons,
	     c_get.nlist,
	     c_get.nstru);
  fprintf(Yap_stderr, "UNIFY: %8d %8d %8d %8d %8d %8d %8d\n",
	     c_unify.nxvar,
	     c_unify.nyvar,
	     c_unify.nxval,
	     c_unify.nyval,
	     c_unify.ncons,
	     c_unify.nlist,
	     c_unify.nstru);
  fprintf(Yap_stderr, "  PUT: %8d %8d %8d %8d %8d %8d %8d\n",
	     c_put.nxvar,
	     c_put.nyvar,
	     c_put.nxval,
	     c_put.nyval,
	     c_put.ncons,
	     c_put.nlist,
	     c_put.nstru);
  fprintf(Yap_stderr, "WRITE: %8d %8d %8d %8d %8d %8d %8d\n",
	     c_write.nxvar,
	     c_write.nyvar,
	     c_write.nxval,
	     c_write.nyval,
	     c_write.ncons,
	     c_write.nlist,
	     c_write.nstru);
  fprintf(Yap_stderr, "      ___________________________________________________\n");
  fprintf(Yap_stderr, "TOTAL: %8d %8d %8d %8d %8d %8d %8d\n",
	     c_get.nxvar + c_unify.nxvar + c_put.nxvar + c_write.nxvar,
	     c_get.nyvar + c_unify.nyvar + c_put.nyvar + c_write.nyvar,
	     c_get.nxval + c_unify.nxval + c_put.nxval + c_write.nxval,
	     c_get.nyval + c_unify.nyval + c_put.nyval + c_write.nyval,
	     c_get.ncons + c_unify.ncons + c_put.ncons + c_write.ncons,
	     c_get.nlist + c_unify.nlist + c_put.nlist + c_write.nlist,
	     c_get.nstru + c_unify.nstru + c_put.nstru + c_write.nstru
    );

  fprintf(Yap_stderr, "\n Analysis of Unification Instructions in %s \n", program);
  fprintf(Yap_stderr, "           XVAR,   YVAR,    XVAL,    YVAL,     CONS,     LIST,  STRUCT\n");
  fprintf(Yap_stderr, "  GET:  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%\n",
	     (((double) c_get.nxvar) * 100) / total,
	     (((double) c_get.nyvar) * 100) / total,
	     (((double) c_get.nxval) * 100) / total,
	     (((double) c_get.nyval) * 100) / total,
	     (((double) c_get.ncons) * 100) / total,
	     (((double) c_get.nlist) * 100) / total,
	     (((double) c_get.nstru) * 100) / total);
  fprintf(Yap_stderr, "UNIFY:  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%\n",
	     (((double) c_unify.nxvar) * 100) / total,
	     (((double) c_unify.nyvar) * 100) / total,
	     (((double) c_unify.nxval) * 100) / total,
	     (((double) c_unify.nyval) * 100) / total,
	     (((double) c_unify.ncons) * 100) / total,
	     (((double) c_unify.nlist) * 100) / total,
	     (((double) c_unify.nstru) * 100) / total);
  fprintf(Yap_stderr, "  PUT:  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%\n",
	     (((double) c_put.nxvar) * 100) / total,
	     (((double) c_put.nyvar) * 100) / total,
	     (((double) c_put.nxval) * 100) / total,
	     (((double) c_put.nyval) * 100) / total,
	     (((double) c_put.ncons) * 100) / total,
	     (((double) c_put.nlist) * 100) / total,
	     (((double) c_put.nstru) * 100) / total);
  fprintf(Yap_stderr, "WRITE:  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%\n",
	     (((double) c_write.nxvar) * 100) / total,
	     (((double) c_write.nyvar) * 100) / total,
	     (((double) c_write.nxval) * 100) / total,
	     (((double) c_write.nyval) * 100) / total,
	     (((double) c_write.ncons) * 100) / total,
	     (((double) c_write.nlist) * 100) / total,
	     (((double) c_write.nstru) * 100) / total);
  fprintf(Yap_stderr, "      ___________________________________________________\n");
  fprintf(Yap_stderr, "TOTAL:  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%  %3.2f%%\n",
	     (((double) c_get.nxvar + c_unify.nxvar + c_put.nxvar + c_write.nxvar) * 100) / total,
	     (((double) c_get.nyvar + c_unify.nyvar + c_put.nyvar + c_write.nyvar) * 100) / total,
	     (((double) c_get.nxval + c_unify.nxval + c_put.nxval + c_write.nxval) * 100) / total,
	     (((double) c_get.nyval + c_unify.nyval + c_put.nyval + c_write.nyval) * 100) / total,
	     (((double) c_get.ncons + c_unify.ncons + c_put.ncons + c_write.ncons) * 100) / total,
	     (((double) c_get.nlist + c_unify.nlist + c_put.nlist + c_write.nlist) * 100) / total,
	     (((double) c_get.nstru + c_unify.nstru + c_put.nstru + c_write.nstru) * 100) / total
    );

  fprintf(Yap_stderr, "\n Control Instructions Executed in %s \n", program);
  fprintf(Yap_stderr, "Grouped as\n\n");
  fprintf(Yap_stderr, "  CALL              instructions: %8d (%3d%%)\n",
	     c_control.ncalls, (c_control.ncalls * 100) / total);
  fprintf(Yap_stderr, "  PROCEED           instructions: %8d (%3d%%)\n",
	     c_control.nproceeds, (c_control.nproceeds * 100) / total);
  fprintf(Yap_stderr, "  EXECUTE           instructions: %8d (%3d%%)\n",
	     c_control.nexecs, (c_control.nexecs * 100) / total);
  fprintf(Yap_stderr, "  CUT               instructions: %8d (%3d%%)\n",
	     c_control.ncuts, (c_control.ncuts * 100) / total);
  fprintf(Yap_stderr, "  CALL_BIP          instructions: %8d (%3d%%)\n",
	     c_control.ncallbips, (c_control.ncallbips * 100) / total);
  fprintf(Yap_stderr, "  ALLOCATE          instructions: %8d (%3d%%)\n",
	     c_control.nallocs, (c_control.nallocs * 100) / total);
  fprintf(Yap_stderr, "  DEALLOCATE        instructions: %8d (%3d%%)\n",
	     c_control.ndeallocs, (c_control.ndeallocs * 100) / total);
  fprintf(Yap_stderr, "_______________________________________________\n");
  fprintf(Yap_stderr, "   TOTAL            instructions: %8d (%3d%%)\n\n", total,
	     (total * 100) / total);

  fprintf(Yap_stderr, "\n Choice Point Manipulation Instructions Executed in %s \n", program);
  fprintf(Yap_stderr, "Grouped as\n\n");
  fprintf(Yap_stderr, "  TRY              instructions: %8d (%3d%%)\n",
	     c_cp.ntries, (c_cp.ntries * 100) / total);
  fprintf(Yap_stderr, "  RETRY            instructions: %8d (%3d%%)\n",
	     c_cp.nretries, (c_cp.nretries * 100) / total);
  fprintf(Yap_stderr, "  TRUST            instructions: %8d (%3d%%)\n",
	     c_cp.ntrusts, (c_cp.ntrusts * 100) / total);
  fprintf(Yap_stderr, "_______________________________________________\n");
  fprintf(Yap_stderr, "   TOTAL            instructions: %8d (%3d%%)\n\n", total,
	     (total * 100) / total);

  return TRUE;
}

static Int
p_show_sequences(void)
{
  int i, j;
  YAP_ULONG_LONG min;
  YAP_ULONG_LONG sum = 0;
  Term t = Deref(ARG1);

  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR, t, "shows_sequences/1");
    return FALSE;
  }
  if (!IsIntegerTerm(t)) {
    Yap_Error(TYPE_ERROR_INTEGER, t, "shows_sequences/1");
    return FALSE;
  }
  min = (YAP_ULONG_LONG)IntegerOfTerm(t);
  if (min <= 0) {
    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t, "shows_sequences/1");
    return FALSE;
  }
  if (min <= 0) {
    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t, "shows_sequences/1");
    return FALSE;
  }
  for (i = 0; i <= _std_top; ++i) {
    sum += Yap_opcount[i];
  }
  for (i = 0; i <= _std_top; ++i) {
    for (j = 0; j <= _std_top; ++j) {
      YAP_ULONG_LONG seqs = Yap_2opcount[i][j];
      if (seqs && sum/seqs <= min) {
	/*
	Term t[3], t0;
	Functor f = 
	t[0] = Yap_MkFloatTerm(((double)seqs*100.0)/sum);
	t[1] = Yap_LookupAtom(Yap_op_names[i]);
	t[2] = Yap_LookupAtom(Yap_op_names[j]);
	t0 = MkApplTerm(
	Yap_MkPairTerm(Yap_op_names[i]
	*/
	fprintf(stderr,"%f -> %s,%s\n",((double)seqs*100.0)/sum,Yap_op_names[i],Yap_op_names[j]);
	/* we found one */
      }
    }  
  }
  return TRUE;
}

void 
Yap_InitAnalystPreds(void)
{
  Yap_InitCPred("wam_profile_reset_op_counters", 0, p_reset_op_counters, SafePredFlag |SyncPredFlag);
  Yap_InitCPred("wam_profile_show_op_counters", 1, p_show_op_counters, SafePredFlag|SyncPredFlag);
  Yap_InitCPred("wam_profile_show_ops_by_group", 1, p_show_ops_by_group, SafePredFlag |SyncPredFlag);
  Yap_InitCPred("wam_profile_show_sequences", 1, p_show_sequences, SafePredFlag |SyncPredFlag);
}

#endif /* ANALYST */
