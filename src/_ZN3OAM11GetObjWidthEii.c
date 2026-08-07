//cpp
// @symbol _ZN3OAM11GetObjWidthEii
#include "decl_common.h"
#include "OAM.h"

/* OAM::GetObjWidth(int shape, int sizeBits) at 0x02020980 -- static, no `this`.
 *
 * The hardware sprite dimension table: OBJ shape (square / wide / tall) and size
 * bits together pick one of twelve widths. `sizeBits` is a BYTE offset into the
 * table, not an index -- it is added to the base before the `[shape]` index is
 * applied, which is why the cast chain reads `(int*)((char*)base + sizeBits)`.
 *
 * The double cast is load-bearing: the table is stride-4 but each entry is read as
 * a single byte, so narrowing the load to u8 after indexing as int* is what the ROM
 * does. GetObjHeight is the same shape over the height table 12 bytes further on.
 */
u8 OAM::GetObjWidth(int shape, int sizeBits)
{
    return *(unsigned char *)&((int *)((char *)data_020755a0 + sizeBits))[shape];
}
