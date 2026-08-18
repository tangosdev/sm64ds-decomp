//cpp
/* dScene_c::GraphCallback0() at 0x02018eb8, 8 bytes: `mov r0,#1; bx lr`.
 *
 * One of four scene-graph traversal hooks (GraphCallback0..3, 0x02018ea0..0x02018ec0)
 * that are byte-for-byte identical and do nothing but report success. dScene_c's own
 * code starts at 0x0202e140; these four sit 0x15288 bytes -- 85 KiB -- below it,
 * with the rest of the traversal machinery they are called from.
 *
 * Static, and this is the one thing about them the bytes alone cannot say -- a
 * member that ignores `this` compiles identically either way. The call sites decide
 * it: the four addresses are stored in a table of plain function pointers, and a
 * pointer-to-member is not a plain address. */
#include "dScene_c.h"

int dScene_c::GraphCallback0()
{
    return 1;
}
