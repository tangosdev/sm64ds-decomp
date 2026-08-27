//cpp
extern "C" {
extern void _ZN9dBgCh_GndC1Ev(void* self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void* self, void* pos, void* act);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void* self);
extern void _ZN9dBgCh_GndD1Ev(void* self);
int func_ov002_020c6538(char* c) {
    char rg[0x54];
    int v[3];
    int y;
    _ZN9dBgCh_GndC1Ev(rg);
    v[0] = *(int*)(c+0x5c);
    y = *(int*)(c+0x60);
    v[1] = y;
    v[2] = *(int*)(c+0x64);
    v[1] = y + 0x32000;
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, v, c);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(rg) == 0) {
        _ZN9dBgCh_GndD1Ev(rg);
        return 0;
    }
    {
        int hit = *(int*)(rg+0x44);
        int d = hit - *(int*)(c+0x60);
        if (d < 0) d = -d;
        if (d >= 0x64000) {
            _ZN9dBgCh_GndD1Ev(rg);
            return 0;
        }
        *(int*)(c+0x60) = hit;
        *(int*)(c+0x688) = *(int*)(c+0x60);
        _ZN9dBgCh_GndD1Ev(rg);
        return 1;
    }
}
}
