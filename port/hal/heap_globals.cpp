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
void MIi_CpuClear32(int val, int *dst, int len)
{
    for (int i = 0; i < len / 4; ++i)
        dst[i] = val;
}
}

// cstd::abs, which the expanded and frame heaps' allocatable-size queries
// call. The forwarder was in hal/gx_upload_bridge.cpp and moved here with
// the source file's move from slice_gate4b.txt to slice_gate2.txt, for the same
// reason: the nine targets that link gx_upload_bridge.cpp all link this file too,
// so the pair can have one home instead of one per layer that reaches it.
//
// The definition is the host's own, like hal/cstd_div.c's fdiv. cstd::abs now
// lives in src/cstd4.cpp, the runtime-library TU, which also owns hand-asm
// primitives (swp, dcd, the soft-float compare stubs) that no host compiler
// accepts -- so the slice can no longer compile the src/ definition. The body
// is the one src/cstd4.cpp carries.
namespace cstd {
int abs(int x)
{
    return x < 0 ? -x : x;
}
}
