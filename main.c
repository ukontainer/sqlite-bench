// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "bench.h"

void init() {
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
  FLAGS_benchmarks =
    "fillseq,"
    "fillseqsync,"
    "fillseqbatch,"
    "fillrandom,"
    "fillrandsync,"
    "fillrandbatch,"
    "overwrite,"
    "overwritebatch,"
    "readrandom,"
    "readseq,"
    "fillrand100K,"
    "fillseq100K,"
    "readseq,"
    "readrand100K,"
    ;
  FLAGS_num = 1000000;
  FLAGS_reads = -1;
  FLAGS_value_size = 100;
  FLAGS_histogram = false;
  FLAGS_compression_ratio = 0.5;
  FLAGS_page_size = 1024;
  FLAGS_num_pages = 4096;
  FLAGS_use_existing_db = false;
  FLAGS_transaction = true;
  FLAGS_WAL_enabled = true;
  FLAGS_db = NULL;
}

int main(int argc, char** argv) {
  init();

  char* default_db_path = malloc(sizeof(char) * 1024);
  strcpy(default_db_path, "..");

  for (int i = 1; i < argc; i++) {
    double d;
    int n;
    char junk;
    if (starts_with("--benchmarks=", argv[i])) {
      FLAGS_benchmarks = argv[i] + strlen("--benchmarks=");
    } else if (sscanf(argv[i], "--histogram=%d%c", &n, &junk) == 1 &&
               (n == 0 || n == 1)) {
      FLAGS_histogram = n;
    } else if (sscanf(argv[i], "--compression_ratio=%lf%c", &d, &junk) == 1) {
      FLAGS_compression_ratio = d;
    } else if (sscanf(argv[i], "--use_existing_db=%d%c", &n, &junk) == 1 &&
               (n == 0 || n == 1)) {
      FLAGS_use_existing_db = n;
    } else if (sscanf(argv[i], "--num=%d%c", &n, &junk) == 1) {
      FLAGS_num = n;
    } else if (sscanf(argv[i], "--reads=%d%c", &n, &junk) == 1) {
      FLAGS_reads = n;
    } else if (sscanf(argv[i], "--value_size=%d%c", &n, &junk) == 1) {
      FLAGS_value_size = n;
    } else if (!strcmp(argv[i], "--no_transaction")) {
      FLAGS_transaction = false;
    } else if (sscanf(argv[i], "--page_size=%d%c", &n, &junk) == 1) {
      FLAGS_page_size = n;
    } else if (sscanf(argv[i], "--num_pages=%d%c", &n, &junk) == 1) {
      FLAGS_num_pages = n;
    } else if (sscanf(argv[i], "--WAL_enabled=%d%c", &n, &junk) == 1 &&
               (n == 0 || n == 1)) {
      FLAGS_WAL_enabled = n;
    } else if (strncmp(argv[i], "--db=", 5) == 0) {
      FLAGS_db = argv[i] + 5;
    } else {
      fprintf(stderr, "Invalid flag '%s'\n", argv[i]);
      exit(1);
    }
  }

  /* Choose a location for the test database if none given with --db=<path>  */
  if (FLAGS_db == NULL) {
      FLAGS_db = strcat(default_db_path, "/dbbench");
  }

  benchmark_init();
  benchmark_run();
  benchmark_fini();

  return 0;
}
