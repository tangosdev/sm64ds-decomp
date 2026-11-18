/* Engine miscellaneous data.
 * The six SDK string/typeinfo records, fsi_digest_key_ptr/len, __files and
 * the random_next generator state.
 * ROM 0x0209a330..0x0209a414, 228 bytes, 2 configured symbols.
 *
 * Unclaimed, all zero, left to the linker gap: data_0209a414
 * (0x0209a414, 4 bytes).
 */
#include "types.h"
#include "file_struc.h"

/* 6 word(s) here are addresses with no configured symbol name,
   so they stay literal rather than naming a wrong target. */

extern int __read_console();
extern int __write_console();
extern int __close_console();

/* 0x0209a330: 228 bytes. */
FILE __files[3] = {{0x0,
                    {0, 1, 1, 2, 0, 0},
                    {0x0, 0x0, 0x0, 0x0},
                    0x0,
                    0x0,
                    0x0,
                    {0x0, 0x0},
                    0x0,
                    (unsigned char *)0x020aa2d4,
                    0x100,
                    (unsigned char *)0x020aa2d4,
                    0x0,
                    0x0,
                    0x0,
                    0x0,
                    0,
                    &__read_console,
                    &__write_console,
                    &__close_console,
                    0},
                   {0x1,
                    {0, 2, 1, 2, 0, 0},
                    {0x0, 0x0, 0x0, 0x0},
                    0x0,
                    0x0,
                    0x0,
                    {0x0, 0x0},
                    0x0,
                    (unsigned char *)0x020aa1d4,
                    0x100,
                    (unsigned char *)0x020aa1d4,
                    0x0,
                    0x0,
                    0x0,
                    0x0,
                    0,
                    &__read_console,
                    &__write_console,
                    &__close_console,
                    0},
                   {0x2,
                    {0, 2, 0, 2, 0, 0},
                    {0x0, 0x0, 0x0, 0x0},
                    0x0,
                    0x0,
                    0x0,
                    {0x0, 0x0},
                    0x0,
                    (unsigned char *)0x020aa0d4,
                    0x100,
                    (unsigned char *)0x020aa0d4,
                    0x0,
                    0x0,
                    0x0,
                    0x0,
                    0,
                    &__read_console,
                    &__write_console,
                    &__close_console,
                    0}};
