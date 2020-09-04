/*
 *	error handler header
 */

#ifndef MYW_ERR_H
#define MYW_ERR_H 

#include <stdio.h>
#include <stdlib.h>

/*
 * err_msg - Print error message
 */
void err_msg(const char *err);

/*
 * err_exit - Print error and exit
 */
void err_exit(const char *err);

/*
 * usage_err - Print usage and exit
 */
void usage_err(const char *err);

/*
 * fatal_err - Handle fatal error
 */
void fatal_err(const char *err);

#endif