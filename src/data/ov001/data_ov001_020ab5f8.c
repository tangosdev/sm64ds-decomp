/*
 * Title-overlay resource descriptors.
 *
 * Two 0x10-byte records at 0x020ab620/0x020ab630 name a payload and its size (0x70
 * and 0x134); the 8-byte records at 0x020ab610/0x020ab618 hold the same shape with
 * the payload pointer only. The payloads themselves are the two blobs that follow.
 * The consumers are the early-boot menu code, so no member names are recovered
 * here; the words are the cartridge's.
 */
#include "types.h"

extern const u32 data_ov001_020ab5f8 = 0x00000000;
