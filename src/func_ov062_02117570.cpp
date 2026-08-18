//cpp
extern "C" {
struct dActor_c;
struct WithMeshClsn;
int _ZNK12WithMeshClsn10IsOnGroundEv(struct WithMeshClsn *self);
struct WithMeshClsn *_ZNK12WithMeshClsn14GetFloorResultEv(struct WithMeshClsn *self);
int SurfaceInfo_TestFlag0x20(int *p);
void _ZN12dEnemyBase_c9SpawnCoinEv(struct dActor_c *self);
void _ZN8dActor_c24KillAndTrackInDeathTableEv(struct dActor_c *self);
void func_ov062_02117570(struct dActor_c *self) {
    if (!_ZNK12WithMeshClsn10IsOnGroundEv((struct WithMeshClsn *)((char*)self + 0x144))) return;
    struct WithMeshClsn *floor = _ZNK12WithMeshClsn14GetFloorResultEv((struct WithMeshClsn *)((char*)self + 0x144));
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    _ZN12dEnemyBase_c9SpawnCoinEv(self);
    _ZN8dActor_c24KillAndTrackInDeathTableEv(self);
}
}
