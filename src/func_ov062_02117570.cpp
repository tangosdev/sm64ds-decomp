//cpp
extern "C" {
struct Actor;
struct WithMeshClsn;
int _ZNK12WithMeshClsn10IsOnGroundEv(struct WithMeshClsn *self);
struct WithMeshClsn *_ZNK12WithMeshClsn14GetFloorResultEv(struct WithMeshClsn *self);
int func_02037e78(int *p);
void _ZN5Enemy9SpawnCoinEv(struct Actor *self);
void _ZN5Actor24KillAndTrackInDeathTableEv(struct Actor *self);
void func_ov062_02117570(struct Actor *self) {
    if (!_ZNK12WithMeshClsn10IsOnGroundEv((struct WithMeshClsn *)((char*)self + 0x144))) return;
    struct WithMeshClsn *floor = _ZNK12WithMeshClsn14GetFloorResultEv((struct WithMeshClsn *)((char*)self + 0x144));
    if (!func_02037e78((int*)((char*)floor + 4))) return;
    _ZN5Enemy9SpawnCoinEv(self);
    _ZN5Actor24KillAndTrackInDeathTableEv(self);
}
}
