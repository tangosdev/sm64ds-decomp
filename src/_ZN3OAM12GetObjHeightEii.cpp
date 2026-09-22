//cpp
// @symbol _ZN3OAM12GetObjHeightEii
#include "decl_common.h"
#include "OAM.h"

/* OAM::GetObjHeight(int shape, int sizeBits) at 0x0202096c -- static, no `this`.
 *
 * GetObjWidth's twin over the height table at 0x020755ac -- 12 bytes past the width
 * table, i.e. one row of four entries. Same indexing: `sizeBits` is a byte offset
 * added before the `[shape]` index, and the entry is read as a single byte out of a
 * stride-4 slot.
 */
u8 OAM::GetObjHeight(int shape, int sizeBits)
{
    return *(unsigned char *)&((int *)((char *)data_020755ac + sizeBits))[shape];
}
