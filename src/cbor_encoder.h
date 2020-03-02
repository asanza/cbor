/*
 * File: cbor_encoder.h
 * 
 * Copyright (c) 2020 Diego Asanza <f.asanza@gmail.com>
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CBOR_ENCODER_H
#define _CBOR_ENCODER_H

#include <stdint.h>

typedef int (*__write_byte_t)(uint8_t byte, void* priv);


struct stream_out {
  __write_byte_t fn;
  void* priv;
};

/* type 0 */
int cbor_encoder_uint8(const struct stream_out* f, uint8_t val);
int cbor_encoder_uint16(const struct stream_out* f, uint16_t val);
int cbor_encoder_uint32(const struct stream_out* f, uint32_t val);
int cbor_encoder_uint64(const struct stream_out* f, uint64_t val);

/* type 1 */
int cbor_encoder_int8(const struct stream_out* f, int8_t val);
int cbor_encoder_int16(const struct stream_out* f, int16_t val);
int cbor_encoder_int32(const struct stream_out* f, int32_t val);
int cbor_encoder_int64(const struct stream_out* f, int64_t val);

/* type 2 */
int cbor_encoder_bytearray(const struct stream_out* f,  const char* bytearray);

/* type 3 */
int cbor_encoder_utf8(const struct stream_out* f,  const char* utf8_str);

/* type 4 */
int cbor_encoder_array(const struct stream_out* f, uint32_t len);

int cbor_encoder_map(const struct stream_out* f,  uint32_t len);

int cbor_encoder_float(const struct stream_out* f, float val);


#endif // _CBOR_ENCODER_H
