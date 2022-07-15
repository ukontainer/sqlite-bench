// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef BENCH_H_
#define BENCH_H_

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "sqlite3.h"

#define kNumBuckets 154
#define kNumData 1000000

typedef struct Histogram {
  double min_;
  double max_;
  double num_;
  double sum_;
  double sum_squares_;
  double buckets_[kNumBuckets];
} Histogram;

typedef struct Raw {
  double *data_;
  size_t data_size_;
  int pos_;
} Raw;

typedef struct Random {
  uint32_t seed_;
} Random;

typedef struct RandomGenerator {
  char *data_;
  size_t data_size_;
  int pos_;
} RandomGenerator;

// Comma-separated list of operations to run in the specified order
//   Actual benchmarks:
//
//   fillseq       -- write N values in sequential key order in async mode
//   fillseqsync   -- write N/100 values in sequential key order in sync mode
//   fillseqbatch  -- batch write N values in sequential key order in async mode
//   fillrandom    -- write N values in random key order in async mode
//   fillrandsync  -- write N/100 values in random key order in sync mode
//   fillrandbatch -- batch write N values in sequential key order in async mode
//   overwrite     -- overwrite N values in random key order in async mode
//   fillrand100K  -- write N/1000 100K values in random order in async mode
//   fillseq100K   -- write N/1000 100K values in sequential order in async mode
//   readseq       -- read N times sequentially
//   readrandom    -- read N times in random order
//   readrand100K  -- read N/1000 100K values in sequential order in async mode
extern char* FLAGS_benchmarks;

// Number of key/values to place in database
extern int FLAGS_num;

// Number of read operations to do.  If negative, do FLAGS_num reads.
extern int FLAGS_reads;

// Size of each value
extern int FLAGS_value_size;

// Print histogram of operation timings
extern bool FLAGS_histogram;

// Print raw data
extern bool FLAGS_raw;

// Arrange to generate values that shrink to this fraction of
// their original size after compression
extern double FLAGS_compression_ratio;

// Page size. Default 1 KB.
extern int FLAGS_page_size;

// Number of pages.
// Default cache size = FLAGS_page_size * FLAGS_num_pages = 4 MB.
extern int FLAGS_num_pages;

// If true, do not destroy the existing database.  If you set this
// flag and also specify a benchmark that wants a fresh database, that
// benchmark will fail.
extern bool FLAGS_use_existing_db;

// If true, we allow batch writes to occur
extern bool FLAGS_transaction;

// If true, we enable Write-Ahead Logging
extern bool FLAGS_WAL_enabled;

// Use the db with the following name.
extern char* FLAGS_db;

/* benchmark.c */
void benchmark_init(void);
void benchmark_fini(void);
void benchmark_run(void);
void benchmark_open(void);
void benchmark_write(bool, int, int, int, int, int);
void benchmark_read(int, int);
void benchmark_read_sequential(void);

/* histogram.c */
void histogram_clear(Histogram*);
void histogram_add(Histogram*, double);
void histogram_merge(Histogram*, const Histogram*);
char* histogram_to_string(Histogram*);

/* Raw */
void raw_clear(Raw *);
void raw_add(Raw *, double);
char* raw_to_string(Raw *);
void raw_print(FILE *, Raw *);

/* random.c */
void rand_init(Random*, uint32_t);
uint32_t rand_next(Random*);
uint32_t rand_uniform(Random*, int);
void rand_gen_init(RandomGenerator*, double);
char* rand_gen_generate(RandomGenerator*, int);

/* util.c */
uint64_t now_micros(void);
bool starts_with(const char*, const char*);
char* trim_space(const char*);

#endif /* BENCH_H_ */
