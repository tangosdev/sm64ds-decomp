//cpp
extern "C" {
int dBgCh_Actr_UpdateContinuous_Veneer(void* c);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* c);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* s, int* out);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* c);
void* _ZNK10dBgCh_Actr13GetWallResultEv(void* c);

void func_ov072_02120fd4(int* self, void* clsn)
{
    int n0[3];
    int n1[3];
    int b = (int)((self[0xb0/4] & 0x4000) != 0);
    if (b != 0) return;
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn)+4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4/4];
            long long bb = (long long)n0[2] * (long long)self[0xac/4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((bb + 0x800) >> 12);
            self[0xa8/4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)_ZNK10dBgCh_Actr13GetWallResultEv(clsn)+4, n1);
    }
}
}
