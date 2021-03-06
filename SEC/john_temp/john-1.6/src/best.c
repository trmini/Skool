/*
 * This file is part of John the Ripper password cracker,
 * Copyright (c) 1996-98 by Solar Designer
 */

/*
 * Benchmark to detect the best algorithm for a particular architecture.
 */

#include <stdio.h>
#include <time.h>

#include "params.h"
#include "common.h"
#include "formats.h"
#include "bench.h"

extern struct fmt_main fmt_DES, fmt_MD5, fmt_BF;

int main(int argc, char **argv)
{
	struct fmt_main *format;
	struct bench_results results;
	unsigned long virtual;
	char s_real[64], s_virtual[64];

	if (argc != 2) return 1;

	switch (argv[1][0]) {
	case '1':
		format = &fmt_DES;
		break;

	case '2':
		format = &fmt_MD5;
		break;

	case '3':
		format = &fmt_BF;
		break;

	default:
		return 1;
	}

	fprintf(stderr, "Benchmarking: %s%s [%s]... ",
		format->params.format_name,
		format->params.benchmark_comment,
		format->params.algorithm_name);

	common_init();

	if (benchmark_format(format, BENCHMARK_MANY, &results)) {
		virtual = 0;

		fprintf(stderr, "FAILED\n");
	} else {
		virtual = (unsigned long)
			(results.count * CLK_TCK * 10 / results.virtual);

		benchmark_cps(results.count, results.real, s_real);
		benchmark_cps(results.count, results.virtual, s_virtual);

		fprintf(stderr, "%s c/s real, %s c/s virtual\n",
			s_real, s_virtual);
	}

	printf("%lu\n", virtual);

	return virtual ? 0 : 1;
}
