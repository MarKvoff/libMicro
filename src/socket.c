/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2015 Ryan Zezeski <ryan@zinascii.com>
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "libmicro.h"

typedef struct {
	int			ts_once;
	int			ts_fd;
} tsd_t;

#define	DEFF		"PF_UNIX"

static char			*optf = DEFF;
static int			family;

int
lookup_family(char *name)
{
	if (strcmp("PF_UNIX", name) == 0) {
		return (PF_UNIX);
	} else if (strcmp("PF_INET", name) == 0) {
		return (PF_INET);
	} else if (strcmp("PF_INET6", name) == 0) {
		return (PF_INET6);
	}

	return (-1);
}

int
benchmark_init()
{
	lm_tsdsize = sizeof (tsd_t);

	(void) sprintf(lm_optstr, "f:n");

	(void) sprintf(lm_usage,
	    "       [-f socket-family (default %s)]\n"
	    "notes: measures socket\n",
	    DEFF);

	return (0);
}

int
benchmark_optswitch(int opt, char *optarg)
{
	switch (opt) {
	case 'f':
		optf = optarg;
		break;
	default:
		return (-1);
	}

	return (0);
}

int
benchmark_initrun()
{
	(void) setfdlimit(lm_optT + 10);
	family = lookup_family(optf);

	return (0);
}

int
benchmark_pre(void *tsd)
{
	tsd_t			*ts = (tsd_t *)tsd;

	if (ts->ts_once++ == 0) {
		ts->ts_fd = -1;
	}

	return (0);
}

int
benchmark(void *tsd, result_t *res)
{
	tsd_t			*ts = (tsd_t *)tsd;

	LM_CHK((ts->ts_fd = socket(family, SOCK_STREAM, 0)) != -1);

	return (0);
}

int
benchmark_post(void *tsd)
{
	tsd_t			*ts = (tsd_t *)tsd;

	LM_CHK(close(ts->ts_fd) == 0);

	return (0);
}
