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

static __write_byte_t __outchar_fn;
static void* priv;

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
  #error "for big endians, __outbytes must be changed"
#endif

static int __outbytes(const uint8_t* bytes, int len)
{
  bytes = bytes + len;
  while( len-- )
  {
    bytes--;
    __outchar_fn(*bytes, priv);
  }
  return 0;
}

int cbor_encoder_init(__write_byte_t fn, void* _priv)
{
  __outchar_fn = fn;
  priv = _priv;
  return 0;
}

int cbor_encoder_uint8(uint8_t val)
{
  if( val > 23 )
  {
    __outchar_fn(0x18, priv);
    __outchar_fn(val, priv);
  } else {
    __outchar_fn(val, priv);
  }

  return 0;
}

int cbor_encoder_uint16(uint16_t val)
{
  if( val <= 0xFF )
  {
    cbor_encoder_uint8((uint8_t) val);
  } else {
    __outchar_fn(0x19, priv);
    __outbytes((uint8_t*) &val, sizeof(uint16_t));
  }
  return 0;
}

int cbor_encoder_uint32(uint32_t val)
{
  if ( val <= 0xFFFF )
  {
    cbor_encoder_uint16((uint16_t) val);
  } else {
    __outchar_fn(0x1a, priv);
    __outbytes((uint8_t*) &val, sizeof(uint32_t));
  }
  return 0;
}

int cbor_encoder_uint64(uint64_t val)
{
  if( val <= 0xFFFFFFFF )
  {
    cbor_encoder_int32((uint32_t) val);
  } else {
    __outchar_fn(0x1b, priv);
    __outbytes((uint8_t*) &val, sizeof(uint64_t));    
  }
  return 0;
}

int cbor_encoder_int8( int8_t val )
{
  uint8_t ui = val >> 7;
  uint8_t mt = ui & 0x20;
  ui ^= val;
  if (ui < 24)
    __outchar_fn(mt + ui, priv);
  else {
    __outchar_fn( mt + 24, priv );
    __outchar_fn(ui, priv);
  }
  return 0;
}

int cbor_encoder_int16( int16_t val )
{

  uint16_t ui = val >> 15;
  uint16_t mt = ui & 0x20;
  ui ^= val;
 
  if( ui <= 0xFFU ) {
    return cbor_encoder_int8(val);
  }

  __outchar_fn( mt + 25, priv );
  __outbytes((uint8_t*) &ui, sizeof(int16_t));

  return 0;
}

int cbor_encoder_int32( int32_t val )
{
  uint32_t ui = val >> 31;
  uint32_t mt = ui & 0x20;
  ui ^= val;
 
  if( ui <= 0xFFFFU ) {
    return cbor_encoder_int16(val);
  }

  __outchar_fn( mt + 26, priv );
  __outbytes((uint8_t*) &ui, sizeof(int32_t));
  return(0);
}

int cbor_encoder_int64( int64_t val )
{
  uint64_t ui = val >> 63;
  uint64_t mt = ui & 0x20;
  ui ^= val;
 
  if( ui <= 0xFFFFFFFFU ) {
    return cbor_encoder_int32(val);
  }

  __outchar_fn( mt + 27, priv );
  __outbytes((uint8_t*) &ui, sizeof(int64_t));
  return(0);
}

int cbor_encoder_bytearray(const char* bytearray)
{
  uint32_t n;
  n = strlen(bytearray);
  if ( n <= 0x17 )
  {
    __outchar_fn(0x40 + n, priv);
  } else if ( n <= 0xFF ) {
    __outchar_fn(0x58, priv);
    __outchar_fn((uint8_t) n, priv);
  } else if ( n <= 0xFFFF ) { 
    __outchar_fn(0x59, priv);
    __outbytes(((uint8_t*) &n + 2), sizeof(uint16_t));
  } else {
    assert(0);
  };
  __outbytes(bytearray, n);
  return 0;
}

int cbor_encoder_array(uint32_t len)
{
  if( len <= 0x17 )
  {
    __outchar_fn( 0x80 + (uint8_t) len, priv );
  } else if( len <= 0xFF ) {
    __outchar_fn( 0x98, priv );
    __outchar_fn( (uint8_t) len, priv );
  } else if ( len <= 0xFFFF ) {
    __outchar_fn( 0x99, priv );
    __outbytes( (uint8_t*) &len, sizeof(uint16_t) );
  } else {
    __outchar_fn( 0x9a, priv );
    __outbytes( (uint8_t*) &len, sizeof(uint32_t) );
  }
  return 0;
}

int cbor_encoder_map(uint32_t len)
{
  if( len <= 0x17 )
  {
    __outchar_fn( 0xa0 + (uint8_t) len, priv );
  } else {
    assert(0);
  }
  return 0;
}

int cbor_encoder_utf8(const char* str)
{
  uint32_t n;
  n = strlen(str);
  if ( n <= 0x17 )
  {
    __outchar_fn(0x60 + n, priv);
  } else if ( n <= 0xFF ) {
    __outchar_fn(0x78, priv);
    __outchar_fn((uint8_t) n, priv);
  } else if ( n <= 0xFFFF ) { 
    __outchar_fn(0x79, priv);
    __outbytes(((uint8_t*) &n + 2), sizeof(uint16_t));
  } else {
    assert(0);
  };
  __outbytes(str, n);
  return 0;
} 

int cbor_encoder_float( float val )
{
  __outchar_fn(0xfa, priv);
  __outbytes((uint8_t*) &val, sizeof(float));
}