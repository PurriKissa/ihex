# Intel HEX Parser (ihex)

A small, streaming Intel HEX lexer/parser in C — minimal, portable, and suitable for embedded systems or host tooling.  
It parses Intel HEX records byte-by-byte, validates checksums, supports Extended Linear Address (type 04) records, and dispatches parsed data bytes through a user-provided callback.

---

# Features
- Streaming byte-by-byte parser (suitable for serial/stream input)
- Handles Intel HEX record types:
  - `00` — Data Record
  - `01` — End Of File Record
  - `04` — Extended Linear Address Record (32-bit addressing)
- Checksum validation with error reporting
- Simple, callback-based API — you control how parsed data bytes are handled
- Compact and easy to integrate in embedded projects

---

# Table of contents
- ## Usage
- ## API
- ## Example
- ## Build
- ## Tests
- ## Design notes & implementation details

---

# Usage

1. Include the parser header in your project:
```c
#include "ihex.h"
```

2. Initialize a reader and supply a data callback:
```c
ihex_tReader reader;
ihex_Init(&reader, my_data_callback);
ihex_Begin(&reader);
```

3. Feed the parser one byte at a time (for example, from a file, UART, or memory):
```c
uint8_t b = ...;
ihex_tMessage msg = ihex_Put(&reader, b);

if (msg == IHEX_MESSAGE_CHECK_SUM_ERROR) {
    // handle checksum error
} else if (msg == IHEX_MESSAGE_END) {
    // reached EOF record
}
```

4. Implement a data callback to receive parsed data bytes and addresses:
```c
ihex_tMessage my_data_callback(uint32_t address, uint8_t data_byte)
{
    // store or process the byte at `address`
    // return IHEX_MESSAGE_CONTINUE or some other code if you want to stop
    flash_write_byte(address, data_byte);
    return IHEX_MESSAGE_CONTINUE;
}
```

---

# API

- `void ihex_Init(ihex_tReader* reader, ihex_tDataCallback func_data);`  
  Initialize the parser state and set the callback used when data bytes are parsed.

- `void ihex_Begin(ihex_tReader* reader);`  
  Prepare the parser for a new stream (resets internal state and extended address).

- `ihex_tMessage ihex_Put(ihex_tReader* reader, uint8_t data);`  
  Feed a single byte into the parser. Returns an `ihex_tMessage` (status) value:
  - `IHEX_MESSAGE_CONTINUE` — keep feeding bytes
  - `IHEX_MESSAGE_END` — parsed an End-Of-File record
  - `IHEX_MESSAGE_CHECK_SUM_ERROR` — checksum mismatch on the last record
  - `IHEX_MESSAGE_INVALID_INPUT_DATA` — invalid character in the stream

- `typedef ihex_tMessage (*ihex_tDataCallback)(uint32_t address, uint8_t data);`  
  The data callback signature (the names and exact typedef may differ slightly — consult `ihex.h`).

---

# Example — file-based loader

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ihex.h"

static ihex_tMessage my_data_callback(uint32_t address, uint8_t data_byte)
{
    printf("0x%08X: 0x%02X\n", address, data_byte);
    return IHEX_MESSAGE_CONTINUE;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.hex>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    ihex_tReader reader;
    ihex_Init(&reader, my_data_callback);
    ihex_Begin(&reader);

    int c;
    while ((c = fgetc(f)) != EOF) {
        ihex_tMessage msg = ihex_Put(&reader, (uint8_t)c);

        if (msg == IHEX_MESSAGE_INVALID_INPUT_DATA) {
            fprintf(stderr, "Invalid input character in HEX file\n");
            break;
        } else if (msg == IHEX_MESSAGE_CHECK_SUM_ERROR) {
            fprintf(stderr, "Checksum error on a record\n");
            break;
        } else if (msg == IHEX_MESSAGE_END) {
            break;
        }
    }

    fclose(f);
    return 0;
}
```

---

# Build

```bash
gcc -std=c11 -Wall -Wextra -O2 -o ihex_loader main.c ihex.c
```

---

# Tests

Test vectors example:
```
:020000040003F7
:100000000C945C000C946E000C946E000C946E00CA
:00000001FF
```

---

# Design notes & implementation details

- Parsing is nibble-based: ASCII hex chars `0-9`, `A-F`, `a-f` converted to nibbles.
- Explicit lexer state machine: colon, byte-count, address, type, data, checksum.
- Extended Linear Address (type 04) records combined into a 32-bit `ext_offset`.
- Checksum is Intel HEX style (sum of bytes, two's complement).
- Streaming-friendly: supports feeding arbitrarily sized chunks.
