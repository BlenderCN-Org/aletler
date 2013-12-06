/*
 * Changxi Zheng (cxzheng@cs.cornell.edu)
 */

#ifndef NANO_TIMER_H
#   define NANO_TIMER_H

#undef __BEGIN_DECLS
#undef __END_DECLS

#ifdef	__cplusplus
#   define __BEGIN_DECLS    extern "C" {
#   define __END_DECLS      }
#else
#   define __BEGIN_DECLS
#   define __END_DECLS
#endif

#undef __p
#if defined (__STDC__) || defined (_AIX) \
    || (defined (__mips) && defined (_SYSTYPE_SVR4)) \
    || defined(WIN32) || defined(__cplusplus)
#   define __p(protos) protos
#else
#   define __p(protos) ()
#endif

__BEGIN_DECLS

double    nano_time_d  __p((void));
long long nano_time_i  __p((void));
double    milli_time_d __p((void));
long long milli_time_i __p((void));

__END_DECLS

#endif
