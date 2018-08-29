// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "bench.h"

static char *random_string(Random*, int);
static char *compressible_string(Random*, double, size_t);

/*
 * https://github.com/google/leveldb/blob/master/util/testutil.cc
 */
static char *random_string(Random* rnd, int len) {
  char* dst = malloc(sizeof(char) * (size_t)(len + 1));
  for (int i = 0; i < len; i++) {
    dst[i] = (char)(' ' + rand_uniform(rnd, 95));
  }
  dst[len] = '\0';

  return dst;
}

static char *compressible_string(Random* rnd, double compressed_fraction,
                                  size_t len) {
  int raw = (int)(len * compressed_fraction);
  if (raw < 1) raw = 1;
  char* raw_data = random_string(rnd, raw);
  size_t raw_data_len = strlen(raw_data);

  int pos = 0;
  char* dst = malloc(sizeof(char) * (len + 1));
  dst[0] = '\0';
  while (pos < len) {
    strcat(dst, raw_data);
    pos += raw_data_len;
  }

  return dst;
}

/*
 * https://github.com/google/leveldb/blob/master/util/random.h
 */
void rand_init(Random* rand_, uint32_t s) {
  rand_->seed_ = s & 0x7fffffffu;
  /* Avoid bad seeds. */
  if (rand_->seed_ == 0 || rand_->seed_ == 2147483647L) {
    rand_->seed_ = 1;
  }
}

uint32_t rand_next(Random* rand_) {
  static const uint32_t M = 2147483647L;
  static const uint64_t A = 16807;

  uint64_t product = rand_->seed_ * A;

  rand_->seed_ = (uint32_t)((product >> 31) + (product & M));

  if (rand_->seed_ > M) {
    rand_->seed_ -= M;
  }

  return rand_->seed_;
}

uint32_t rand_uniform(Random* rand_, int n) { return rand_next(rand_) % n; }

void rand_gen_init(RandomGenerator* gen_, double compression_ratio) {
  Random rnd;
  char* piece;
  
  gen_->data_ = malloc(sizeof(char) * 1048576);
  gen_->data_size_ = 0;
  gen_->pos_ = 0;
  (gen_->data_)[0] = '\0';

  rand_init(&rnd, 301);
  while (gen_->data_size_ < 1048576) {
    piece = compressible_string(&rnd, compression_ratio, 100);
    strcat(gen_->data_, piece);
    gen_->data_size_ += strlen(piece);
  }

  free(piece);
}

char* rand_gen_generate(RandomGenerator* gen_, int len) {
  if (gen_->pos_ + len > gen_->data_size_) {
    gen_->pos_ = 0;
    assert(len < gen_->data_size_);
  }
  gen_->pos_ += len;
  char* substr = malloc(sizeof(char) * (len + 1));
  strncpy(substr, (gen_->data_) + gen_->pos_ - len, len);

  return substr;
}
