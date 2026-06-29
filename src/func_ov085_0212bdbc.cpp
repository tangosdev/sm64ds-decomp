//cpp
extern "C" {
extern int _ZN6Player14IsFrontSlidingEv(void*);
extern int _ZN6Player17LostGrabbedObjectEv(void*);
extern void* _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(void*, void*, void*);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void*,void*,void*,int,int,unsigned int);
extern char data_ov085_021306ec[];
extern int data_020a0e68[];
struct M4x3 { int w[12]; };

void func_ov085_0212bdbc(char* c){
    int idx;
    void* res;
    if (!*(void**)(c + 0x45c)) return;
    if (!*(int*)(*(char**)(c + 0x45c) + 0xc8)) return;
    idx = 0;
    if (_ZN6Player14IsFrontSlidingEv(*(void**)(c + 0x45c)) || _ZN6Player17LostGrabbedObjectEv(*(void**)(c + 0x45c))) {
        idx = 1;
    }
    if (*(int*)(*(char**)(c + 0x45c) + 8) == 2) {
        idx = (idx + 2) & 0xff;
    }
    res = _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(c, *(void**)(c + 0x45c), data_ov085_021306ec + idx * 0xc);
    *(struct M4x3*)(c + 0x31c) = *(struct M4x3*)res;
    Matrix4x3_FromTranslation(data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xc000) >> 3, *(int*)(c + 0x64) >> 3);
    *(struct M4x3*)(c + 0x390) = *(struct M4x3*)data_020a0e68;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x368, c + 0x390, 0x46000, 0x258000, 0xf);
}
}
