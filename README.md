# CBOR Encoder

This repository contains a very simple cbor encoder for use in 
embedded systems.

The main difference with other implementations, is that it does not
necessarily need a encoder buffer, as it can encode incomming
data on the fly directly to an output stream (i.e. uart)

## Usage

The actual code is located in the src repository.

Example, encoding the following json string and write to the serial
port.
```json
{"temp": [23.85941505432129, 2.8317720890045166], "units": "°C"}
```

```c
#include <cbor_encoder.h>
#include <stdint.h>

/* user provides the uart_writechar(char c) function */

static int write_byte(uint8_t c, void* priv) {
  (void*) priv;
  uart_writechar((char) c);
  /* return -1 if error here */
  return 0;
}

static struct stream_out f = {
  .fn = write_byte,
  .priv = NULL,
};

void main( void )
{
  cbor_encoder_map(&f, 2);
  cbor_encoder_utf8(&f, "temp");
  cbor_encoder_array(&f, 2);
  cbor_encoder_float(&f, 23.85941505432129);
  cbor_encoder_float(&f, 2.8317720890045166);
  cbor_encoder_utf8(&f, "units");
  cbor_encoder_utf8(&f, "°C");
}

```

## License

                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/