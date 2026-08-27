typedef struct { int x, y, z; } Vector3;
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
extern char* _ZNK10dBgCh_Actr14GetFloorResultEv(void* p);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* p, Vector3* v);
extern short func_02010844(void* unused, Vector3* v, short angle);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* p);
int func_ov062_02115f84(char* c) {
    Vector3 v;
    short r4 = 0;
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x144)) {
        char* fr = _ZNK10dBgCh_Actr14GetFloorResultEv(c+0x144);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr+4, &v);
        r4 = func_02010844(c, &v, *(short*)(c+0x8e));
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(c+0x144))
        return 1;
    if (r4 < 0) r4 = -r4;
    if (r4 > 0x1000) return 2;
    return 0;
}
