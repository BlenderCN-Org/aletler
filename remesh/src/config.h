/*
 * =====================================================================================
 *
 *       Filename:  config.h
 *
 *        Version:  1.0
 *        Created:  06/14/2012 00:32:13
 *       Revision:  none
 *       Compiler:  icpc
 *
 *         Author:  Changxi Zheng (cz), cxz@cs.columbia.edu
 *                  Columbia University
 *
 * =====================================================================================
 */
#ifndef CONFIG_INC
#   define CONFIG_INC

#ifdef USE_DOUBLE_PRECI
#   define REAL          double
#elif USE_SINGLE_PRECI
#   define REAL          float
#else
#   define REAL          double
#endif

#endif
