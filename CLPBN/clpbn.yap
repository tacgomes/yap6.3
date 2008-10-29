

:- module(clpbn, [{}/1,
		  clpbn_flag/2,
		  set_clpbn_flag/2,
		  clpbn_flag/3,
		  clpbn_key/2,
		  clpbn_init_solver/3,
		  clpbn_run_solver/3]).

:- use_module(library(atts)).
:- use_module(library(lists)).
:- use_module(library(terms)).

:- op( 500, xfy, with).

%
% avoid the overhead of using goal_expansion/2.
%
:- multifile
	user:term_expansion/2.

:- dynamic
	user:term_expansion/2.

:- attribute key/1, dist/2, evidence/1, starter/0.


:- use_module('clpbn/vel',
	      [vel/3,
	       check_if_vel_done/1
	      ]).

:- use_module('clpbn/jt',
	      [jt/3
	      ]).

:- use_module('clpbn/bnt',
	      [do_bnt/3,
	       check_if_bnt_done/1
	      ]).

:- use_module('clpbn/gibbs',
	      [gibbs/3,
	       check_if_gibbs_done/1,
	       init_gibbs_solver/3,
	       run_gibbs_solver/3
	      ]).

:- use_module('clpbn/graphs',
	      [
	       clpbn2graph/1
	      ]).

:- use_module('clpbn/dists',
	      [
	       dist/4,
	       get_dist/4,
	       get_evidence_position/3,
	       get_evidence_from_position/3
	      ]).

:- use_module('clpbn/evidence',
	      [
	       store_evidence/1,
	       incorporate_evidence/2,
	       check_stored_evidence/2,
	       add_evidence/2
	      ]).

:- use_module('clpbn/utils',
	      [
	       sort_vars_by_key/3
	      ]).

:- use_module('clpbn/connected',
	      [
	       influences/4
	      ]).

:- dynamic solver/1,output/1,use/1,suppress_attribute_display/1.

solver(jt).

%output(xbif(user_error)).
%output(gviz(user_error)).
output(no).
suppress_attribute_display(false).

clpbn_flag(Flag,Option) :-
	clpbn_flag(Flag, Option, Option).

set_clpbn_flag(Flag,Option) :-
	clpbn_flag(Flag, _, Option).

clpbn_flag(output,Before,After) :-
	retract(output(Before)),
	assert(output(After)).
clpbn_flag(solver,Before,After) :-
	retract(solver(Before)),
	assert(solver(After)).
clpbn_flag(bnt_solver,Before,After) :-
	retract(bnt:bnt_solver(Before)),
	assert(bnt:bnt_solver(After)).
clpbn_flag(bnt_path,Before,After) :-
	retract(bnt:bnt_path(Before)),
	assert(bnt:bnt_path(After)).
clpbn_flag(bnt_model,Before,After) :-
	retract(bnt:bnt_model(Before)),
	assert(bnt:bnt_model(After)).
clpbn_flag(suppress_attribute_display,Before,After) :-
	retract(suppress_attribute_display(Before)),
	assert(suppress_attribute_display(After)).


{Var = Key with Dist} :-
	put_atts(El,[key(Key),dist(DistInfo,Parents)]),
	dist(Dist, DistInfo, Key, Parents),
	add_evidence(Var,Key,DistInfo,El).

check_constraint(Constraint, _, _, Constraint) :- var(Constraint), !.
check_constraint((A->D), _, _, (A->D)) :- var(A), !.
check_constraint((([A|B].L)->D), Vars, NVars, (([A|B].NL)->D)) :- !,
	check_cpt_input_vars(L, Vars, NVars, NL).
check_constraint(Dist, _, _, Dist).

check_cpt_input_vars([], _, _, []).
check_cpt_input_vars([V|L], Vars, NVars, [NV|NL]) :-
	replace_var(Vars, V, NVars, NV),
	check_cpt_input_vars(L, Vars, NVars, NL).

replace_var([], V, [], V).
replace_var([V|_], V0, [NV|_], NV) :- V == V0, !.
replace_var([_|Vars], V, [_|NVars], NV) :-
	replace_var(Vars, V, NVars, NV).

add_evidence(V,Key,Distinfo,NV) :-
	nonvar(V), !,
	get_evidence_position(V, Distinfo, Pos),
	check_stored_evidence(Key, Pos),
	clpbn:put_atts(NV,evidence(Pos)).
add_evidence(V,K,_,V) :-
	add_evidence(K,V).

clpbn_marginalise(V, Dist) :-
	attributes:all_attvars(AVars),
	project_attributes([V], AVars),
	vel:get_atts(V, posterior(_,_,Dist,_)).

%
% called by top-level
% or by call_residue/2
%
project_attributes(GVars, AVars) :-
	suppress_attribute_display(false),
	AVars = [_|_],
	solver(Solver),
	( GVars = [_|_] ; Solver = graphs), !,
	clpbn_vars(AVars, DiffVars, AllVars),
	get_clpbn_vars(GVars,CLPBNGVars0),
	simplify_query_vars(CLPBNGVars0, CLPBNGVars),
	(
	    Solver = graphs
	->
	    write_out(Solver, [[]], AllVars, DiffVars)
	;
	    influences(AllVars, CLPBNGVars, _, NewAllVars),
writeln(AllVars:NewAllVars),
	    write_out(Solver, [CLPBNGVars], NewAllVars, DiffVars)
	).
project_attributes(_, _).

clpbn_vars(AVars, DiffVars, AllVars) :-
	sort_vars_by_key(AVars,SortedAVars,DiffVars),
	incorporate_evidence(SortedAVars, AllVars).

get_clpbn_vars([],[]).
get_clpbn_vars([V|GVars],[V|CLPBNGVars]) :-
	get_atts(V, [key(_)]), !,
	get_clpbn_vars(GVars,CLPBNGVars).
get_clpbn_vars([_|GVars],CLPBNGVars) :-
	get_clpbn_vars(GVars,CLPBNGVars).

simplify_query_vars(LVs0, LVs) :-
	sort(LVs0,LVs1),
	get_rid_of_ev_vars(LVs1,LVs).

%
% some variables might already have evidence in the data-base.
%
get_rid_of_ev_vars([],[]).
get_rid_of_ev_vars([V|LVs0],LVs) :-
	clpbn:get_atts(V, [dist(Id,_),evidence(Pos)]), !,
	get_evidence_from_position(Ev, Id, Pos),
	clpbn_display:put_atts(V, [posterior([],Ev,[],[])]), !,
	get_rid_of_ev_vars(LVs0,LVs).
get_rid_of_ev_vars([V|LVs0],[V|LVs]) :-
	get_rid_of_ev_vars(LVs0,LVs).


% do nothing if we don't have query variables to compute.
write_out(graphs, _, AVars, _) :-
	clpbn2graph(AVars).
write_out(vel, GVars, AVars, DiffVars) :-
	vel(GVars, AVars, DiffVars).
write_out(jt, GVars, AVars, DiffVars) :-
	jt(GVars, AVars, DiffVars).
write_out(gibbs, GVars, AVars, DiffVars) :-
	gibbs(GVars, AVars, DiffVars).
write_out(bnt, GVars, AVars, DiffVars) :-
	do_bnt(GVars, AVars, DiffVars).

get_bnode(Var, Goal) :-
	get_atts(Var, [key(Key),dist(Dist,Parents)]),
	get_dist(Dist,_,Domain,CPT),
	(Parents = [] -> X = tab(Domain,CPT) ; X = tab(Domain,CPT,Parents)),
	dist_goal(X, Key, Goal0),
	include_evidence(Var, Goal0, Key, Goali),
	include_starter(Var, Goali, Key, Goal).

include_evidence(Var, Goal0, Key, ((Key:-Ev),Goal0)) :-
	get_atts(Var, [evidence(Ev)]), !.
include_evidence(_, Goal0, _, Goal0).

include_starter(Var, Goal0, Key, ((:-Key),Goal0)) :-
	get_atts(Var, [starter]), !.
include_starter(_, Goal0, _, Goal0).

dist_goal(Dist, Key, (Key=NDist)) :-
	term_variables(Dist, DVars),
	process_vars(DVars, DKeys),
	my_copy_term(Dist,DVars, NDist,DKeys).

my_copy_term(V, DVars, Key, DKeys) :-
	find_var(DVars, V, Key, DKeys).
my_copy_term(A, _, A, _) :- atomic(A), !.
my_copy_term(T, Vs, NT, Ks) :-
	T =.. [Na|As],
	my_copy_terms(As, Vs, NAs, Ks),
	NT =.. [Na|NAs].

my_copy_terms([], _, [], _).
my_copy_terms([A|As], Vs, [NA|NAs], Ks) :-
	my_copy_term(A, Vs, NA, Ks),
	my_copy_terms(As, Vs, NAs, Ks).

find_var([V1|_], V, Key, [Key|_]) :- V1 == V, !.
find_var([_|DVars], V, Key, [_|DKeys]) :-
	find_var(DVars, V, Key, DKeys).

process_vars([], []).
process_vars([V|Vs], [K|Ks]) :-
        process_var(V, K),
	process_vars(Vs, Ks).

process_var(V, K) :- get_atts(V, [key(K)]), !. 
% oops: this variable has no attributes.
process_var(V, _) :- throw(error(instantiation_error,clpbn(attribute_goal(V)))).

%
% unify a CLPBN variable with something. 
%
verify_attributes(Var, T, Goals) :-
	get_atts(Var, [key(Key),dist(Dist,Parents)]), !,
	/* oops, someone trying to bind a clpbn constrained variable */
	Goals = [],
	bind_clpbn(T, Var, Key, Dist, Parents).
verify_attributes(_, _, []).


bind_clpbn(T, Var, Key, Dist, Parents) :- var(T),
	get_atts(T, [key(Key1),dist(Dist1,Parents1)]), !,
	bind_clpbns(Key, Dist, Parents, Key1, Dist1, Parents1),
	(
	  get_atts(T, [evidence(Ev1)]) ->
	    bind_evidence_from_extra_var(Ev1,Var)
	;
	  get_atts(Var, [evidence(Ev)]) ->
	    bind_evidence_from_extra_var(Ev,T)
	;
	  true).
bind_clpbn(_, Var, _, _, _, _) :-
	use(bnt),
	check_if_bnt_done(Var), !.
bind_clpbn(_, Var, _, _, _, _) :-
	use(vel),
	check_if_vel_done(Var), !.
bind_clpbn(_, Var, _, _, _, _) :-
	use(jt),
	check_if_vel_done(Var), !.
bind_clpbn(T, Var, Key0, _, _, _) :-
	get_atts(Var, [key(Key)]), !,
	(
	  Key = Key0 -> true
	;
	  add_evidence(Var,T)
	).

fresh_attvar(Var, NVar) :-
	get_atts(Var, LAtts),
	put_atts(NVar, LAtts).

% I will now allow two CLPBN variables to be bound together.
%bind_clpbns(Key, Dist, Parents, Key, Dist, Parents).
bind_clpbns(Key, Dist, Parents, Key1, Dist1, Parents1) :- 
	Key == Key1, !,
	get_dist(Dist,Type,Domain,Table),
	get_dist(Dist1,Type1,Domain1,Table1),
	( Dist == Dist1,
	  same_parents(Parents,Parents1)
	->
	  true
	;
	  throw(error(domain_error(bayesian_domain),bind_clpbns(var(Key, Type, Domain, Table, Parents),var(Key1, Type1, Domain1, Table1, Parents1))))
	).
bind_clpbns(Key, _, _, _, Key1, _, _, _) :-
	Key\=Key1, !, fail.
bind_clpbns(_, _, _, _, _, _, _, _) :-
	format(user_error, 'unification of two bayesian vars not supported~n', []).

same_parents([],[]).
same_parents([P|Parents],[P1|Parents1]) :-
	same_node(P,P1),
	same_parents(Parents,Parents1).

same_node(P,P1) :- P == P1, !.
same_node(P,P1) :-
	get_atts( P,[key(K)]),
	get_atts(P1,[key(K)]),
	P = P1.


bind_evidence_from_extra_var(Ev1,Var) :-
	get_atts(Var, [evidence(Ev0)]),!,Ev0 = Ev1.
bind_evidence_from_extra_var(Ev1,Var) :-
	put_atts(Var, [evidence(Ev1)]).

user:term_expansion((A :- {}), ( :- true )) :-	 !, % evidence
	prolog_load_context(module, M),
	store_evidence(M:A).

clpbn_key(Var,Key) :-
	get_atts(Var, [key(Key)]).

%
% This is a routine to start a solver, called by the learning procedures (ie, em).
% LVs is a list of lists of variables one is interested in eventually marginalising out
% Vs0 gives the original graph
% AllDiffs gives variables that are not fully constrainted, ie, we don't fully know
% the key. In this case, we assume different instances will be bound to different
% values at the end of the day.
%
clpbn_init_solver(LVs,Vs0,VarsWithUnboundKeys) :-
       	solver(Solver),
	clpbn_init_known_solver(Solver,LVs,Vs0,VarsWithUnboundKeys).

clpbn_init_known_solver(gibbs, LVs, Vs0, VarsWithUnboundKeys) :- !,
	init_gibbs_solver(LVs, Vs0, VarsWithUnboundKeys).
clpbn_init_known_solver(_, _, _, _).

%
% LVs is the list of lists of variables to marginalise
% Vs is the full graph
% Ps are the probabilities on LVs.
% 
%
clpbn_run_solver(LVs,Vs,LPs) :-
       	solver(Solver),
	clpbn_run_known_solver(Solver,LVs,Vs,LPs).

clpbn_run_known_solver(gibbs,LVs, Vs, LPs) :- !,
	run_gibbs_solver(LVs, Vs, LPs).
