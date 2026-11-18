/* Engine miscellaneous data.
 * The six SDK string/typeinfo records, fsi_digest_key_ptr/len, __files and
 * the random_next generator state.
 * ROM 0x0209a624..0x0209a628, 4 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0209a624: 4 bytes. */
unsigned int random_next = 0x00000001u;
