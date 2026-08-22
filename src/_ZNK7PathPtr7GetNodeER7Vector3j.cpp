//cpp
// @symbol _ZNK7PathPtr7GetNodeER7Vector3j
/* PathPtr::GetNode(Vector3 &vec_, u32 idx) const -- read node `idx' of this path
 * out of the level's path table as a Fix12 position.
 *
 * The table is one flat array of 6-byte (three s16) records shared by every
 * path; func_0203acbc returns its base and unk_000 points at this path's own
 * header, whose first halfword is the index of its first record. Nothing
 * matched has typed that header, so the two casts stay raw.
 *
 * The <<12 is the s16-to-Fix12 widening, written as a shift rather than a Fix12
 * construction because that is what the ROM's three `mov rN, rN, lsl #12' are. */
#include "PathPtr.h"

extern "C" int func_0203acbc(void);

void PathPtr::GetNode(Vector3 &vec_, unsigned int idx) const
{
    char *base = (char *)func_0203acbc();
    u16 first = *(u16 *)unk_000;
    char *row = base + first * 6;
    int off = idx * 6;
    vec_.x = ((int)*(s16 *)(row + off)) << 12;
    vec_.y = ((int)*(s16 *)(row + off + 2)) << 12;
    vec_.z = ((int)*(s16 *)(row + off + 4)) << 12;
}
