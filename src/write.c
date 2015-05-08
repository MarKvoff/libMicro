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

#ifdef linux
#define	_XOPEN_SOURCE 500
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "libmicro.h"

typedef struct {
	char			*ts_buf;
	int			ts_fd;
} tsd_t;

#define	DEFF			"/dev/null"
#define	DEFS			1024

static int			optc = 0;
static char			*optf = DEFF;
static long long		opts = DEFS;
static int			optd;

int
benchmark_init()
{
	lm_tsdsize = sizeof (tsd_t);

	(void) sprintf(lm_optstr, "cdf:s:");

	(void) sprintf(lm_usage,
	    "       [-f file-to-write (default %s)]\n"
	    "       [-s buffer-size (default %d)]\n"
	    "       [-c ] (make sure buffer is in cache)\n"
#ifdef __sun
	    "       [-d ] use directio"
#endif
	    "notes: measures write()\n",
	    DEFF, DEFS);

	(void) sprintf(lm_header, "%8s", "size");

	return (0);
}

int
benchmark_optswitch(int opt, char *optarg)
{
	switch (opt) {

	case 'd':
		optd++;
		break;
	case 'c':
		optc++;
		break;
	case 'f':
		optf = optarg;
		break;
	case 's':
		opts = sizetoll(optarg);
		break;
	default:
		return (-1);
	}
	return (0);
}

int
benchmark_pre(void *tsd)
{
	tsd_t			*ts = (tsd_t *)tsd;
	int			i;

	if (ts->ts_buf == NULL) {
		LM_CHK((ts->ts_buf = malloc(opts)) != NULL);
		LM_CHK((ts->ts_fd = open(optf, O_WRONLY)) != -1);

#ifdef __sun
		if (optd) {
			LM_CHK(directio(ts->ts_fd, DIRECTIO_ON) == 0);
		}
#endif
		/*
		 * Bring buf into cache if specified.
		 */
		if (optc)
			for (i = 0; i < opts; i++)
				ts->ts_buf[i] = 0;
	}

	LM_CHK(lseek(ts->ts_fd, 0, SEEK_SET) == 0);

	return (0);
}

int
benchmark(void *tsd, result_t *res)
{
	tsd_t			*ts = (tsd_t *)tsd;

	LM_CHK(write(ts->ts_fd, ts->ts_buf, opts) == opts);

	return (0);
}

char *
benchmark_result()
{
	static char		result[256];

	(void) sprintf(result, "%8lld", opts);

	return (result);
}
