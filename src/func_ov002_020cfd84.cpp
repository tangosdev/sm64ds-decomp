//cpp
// func_ov002_020cfd84 at 0x020cfd84
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
struct CRData { int a,b,c,d,e; unsigned short f,g; int h,i,j; };
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN5dBgPiC1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern int _ZNK5dBgPi9GetClsnIDEv(void* self);
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN5dBgPiD1Ev(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);

int func_ov002_020cfd84(void* actor, void* a, void* b) {
    char rl[0x78];
    char res[0x28];
    _ZN9dBgCh_LinC1Ev(rl);
    _ZN5dBgPiC1Ev(res);
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, a, b, actor);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
        *(CRData*)(res + 4) = *(CRData*)(rl + 0x14);
        if (_ZNK5dBgPi9GetClsnIDEv(res) != -1) {
            void* act = _ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(res));
            if (act != 0) {
                int t = (*(unsigned short*)((char*)act + 0xc) == 0x12a);
                if (t == 0) {
                    _ZN5dBgPiD1Ev(res);
                    _ZN9dBgCh_LinD1Ev(rl);
                    return 1;
                }
            }
        }
    }
    _ZN5dBgPiD1Ev(res);
    _ZN9dBgCh_LinD1Ev(rl);
    return 0;
}
}