#include "msl/string.h"

/* "LZ77", the tag in front of an LZ-compressed game file. */
extern int data_0208edf8[];

/* Whether the file bytes at a open with the "LZ77" tag (memcmp of the
   first 4 bytes). The loaders then read the LZ header at
   a + 4 (func_02018ac4) and decompress from there. The file address is an
   int here, as most of the callers declare it. */
int func_02018568(int a) {
    return memcmp((const void *)a, data_0208edf8, 4) == 0;
}
