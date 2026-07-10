//cpp
extern "C" {
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned, unsigned, void*);
int _ZN6Player24St_MetalWaterGround_InitEv(char* c) {
    int* p = *(int**)(c+0x358);
    int b0 = (p != 0);
    if (b0) {
        int h = *(unsigned short*)((char*)p+0xc);
        int b1 = (h == 0x10c);
        if (!b1) {
            int b2 = (h == 0x10b);
            if (!b2) goto tail;
        }
        *(int*)(int)(((long long)(int)((char*)p+0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x4000;
        *(int*)(int)(((long long)(int)((char*)*(int**)(c+0x358)+0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x100;
        *(int**)(c+0x358) = 0;
    }
tail:
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2e, c+0x74);
    *(int*)(c+0x9c) = -0x1800;
    *(int*)(c+0xa0) = -0x14000;
    if (*(int*)(c+0x98) >= 0x14000) *(int*)(c+0x98) = 0x14000;
    *(unsigned char*)(c+0x706) = 1;
    return 1;
}
}
