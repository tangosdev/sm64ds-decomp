//cpp
// @symbol _ZNK7PathPtr7GetNodeER7Vector3j
/* Read node `idx' of this path out of the level's node table as a Fix12
   position. The table is one flat array of 6-byte (three s16) records shared by
   every path, and def->firstNode says where this path's run begins.

   The <<12 is the s16-to-Fix12 widening, spelled as a shift because that is what
   the ROM's three `mov rN, rN, lsl #12' are -- a Fix12 construction here costs
   bytes. */
#include "PathPtr.h"

extern "C" int func_0203acbc(void); /* the node table's base */

void PathPtr::GetNode(Vector3 &node, unsigned int idx) const
{
    char *base = (char *)func_0203acbc();
    u16 first = def->firstNode;
    char *row = base + first * 6;
    int off = idx * 6;
    node.x = ((int)*(s16 *)(row + off)) << 12;
    node.y = ((int)*(s16 *)(row + off + 2)) << 12;
    node.z = ((int)*(s16 *)(row + off + 4)) << 12;
}
