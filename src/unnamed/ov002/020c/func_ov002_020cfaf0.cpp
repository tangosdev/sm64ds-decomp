//cpp
extern short data_02082214[];
extern "C" {
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, void* a, void* b, void* act);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);

int func_ov002_020cfaf0(char* c) {
    char rl[0x78];
    int v1[3];
    int v2[3];
    _ZN9dBgCh_LinC1Ev(rl);
    {
        int ang = *(short*)(c + 0x8e);
        int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
        int s = data_02082214[j] * 32;
        int t = data_02082214[j + 1] * 32;
        int ax = *(int*)(c + 0x5c) + s;
        int az = *(int*)(c + 0x64) + t;
        int ay = *(int*)(c + 0x60) - 0x20000;
        v1[0] = ax;
        v1[1] = ay;
        v1[2] = az;
        int bx = *(int*)(c + 0x5c) + s;
        int bz = *(int*)(c + 0x64) + t;
        int by = *(int*)(c + 0x60) + 0x20000;
        v2[0] = bx;
        v2[1] = by;
        v2[2] = bz;
    }
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, v1, v2, c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
        _ZN9dBgCh_LinD1Ev(rl);
        return 1;
    }
    _ZN9dBgCh_LinD1Ev(rl);
    return 0;
}
}
