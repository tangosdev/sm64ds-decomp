#include "types.h"
/* SharedFilePtr::Construct(u32 ov0FileID) at 0x0201799c
 * Declared in SharedFilePtr.h as `SharedFilePtr& Construct(u32 ov0FileID);`.
 * Saves `this`, forwards to func_02017e48 (the real init helper -- it is still
 * an unnamed func_ symbol in symbols.txt at 0x02017e48), then returns `this`
 * so the reference return type is satisfied.
 *
 * The callee receives `this` in r0 (it is preserved across the call), matching
 * the `SharedFilePtr*`-first signature below.
 */
struct SharedFilePtr; /* { u16 fileID; u8 numRefs; char* filePtr; } -- see SharedFilePtr.h */

extern void func_02017e48(struct SharedFilePtr* self); /* 0x02017e48 */

struct SharedFilePtr* _ZN13SharedFilePtr9ConstructEj(struct SharedFilePtr* self, u32 ov0FileID)
{
    func_02017e48(self);
    return self;
}
