//cpp
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" void* _ZN6Memory13operator_new2Ej(unsigned int sz);
extern "C" int func_02045d10(void);
extern "C" int func_02045cf0(void);
extern "C" int func_02045ce0(void);
extern "C" char* func_0204a4c8(char* (*alloc)(int), int a, int b, int c, unsigned short arg5);
extern "C" char* func_02023178(int sz);
extern "C" int func_0206e28c(const u8* a, const u8* b, u32 n);
extern "C" void* _ZN6Memory8AllocateEj(unsigned int sz);
extern "C" void DecompressLZ16(const void* src, void* dst);
extern "C" void _ZN4CP1514FlushDataCacheEjj(unsigned int addr, unsigned int size);
extern "C" void func_0204a17c(void* a, void* b);
extern "C" int func_0204a0dc(void* thiz, u32 (*cb)(const void*, u32));
extern "C" int func_0204a028(void* thiz, unsigned int (*cb)(unsigned int, unsigned int));
extern "C" u32 _ZN4Heap7_SizeofEPv(void* heap, void* p);
extern "C" void _ZN4Heap10ReallocateEPvj(void* heap, void* p, unsigned int sz);

extern "C" u32 _ZN8Particle7Texture12AllocTexVramEjb(const void*, u32);
extern "C" unsigned int _ZN8Particle7Texture12AllocPalVramEjb(unsigned int, unsigned int);

extern signed char data_0209f2f8;
extern void* data_0209ee80;
extern void* data_0209ee78;
extern void* data_0209ee7c;
extern int data_0209ee88;
extern int data_0209ee84;
extern int data_0209ee8c;
extern char data_02075f14[];
extern unsigned char data_0208f668[];
extern void* data_020a0ea0;

#define M(p) ((long long)(int)(p) & 0xffffffffffffffffLL)

extern "C" void _ZN8Particle10SysTracker10InitialiseEv(char* self)
{
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

    *(char**)(self + 4) = func_0204a4c8(func_02023178, countA, countB, 0x1a, 0x3e);
    *(int*)(*(char**)(self + 4) + 0x30) = 0x8000;

    if (func_0206e28c((u8*)data_02075f14, data_0208f668, 4) != 0) {
        *(char**)self = data_02075f14;
    } else {
        char* hdr = (char*)(int)M(data_02075f14);
        unsigned int size = (unsigned int)M(*(unsigned int*)(hdr + 4) >> 8);
        void* dst = _ZN6Memory8AllocateEj(size);
        DecompressLZ16(hdr + 4, dst);
        _ZN4CP1514FlushDataCacheEjj((unsigned int)dst, size);
        *(void**)self = dst;
    }

    func_0204a17c(*(char**)(self + 4), *(char**)self);
    func_0204a0dc(*(void**)(self + 4), _ZN8Particle7Texture12AllocTexVramEjb);
    func_0204a028(*(void**)(self + 4), _ZN8Particle7Texture12AllocPalVramEjb);

    if (*(char**)self != data_02075f14) {
        void* heap = (void*)(int)M(data_020a0ea0);
        unsigned int oldSize = (unsigned int)M(*(unsigned int*)(*(char**)self + 0x18));
        _ZN4Heap7_SizeofEPv(heap, *(void**)self);
        _ZN4Heap10ReallocateEPvj(heap, *(void**)self, oldSize);
    }
}
