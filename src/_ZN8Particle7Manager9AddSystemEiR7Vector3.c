typedef unsigned short u16;
typedef unsigned int u32;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

extern void* func_0204d958(void* p);
extern void func_0204ae2c(void* o, void* p18, void* src3);
extern void func_0204d9a0(void* a, void* b);

int _ZN8Particle7Manager9AddSystemEiR7Vector3(char* self, int idx, void* v) {
    int r4;
    char* sys;
    u32* p;
    u32 lo;
    r4 = 0;
    if (*(int*)(self + 0xc) != 0) {
        sys = (char*)func_0204d958(self + 0xc);
        r4 = (int)sys;
        func_0204ae2c(sys, *(char**)(self + 0x1c) + idx * 0x20, v);
        p = (u32*)AT(sys, 0x74);
        *p = (*p & ~0x3fu) | (*(u16*)(self + 0x2c) & 0x3f);
        *p = (*p & ~0xfc0u) | ((*(u16*)(self + 0x2e) & 0x3f) << 6);
        lo = (u32)(((long long)(int)(*(u32*)(sys + 0x74) << 26)) & 0xFFFFFFFFFFFFFFFFLL);
        lo = lo >> 26;
        *p = (*p & ~0x3f000u) | (((u32)(((long long)(int)lo) & 0xFFFFFFFFFFFFFFFFLL) & 0x3f) << 12);
        *p = *p & 0x3ffff;
        func_0204d9a0(self + 4, sys);
        if ((***(u32***)(sys + 0x18) << 17) >> 31) r4 = 0;
    }
    return r4;
}
