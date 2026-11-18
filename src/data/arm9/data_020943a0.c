/* Multiboot scene profile and RTTI.
 * g_profile_MULTIBOOT, the _ZTS/_ZTI records of dScMB_c and its graph
 * callback, and the 64-byte scene record that follows.
 * ROM 0x020943a0..0x020943c4, 36 bytes, 1 configured symbols.
 */
#include "types.h"

extern u32 _ZTI7dScMB_c[];

/* 0x020943a0: 36 bytes. */
u32 _ZTSN7dScMB_c15graphCallback_cE[9] = {
    0x5364374e, 0x5f424d63, 0x67353163, 0x68706172,        0x6c6c6143,
    0x6b636162, 0x0045635f, 0x00000000, (u32)_ZTI7dScMB_c,
};
