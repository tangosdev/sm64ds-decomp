/* Engine miscellaneous data.
 * The six SDK string/typeinfo records, fsi_digest_key_ptr/len, __files and
 * the random_next generator state.
 * ROM 0x0209a418..0x0209a470, 88 bytes, 8 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 data_0209a424[2];
extern u32 data_0209a460[4];
extern int __mbtowc_noconv();
extern int __wctomb_noconv();
extern u32 data_0209a470[];
extern u8 data_0209a4e8[];

/* 0x0209a450: 16 bytes. */
char data_0209a450[16] = "%a %b %e %T %Y";

/* 0x0209a444: 12 bytes. */
char data_0209a444[12] = "%I:%M:%S %p";

/* 0x0209a42c: 12 bytes. */
char data_0209a42c[12] = "%m/%d/%y";

/* 0x0209a438: 12 bytes. */
void *data_0209a438[3] = {
    (void *)&data_0209a470,
    (void *)&data_0209a460,
    (void *)&data_0209a424,
};

/* 0x0209a424: 8 bytes. */
u32 data_0209a424[2] = {
    (u32)&__mbtowc_noconv,
    (u32)&__wctomb_noconv,
};

/* 0x0209a41c: 8 bytes. */
char data_0209a41c[8] = "AM|PM";

/* 0x0209a418: 4 bytes. */
char data_0209a418[4] = "%T";

/* 0x0209a460: 16 bytes. */
u32 data_0209a460[4] = {
    0x00000041,
    0x0000003a,
    0x00000000,
    (u32)data_0209a4e8,
};
