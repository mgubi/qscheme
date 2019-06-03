/*
 * Procedure related functions
 *
 * $Id: proc.h 1.4 Wed, 19 Aug 1998 02:40:44 +0200 crad $
 */
void        scm_env_mark(SOBJ env);
void        scm_proc_mark(SOBJ proc);
void        scm_clos_mark(SOBJ obj);
void        scm_env_sweep(SOBJ env);
void        scm_proc_sweep(SOBJ proc);
void        scm_env_print(SOBJ obj, PORT *p);
void        scm_proc_print(SOBJ obj, PORT *p);
void        scm_clos_print(SOBJ obj, PORT *p);
