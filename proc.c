/* -*- tab-width:4; -*- */
/*
 * Procedure related functions
 *
 * $Id: proc.c 1.23 Mon, 20 Mar 2000 00:26:37 +0100 crad $
 */
#include "s.h"
#include "proc.h"
#include "heap.h"

void scm_env_mark(SOBJ env)
{
  SOBJ val;
  int i;

  if (env == NULL) 	return;

  for (i = 0; i < SCM_INUM(SCM_ENV_FRAME(env)->nslots); i++) {
	val = SCM_ENV_FRAME(env)->binding[i];
	if (scm_is_pointer_to_heap(val)) {
	  scm_gc_mark(val);
	}
  }
  if (SCM_ENV_NEXT(env) != NULL) {
	scm_gc_mark(SCM_ENV_NEXT(env));
  }
}

void scm_proc_mark(SOBJ proc)
{
  SCM_Code *c; 
  int i;
  
  if (SCM_PROC_ENV(proc)) 	scm_gc_mark(SCM_PROC_ENV(proc));

  if ((c = SCM_PROC_CODE(proc)) != NULL) {
	scm_gc_mark(c->envlist);
	
	for (i = 0; i < c->size; i++) {
	  if (scm_is_pointer_to_heap(c->code[i]))
		scm_gc_mark(c->code[i]);
	}
  }
}

void scm_clos_mark(SOBJ obj)
{
  if (SCM_CLOSURE_ENV(obj)) 	scm_gc_mark(SCM_CLOSURE_ENV(obj));
  if (SCM_CLOSURE_CODE(obj))	scm_gc_mark(SCM_CLOSURE_CODE(obj));
}

void scm_env_sweep(SOBJ env)
{
  if (SCM_ENV_FRAME(env)) {
	scm_free(SCM_ENV_FRAME(env));
	SCM_ENV_FRAME(env) = NULL;
  }
}

void scm_proc_sweep(SOBJ proc)
{
  if (SCM_PROC_CODE(proc)) {
	scm_free(SCM_PROC_CODE(proc));
	SCM_PROC_CODE(proc) = NULL;
  }
}

void scm_env_print(SOBJ obj, PORT *p)
{
  int i;
  if (obj == NULL) return;
	
  if (SCM_ENV_FRAME(obj) == NULL) {
	port_puts(p, "#<env null>");
  } else {
	port_puts(p, "#<env next=");
	port_putx(p, SCM_ENV_NEXT(obj));
	port_puts(p, " nslots=");
	port_putn(p, SCM_INUM(SCM_ENV_FRAME(obj)->nslots));
	port_puts(p, " binding[]=");
	for (i = 0; i < SCM_INUM(SCM_ENV_FRAME(obj)->nslots); i++) {
	  if (!SCM_INUMP(SCM_ENV_FRAME(obj)->binding[i]) &&
		  SCM_CLOSUREP(SCM_ENV_FRAME(obj)->binding[i])) {
		port_puts(p, "#<closure>");
	  } else {
		scm_write_obj(SCM_ENV_FRAME(obj)->binding[i], p, 1);
	  }
	  if (i != SCM_INUM(SCM_ENV_FRAME(obj)->nslots)-1) port_putc(p, ' ');
	}
	port_putc(p, '>');
  }
}

void scm_proc_print(SOBJ obj, PORT *p)
{
  SOBJ env, e;
  int i;

  if (SCM_PROC_CODE(obj) == NULL) {
	port_puts(p, "#<proc null>");
  } else {
	port_puts(p, "#<proc (");
	env = SCM_PROC_CODE(obj)->envlist;
	if (env && SCM_PAIRP(env))
	  e = scm_reverse(SCM_CAR(env));
	else
	  e = NULL;

	if (SCM_PROC_CODE(obj)->nlocals) {
	  port_puts(p, "local: ");
	  for (i = 0; i < SCM_PROC_CODE(obj)->nlocals && e; i++) {
		scm_write_obj(SCM_CAAR(e), p, 1);
		e = SCM_CDR(e);  if (e) port_putc(p, ' ');
	  }
	}
	if (SCM_PROC_CODE(obj)->nlocals) {
	  port_puts(p, " args: ");
	}
	while(e) {
	  if (SCM_CAR(e)) {
		scm_write_obj(SCM_CAAR(e), p, 1);
	  } else {
		port_puts(p, "*BAD_ENV*");
		break;
	  }
	  e = SCM_CDR(e);  if (e) port_putc(p, ' ');
	}
	port_puts(p, ") ");
	scm_env_print(SCM_PROC_ENV(obj), p);
	port_puts(p, " code=");
	port_putx(p, SCM_PROC_CODE(obj));
	port_puts(p, ">");
  }
}

void scm_clos_print(SOBJ obj, PORT *p)
{
  port_puts(p, "#<closure ");
  if (SCM_CLOSURE_ENV(obj)) {
	scm_env_print(SCM_CLOSURE_ENV(obj), p);
  } else {
	port_puts(p, "global");
  }
  port_puts(p, " ");
  scm_proc_print(SCM_CLOSURE_CODE(obj), p);
  port_puts(p, ">");
}

/*-- public functions */

/* return #t if x is something we can call */

/*S* (procedure? OBJ) => BOOLEAN */
/*D* Returns #t if OBJ is a procedure, #f otherwise */

SOBJ scm_procedurep(SOBJ x)
{
  return(SCM_MKBOOL(SCM_PROCP(x) || SCM_CLOSUREP(x) ||
					SCM_PRIMP(x) || SCM_CPRIMP(x)));
}

/*E* (environment? OBJ) => BOOLEAN */
/*D* Returns #t if OBJ is an environment, #f otherwise */

/*E* (closure? OBJ) => BOOLEAN */
/*D* Returns #t if OBJ is a closure, #f otherwise */

/*E* (primitive? OBJ) => BOOLEAN */
/*D* Returns #t if OBJ is a primitive, #f otherwise */

/*E* (cprimitive? OBJ) => BOOLEAN */
/*D* Returns #t if OBJ is a C primitive, #f otherwise */

SOBJ scm_environmentp(SOBJ x){  return(SCM_MKBOOL(SCM_ENVP(x))); 		}
SOBJ scm_closurep(SOBJ x) 	 {	return(SCM_MKBOOL(SCM_CLOSUREP(x))); 	}
SOBJ scm_primitivep(SOBJ x)  {  return(SCM_MKBOOL(SCM_PRIMP(x))); 		}
SOBJ scm_cprimitivep(SOBJ x) {  return(SCM_MKBOOL(SCM_CPRIMP(x))); 		}

void scm_init_proc()
{
  scm_add_cprim("procedure?", 	scm_procedurep, 	1);
  scm_add_cprim("environment?", scm_environmentp, 	1);
  scm_add_cprim("closure?",     scm_closurep, 		1);
  scm_add_cprim("primitive?", 	scm_primitivep, 	1);
  scm_add_cprim("cprimitive?", 	scm_cprimitivep, 	1);
}
