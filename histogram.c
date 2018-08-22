// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "bench.h"

static double median(Histogram*);
static double percentile(Histogram*, double);
static double average(Histogram*);
static double standard_deviation(Histogram*);

const static double bucket_limit[kNumBuckets] = {
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40, 45,
  50, 60, 70, 80, 90, 100, 120, 140, 160, 180, 200, 250, 300, 350, 400, 450,
  500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800, 2000, 2500, 3000,
  3500, 4000, 4500, 5000, 6000, 7000, 8000, 9000, 10000, 12000, 14000,
  16000, 18000, 20000, 25000, 30000, 35000, 40000, 45000, 50000, 60000,
  70000, 80000, 90000, 100000, 120000, 140000, 160000, 180000, 200000,
  250000, 300000, 350000, 400000, 450000, 500000, 600000, 700000, 800000,
  900000, 1000000, 1200000, 1400000, 1600000, 1800000, 2000000, 2500000,
  3000000, 3500000, 4000000, 4500000, 5000000, 6000000, 7000000, 8000000,
  9000000, 10000000, 12000000, 14000000, 16000000, 18000000, 20000000,
  25000000, 30000000, 35000000, 40000000, 45000000, 50000000, 60000000,
  70000000, 80000000, 90000000, 100000000, 120000000, 140000000, 160000000,
  180000000, 200000000, 250000000, 300000000, 350000000, 400000000,
  450000000, 500000000, 600000000, 700000000, 800000000, 900000000,
  1000000000, 1200000000, 1400000000, 1600000000, 1800000000, 2000000000,
  2500000000.0, 3000000000.0, 3500000000.0, 4000000000.0, 4500000000.0,
  5000000000.0, 6000000000.0, 7000000000.0, 8000000000.0, 9000000000.0,
  1e200,
};

static double median(Histogram* hist_) {
  return percentile(hist_, 50.0);
}

static double percentile(Histogram* hist_, double p) {
  double threshold = hist_->num_ * (p / 100.0);
  double sum = 0;
  for (int b = 0; b < kNumBuckets; b++) {
    sum += hist_->buckets_[b];
    if (sum >= threshold) {
      /* Scale linearly within this bucket */
      double left_point = (b == 0) ? 0 : bucket_limit[b - 1];
      double right_point = bucket_limit[b];
      double left_sum = sum - hist_->buckets_[b];
      double right_sum = sum;
      double pos = (threshold - left_sum) / (right_sum - left_sum);
      double r = left_point + (right_point - left_point) * pos;
      if (r < hist_->min_) r = hist_->min_;
      if (r > hist_->max_) r = hist_->max_;
      return r;
    }
  }
  return hist_->max_;
}

static double average(Histogram* hist_) {
  if (hist_->num_ == 0.0) return 0;
  return hist_->sum_ / hist_->num_;
}

static double standard_deviation(Histogram* hist_) {
  if (hist_->num_ == 0.0) return 0;
  double variance = (hist_->sum_squares_ * hist_->num_ - hist_->sum_ * hist_->sum_) / (hist_->num_ * hist_->num_);
  return sqrt(variance);
}

void histogram_clear(Histogram* hist_) {
  hist_->min_ = bucket_limit[kNumBuckets - 1];
  hist_->max_ = 0;
  hist_->num_ = 0;
  hist_->sum_ = 0;
  hist_->sum_squares_ = 0;
  for (int i = 0; i < kNumBuckets; i++) {
    hist_->buckets_[i] = 0;
  }
}
void histogram_add(Histogram* hist_, double value) {
  int b = 0;
  while (b < kNumBuckets - 1 && bucket_limit[b] <= value) {
    b++;
  }
  hist_->buckets_[b] += 1.0;
  if (hist_->min_ > value) hist_->min_ = value;
  if (hist_->max_ < value) hist_->max_ = value;
  hist_->num_++;
  hist_->sum_ += value;
  hist_->sum_squares_ += (value * value);
}
void histogram_merge(Histogram* hist_, const Histogram* other_) {
  if (other_->min_ < hist_->min_) hist_->min_ = other_->min_;
  if (other_->max_ > hist_->max_) hist_->max_ = other_->max_;
  hist_->num_ += other_->num_;
  hist_->sum_ += other_->sum_;
  hist_->sum_squares_ += other_->sum_squares_;
  for (int b = 0; b < kNumBuckets; b++) {
    hist_->buckets_[b] += other_->buckets_[b];
  }
}
char* histogram_to_string(Histogram* hist_) {
  size_t r_size = 1024;
  char* r = malloc(sizeof(char) * 1024);
  strcpy(r, "");
  char buf[200];
  snprintf(buf, sizeof(buf),
            "Count: %.0f  Average: %.4f  StdDiv: %.2f\n",
            hist_->num_, average(hist_), standard_deviation(hist_));
  if (r_size < strlen(r) + strlen(buf)) {
    r = realloc(r, r_size * 2);
    r_size *= 2;
  }
  strcat(r, buf);
  snprintf(buf, sizeof(buf),
            "Min: %.4f  Median: %.4f  Max: %.4f\n",
            (hist_->num_ == 0.0 ? 0.0 : hist_->min_),
            median(hist_), hist_->max_);
  if (r_size < strlen(r) + strlen(buf)) {
    r = realloc(r, r_size * 2);
    r_size *= 2;
  }
  strcat(r, buf);
  if (r_size < strlen(r) + strlen(buf)) {
    r = realloc(r, r_size * 2);
    r_size *= 2;
  }
  strcat(r, "------------------------------------------------------\n");
  const double mult = 100.0 / hist_->num_;
  double sum = 0;
  for (int b = 0; b < kNumBuckets; b++) {
    if (hist_->buckets_[b] <= 0.0) continue;
    sum += hist_->buckets_[b];
    snprintf(buf, sizeof(buf),
              "[ %7.0f, %7.0f ) %7.0f %7.3f%% %7.3f%%",
              ((b == 0) ? 0.0 : bucket_limit[b - 1]),
              bucket_limit[b],
              hist_->buckets_[b],
              mult * hist_->buckets_[b],
              mult * sum);
    if (r_size < strlen(r) + strlen(buf)) {
      r = realloc(r, r_size * 2);
      r_size *= 2;
    }
    strcat(r, buf);

    /* Add hash marks based on percentage; 20 marks for 100%. */
    int marks = (int)(20 * (hist_->buckets_[b] / hist_->num_) + 0.5);
    if (r_size < strlen(r) + marks + 1) {
      r = realloc(r, r_size * 2);
      r_size *= 2;
    }
    for (int i = 0; i < marks; i++)
      strcat(r, "#");
    strcat(r, "\n");
  }
  return r;
}
