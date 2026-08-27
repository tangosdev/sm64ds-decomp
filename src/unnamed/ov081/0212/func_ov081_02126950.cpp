//cpp
extern "C" {
struct dBgCh_Actr;
int func_02038414(void* c);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* c);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, int* out);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* c);
void* _ZNK10dBgCh_Actr13GetWallResultEv(void* c);

int func_ov081_02126950(int* self, void* clsn)
{
    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn)+4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4/4];
            long long b = (long long)n0[2] * (long long)self[0xac/4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8/4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);
    }
}
}
