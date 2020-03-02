/*
 * File: test_cbor_encoder.c
 * 
 * Copyright (c) 2020 Diego Asanza <f.asanza@gmail.com>
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cbor_encoder.h"
#include "unity.h"
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

struct fout {
  int n;
  uint8_t buffer[255];
};

static struct fout fout;

static int out_fn(uint8_t c, void *arg) {
  struct fout *t = (struct fout *)arg;
  t->buffer[t->n++] = c;
}

static struct stream_out f = {
  .fn = out_fn,
  .priv = &fout,
};


void test_cbor_encoder_test_map(void) {
  memset(f.priv, 0, sizeof(fout));

  cbor_encoder_map(&f, 2);
  cbor_encoder_utf8(&f, "a");
  cbor_encoder_uint8(&f, 1);
  cbor_encoder_utf8(&f, "b");
  cbor_encoder_array(&f, 2);
  cbor_encoder_uint32(&f, 2);
  cbor_encoder_uint32(&f, 3);

  TEST_ASSERT_EQUAL_MEMORY_ARRAY("\xA2\x61\x61\x01\x61\x62\x82\x02\x03",
                                 fout.buffer, 1, 9);
}

void test_cbor_encoder_test_map_neg(void) {
  memset(f.priv, 0, sizeof(fout));

  cbor_encoder_map(&f, 2);
  cbor_encoder_utf8(&f, "a");
  cbor_encoder_int16(&f, -257);
  cbor_encoder_utf8(&f, "b");
  cbor_encoder_array(&f, 2);
  cbor_encoder_int32(&f, -323344233);
  cbor_encoder_int32(&f, 323432423);

  TEST_ASSERT_EQUAL_MEMORY_ARRAY("\xA2\x61\x61\x39\x01\x00\x61\x62\x82\x3A\x13"
                                 "\x45\xD7\x68\x1A\x13\x47\x2F\xE7",
                                 fout.buffer, 1, 19);
}

void test_cbor_encoder_array(void) {
  memset(f.priv, 0, sizeof(fout));

  cbor_encoder_array(&f, 23);
  for (int i = 0; i < 23; i++)
    cbor_encoder_int32(&f, i);
  TEST_ASSERT_EQUAL_MEMORY_ARRAY(
      "\x97\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
      "\x11\x12\x13\x14\x15\x16",
      fout.buffer, 1, 24);
}

void test_cbor_encoder_float(void) {
  memset(f.priv, 0, sizeof(fout));

  cbor_encoder_float(&f, 1.890425205230713);
  TEST_ASSERT_EQUAL_MEMORY_ARRAY("\xFA\x3F\xF1\xF9\x74", fout.buffer, 1, 5);
}