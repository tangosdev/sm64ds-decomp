/* Engine miscellaneous data.
 * The six SDK string/typeinfo records, fsi_digest_key_ptr/len, __files and
 * the random_next generator state.
 * ROM 0x0209a084..0x0209a0a0, 28 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u16 data_0209a08c[10];

/* 0x0209a088: 4 bytes. */
int data_0209a088 = 0x00000006;

/* 0x0209a084: 4 bytes. */
unsigned short *data_0209a084 = (unsigned short *)&data_0209a08c;

/* 0x0209a08c: 20 bytes. */
u16 data_0209a08c[10] = {
    'm', 'u', 'l', 't', 'i', 'b', 'o', 'o', 't', 0x0000,
};
