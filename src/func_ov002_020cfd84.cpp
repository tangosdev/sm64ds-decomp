//cpp
// func_ov002_020cfd84 at 0x020cfd84
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
struct CRData { int a,b,c,d,e; unsigned short f,g; int h,i,j; };
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN10ClsnResultC1Ev(void* self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, void* a, void* b, void* act);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern int _ZNK10ClsnResult9GetClsnIDEv(void* self);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN10ClsnResultD1Ev(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);

int func_ov002_020cfd84(void* actor, void* a, void* b) {
    char rl[0x78];
    char res[0x28];
    _ZN11RaycastLineC1Ev(rl);
    _ZN10ClsnResultC1Ev(res);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, a, b, actor);
    if (_ZN11RaycastLine10DetectClsnEv(rl) != 0) {
        *(CRData*)(res + 4) = *(CRData*)(rl + 0x14);
        if (_ZNK10ClsnResult9GetClsnIDEv(res) != -1) {
            void* act = _ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(res));
            if (act != 0) {
                int t = (*(unsigned short*)((char*)act + 0xc) == 0x12a);
                if (t == 0) {
                    _ZN10ClsnResultD1Ev(res);
                    _ZN11RaycastLineD1Ev(rl);
                    return 1;
                }
            }
        }
    }
    _ZN10ClsnResultD1Ev(res);
    _ZN11RaycastLineD1Ev(rl);
    return 0;
}
}