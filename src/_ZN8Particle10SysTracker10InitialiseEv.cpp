//cpp
#include "types.h"
// @symbol _ZN8Particle10SysTracker10InitialiseEv
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Heap.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle__SysTracker.h"
extern "C" extern u32 _ZN8Particle7Texture12AllocTexVramEjb(const void*, u32);
extern "C" extern unsigned int _ZN8Particle7Texture12AllocPalVramEjb(unsigned int, unsigned int);
extern "C" void DecompressLZ16(const void* src, void* dst);


extern signed char data_0209f2f8;
extern int data_0209ee84;
extern int data_0209ee8c;
extern void* data_020a0ea0;

#define M(p) ((long long)(int)(p))

extern "C" void _ZN8Particle10SysTracker10InitialiseEv(struct Particle__SysTracker *self) {
    signed char v = data_0209f2f8;
    unsigned int allocSize = 0x8c00;
    int countA = 0x28;
    int countB = 0x100;

    if (v == 0x24 || v == 0x26 || v == 0x28) {
        countA = 0x40;
        countB = 0x140;
        allocSize = 0xa800;
    }

    {
        char* base = (char*)_ZN6Memory13operator_new2Ej(allocSize);
        char* end = base + allocSize;
        data_0209ee80 = base;
        data_0209ee78 = base;
        data_0209ee7c = end;
    }

    data_0209ee88 = func_02045d10();
    data_0209ee84 = func_02045cf0();
    data_0209ee8c = func_02045ce0();

    self->mManager = func_0204a4c8(func_02023178, countA, countB, 0x1a, 0x3e);
    *(int*)((char*)self->mManager + 0x30) = 0x8000;

    if (func_0206e28c((u8*)data_02075f14, data_0208f668, 4) != 0) {
        self->mResourceFile = data_02075f14;
    } else {
        char* hdr = (char*)(int)M(data_02075f14);
        unsigned int size = (unsigned int)M(*(unsigned int*)(hdr + 4) >> 8);
        void* dst = _ZN6Memory8AllocateEj(size);
        DecompressLZ16(hdr + 4, dst);
        _ZN4CP1514FlushDataCacheEjj((unsigned int)dst, size);
        self->mResourceFile = dst;
    }

    func_0204a17c(self->mManager, self->mResourceFile);
    func_0204a0dc(self->mManager, _ZN8Particle7Texture12AllocTexVramEjb);
    func_0204a028(self->mManager, _ZN8Particle7Texture12AllocPalVramEjb);

    if (self->mResourceFile != data_02075f14) {
        void* heap = (void*)(int)M(data_020a0ea0);
        unsigned int oldSize = (unsigned int)M(*(unsigned int*)((char*)self->mResourceFile + 0x18));
        _ZN4Heap7_SizeofEPv(heap, self->mResourceFile);
        _ZN4Heap10ReallocateEPvj(heap, self->mResourceFile, oldSize);
    }
}
