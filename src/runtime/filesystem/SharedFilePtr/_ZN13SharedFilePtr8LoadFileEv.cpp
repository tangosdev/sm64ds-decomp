//cpp
// @symbol _ZN13SharedFilePtr8LoadFileEv
/* SharedFilePtr::LoadFile -- takes one reference and hands back the file. Loads
 * on the first taker, refuses at 255 holders rather than wrapping the count, and
 * publishes the file ID to the global at 0x0209d3bc the way every entry point
 * here does.
 *
 * Fields stay raw offsets for the reason src/_ZN13SharedFilePtr7ReleaseEv.cpp
 * gives at length: the layout is not recovered and the tree's local declarations
 * of this struct disagree, so include/SharedFilePtr.h deliberately declares no
 * members.
 *
 *   +0x0  u16  fileID
 *   +0x2  u8   numRefs
 *   +0x4  void *filePtr */
#include "SharedFilePtr.h"

extern "C" {
extern unsigned int data_0209d3bc;
int _ZN13SharedFilePtr4LoadEv(SharedFilePtr *self);
}

void *SharedFilePtr::LoadFile()
{
    char *self = (char *)this;

    data_0209d3bc = *(unsigned short *)self;

    if (*(unsigned char *)(self + 2) == 0) {
        if (!_ZN13SharedFilePtr4LoadEv(this))
            return 0;
    }

    if (*(unsigned char *)(self + 2) >= 0xff) {
        return 0;
    }

    *(unsigned char *)(self + 2) += 1;
    return *(void **)(self + 4);
}
