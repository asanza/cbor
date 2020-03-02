/*
 * File: cbor_encoder.c
 * 
 * Copyright (c) 2020 Diego Asanza <f.asanza@gmail.com>
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cbor_encoder.h"
#include <assert.h>
#include <string.h>

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
  #error "for big endians, __outbytes must be changed"
#endif

/**
 * @brief Output a byte array to the output stream
 * 
 * @param f  the output stream
 * @param bytes byte array to send
 * @param len  length of the byte array
 * @return int 
 */
static int __outbytes(const struct stream_out* f, const uint8_t* bytes, int len)
{
  bytes = bytes + len;
  while( len-- )
  {
    bytes--;
    f->fn(*bytes, f->priv);
  }
  return 0;
}

/**
 * @brief output the encoded value to the output stream
 * 
 * @param f  the output stream 
 * @param val  value to be encoded.
 * @return int 
 */
int cbor_encoder_uint8(const struct stream_out* f, uint8_t val)
{
  if( val > 23 )
  {
    f->fn(0x18, f->priv);
    f->fn(0x18, f->priv);
    f->fn(val, f->priv);
  } else {
    f->fn(val, f->priv);
  }

  return 0;
}

int cbor_encoder_uint16(const struct stream_out* f, uint16_t val)
{
  if( val <= 0xFF )
  {
    cbor_encoder_uint8(f, (uint8_t) val);
  } else {
    f->fn(0x19, f->priv);
    __outbytes(f, (uint8_t*) &val, sizeof(uint16_t));
  }
  return 0;
}

int cbor_encoder_uint32(const struct stream_out* f, uint32_t val)
{
  if ( val <= 0xFFFF )
  {
    cbor_encoder_uint16(f, (uint16_t) val);
  } else {
    f->fn(0x1a, f->priv);
    __outbytes(f, (uint8_t*) &val, sizeof(uint32_t));
  }
  return 0;
}

int cbor_encoder_uint64(const struct stream_out* f, uint64_t val)
{
  if( val <= 0xFFFFFFFF )
  {
    cbor_encoder_int32(f, (uint32_t) val);
  } else {
    f->fn(0x1b, f->priv);
    __outbytes(f, (uint8_t*) &val, sizeof(uint64_t));    
  }
  return 0;
}

int cbor_encoder_int8(const struct stream_out* f, int8_t val )
{
  uint8_t ui = val >> 7;
  uint8_t mt = ui & 0x20;
  ui ^= val;
  if (ui < 24)
    f->fn(mt + ui, f->priv);
  else {
    f->fn( mt + 24, f->priv );
    f->fn(ui, f->priv);
  }
  return 0;
}

int cbor_encoder_int16(const struct stream_out* f, int16_t val )
{

  uint16_t ui = val >> 15;
  uint16_t mt = ui & 0x20;
  ui ^= val;
 
  if( ui <= 0xFFU ) {
    return cbor_encoder_int8(f, val);
  }

  f->fn( mt + 25, f->priv );
  __outbytes(f, (uint8_t*) &ui, sizeof(int16_t));

  return 0;
}

int cbor_encoder_int32(const struct stream_out* f, int32_t val )
{
  uint32_t ui = val >> 31;
  uint32_t mt = ui & 0x20;
  ui ^= val;
 
  if( ui <= 0xFFFFU ) {
    return cbor_encoder_int16(f, val);
  }

  f->fn( mt + 26, f->priv );
  __outbytes(f, (uint8_t*) &ui, sizeof(int32_t));
  return(0);
}

int cbor_encoder_int64(const struct stream_out* f, int64_t val )
{
  uint64_t ui = val >> 63;
  uint64_t mt = ui & 0x20;
  ui ^= val;
 
  if( ui <= 0xFFFFFFFFU ) {
    return cbor_encoder_int32(f, val);
  }

  f->fn( mt + 27, f->priv );
  __outbytes(f, (uint8_t*) &ui, sizeof(int64_t));
  return(0);
}

int cbor_encoder_bytearray(const struct stream_out* f, const char* bytearray)
{
  uint32_t n;
  n = strlen(bytearray);
  if ( n <= 0x17 )
  {
    f->fn(0x40 + n, f->priv);
  } else if ( n <= 0xFF ) {
    f->fn(0x58, f->priv);
    f->fn((uint8_t) n, f->priv);
  } else if ( n <= 0xFFFF ) { 
    f->fn(0x59, f->priv);
    __outbytes(f, ((uint8_t*) &n + 2), sizeof(uint16_t));
  } else {
    assert(0);
  };
  __outbytes(f, bytearray, n);
  return 0;
}

int cbor_encoder_array(const struct stream_out* f, uint32_t len)
{
  if( len <= 0x17 )
  {
    f->fn( 0x80 + (uint8_t) len, f->priv );
  } else if( len <= 0xFF ) {
    f->fn( 0x98, f->priv );
    f->fn( (uint8_t) len, f->priv );
  } else if ( len <= 0xFFFF ) {
    f->fn( 0x99, f->priv );
    __outbytes(f,  (uint8_t*) &len, sizeof(uint16_t) );
  } else {
    f->fn( 0x9a, f->priv );
    __outbytes(f,  (uint8_t*) &len, sizeof(uint32_t) );
  }
  return 0;
}

int cbor_encoder_map(const struct stream_out* f, uint32_t len)
{
  if( len <= 0x17 )
  {
    f->fn( 0xa0 + (uint8_t) len, f->priv );
  } else {
    assert(0);
  }
  return 0;
}

int cbor_encoder_utf8(const struct stream_out* f, const char* str)
{
  uint32_t n;
  n = strlen(str);
  if ( n <= 0x17 )
  {
    f->fn(0x60 + n, f->priv);
  } else if ( n <= 0xFF ) {
    f->fn(0x78, f->priv);
    f->fn((uint8_t) n, f->priv);
  } else if ( n <= 0xFFFF ) { 
    f->fn(0x79, f->priv);
    __outbytes(f, ((uint8_t*) &n + 2), sizeof(uint16_t));
  } else {
    assert(0);
  };
  __outbytes(f, str, n);
  return 0;
} 

int cbor_encoder_float(const struct stream_out* f, float val )
{
  f->fn(0xfa, f->priv);
  __outbytes(f, (uint8_t*) &val, sizeof(float));
}