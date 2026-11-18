/* Engine miscellaneous data.
 * The six SDK string/typeinfo records, fsi_digest_key_ptr/len, __files and
 * the random_next generator state.
 * ROM 0x0209a5fc..0x0209a614, 24 bytes, 4 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0209a604: 8 bytes. */
char data_0209a604[8] = "-NAN";

/* 0x0209a600: 4 bytes. */
char data_0209a600[4] = "inf";

/* 0x0209a5fc: 4 bytes. */
char data_0209a5fc[4] = "INF";

/* 0x0209a60c: 8 bytes. */
char data_0209a60c[8] = "-nan";
