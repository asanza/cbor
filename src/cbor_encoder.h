#ifndef _CBOR_ENCODER_H
#define _CBOR_ENCODER_H

#include <stdint.h>

typedef int (*__write_byte_t)(uint8_t byte, void* priv);

int cbor_encoder_init(__write_byte_t fn, void* priv);

/* type 0 */
int cbor_encoder_uint8(uint8_t val);
int cbor_encoder_uint16(uint16_t val);
int cbor_encoder_uint32(uint32_t val);
int cbor_encoder_uint64(uint64_t val);

/* type 1 */
int cbor_encoder_int8(int8_t val);
int cbor_encoder_int16(int16_t val);
int cbor_encoder_int32(int32_t val);
int cbor_encoder_int64(int64_t val);

/* type 2 */
int cbor_encoder_bytearray(const char* bytearray);

/* type 3 */
int cbor_encoder_utf8(const char* utf8_str);

/* type 4 */
int cbor_encoder_array(uint32_t len);

int cbor_encoder_map( uint32_t len );

int cbor_encoder_map();

int cbor_encoder_float( void );


#endif // _CBOR_ENCODER_H
