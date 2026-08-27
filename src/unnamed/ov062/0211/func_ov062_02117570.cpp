//cpp
extern "C" {
struct dActor_c;
struct dBgCh_Actr;
int _ZNK10dBgCh_Actr10IsOnGroundEv(struct dBgCh_Actr *self);
struct dBgCh_Actr *_ZNK10dBgCh_Actr14GetFloorResultEv(struct dBgCh_Actr *self);
int SurfaceInfo_TestFlag0x20(int *p);
void _ZN12dEnemyBase_c9SpawnCoinEv(struct dActor_c *self);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(struct dActor_c *self);
void func_ov062_02117570(struct dActor_c *self) {
    if (!_ZNK10dBgCh_Actr10IsOnGroundEv((struct dBgCh_Actr *)((char*)self + 0x144))) return;
    struct dBgCh_Actr *floor = _ZNK10dBgCh_Actr14GetFloorResultEv((struct dBgCh_Actr *)((char*)self + 0x144));
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    _ZN12dEnemyBase_c9SpawnCoinEv(self);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
}
}
