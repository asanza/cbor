#include "cbor_encoder.h"
#include "unity.h"
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

struct fout {
  int n;
  uint8_t buffer[255];
};

static int out_fn(uint8_t c, void *arg) {
  struct fout *t = (struct fout *)arg;
  t->buffer[t->n++] = c;
}

void test_cbor_encoder_test_map(void) {
  struct fout fout;
  memset(&fout, 0, sizeof(fout));

  cbor_encoder_init(out_fn, &fout);
  cbor_encoder_map(2);
  cbor_encoder_utf8("a");
  cbor_encoder_uint8(1);
  cbor_encoder_utf8("b");
  cbor_encoder_array(2);
  cbor_encoder_uint32(2);
  cbor_encoder_uint32(3);

  TEST_ASSERT_EQUAL_MEMORY_ARRAY("\xA2\x61\x61\x01\x61\x62\x82\x02\x03",
                                 fout.buffer, 1, 9);
}

void test_cbor_encoder_test_map_neg(void) {
  struct fout fout;
  memset(&fout, 0, sizeof(fout));

  cbor_encoder_init(out_fn, &fout);
  cbor_encoder_map(2);
  cbor_encoder_utf8("a");
  cbor_encoder_int16(-257);
  cbor_encoder_utf8("b");
  cbor_encoder_array(2);
  cbor_encoder_int32(-323344233);
  cbor_encoder_int32(323432423);

  TEST_ASSERT_EQUAL_MEMORY_ARRAY("\xA2\x61\x61\x39\x01\x00\x61\x62\x82\x3A\x13"
                                 "\x45\xD7\x68\x1A\x13\x47\x2F\xE7",
                                 fout.buffer, 1, 19);
}

void test_cbor_encoder_array(void) {
  struct fout fout;
  memset(&fout, 0, sizeof(fout));

  cbor_encoder_array(23);
  for (int i = 0; i < 23; i++)
    cbor_encoder_int32(i);
  TEST_ASSERT_EQUAL_MEMORY_ARRAY(
      "\x97\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
      "\x11\x12\x13\x14\x15\x16",
      fout.buffer, 1, 24);
}

void test_cbor_encoder_float(void) {
  struct fout fout;
  memset(&fout, 0, sizeof(fout));

  cbor_encoder_float(1.890425205230713);
  TEST_ASSERT_EQUAL_MEMORY_ARRAY("\xFA\x3F\xF1\xF9\x74", fout.buffer, 1, 5);
}