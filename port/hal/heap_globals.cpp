// Host storage and linkage bridging for the heap subsystem.
//
// The NitroSystem heap registry -- NNS_FND_heapcommon.c's sRootList and
// sRootListInitialized -- is .bss the decomp TU only declares
// (src/engine/heap/NestedHeapIterator.cpp says why: no arm9 .bss range is
// enrolled for it). On the NDS the link places it at 0x020a4d34; on the host
// this file is the definition.
#include "nnsys/fnd.h"

extern "C" {
int sRootListInitialized;
NNSFndList sRootList;

// SDK asm primitive (stmia burst fill) -> plain word fill on host. The
// frontier classifies its TU as HAL-owned; this is the HAL half.
void MultiStore_Int(int val, int *dst, int len)
{
    for (int i = 0; i < len / 4; ++i)
        dst[i] = val;
}
}

// abs, which the expanded and frame heaps' allocatable-size queries call, is
// MSL's C abs (include/msl/stdlib.h): the host C library's own abs links in
// its place, so no forwarder is needed. Its src/ home, src/cstd4.cpp, carries
// hand-asm primitives no host compiler accepts.
