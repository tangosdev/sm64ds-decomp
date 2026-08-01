//cpp
extern "C" {
struct Actor;
struct WithMeshClsn;
int _ZNK12WithMeshClsn10IsOnGroundEv(struct WithMeshClsn *self);
struct WithMeshClsn *_ZNK12WithMeshClsn14GetFloorResultEv(struct WithMeshClsn *self);
int SurfaceInfo_TestFlag0x20(int *p);
void _ZN5Actor8PoofDustEv(struct Actor *self);
void _ZN9ActorBase18MarkForDestructionEv(struct Actor *self);
void func_ov102_0214c7fc(struct Actor *self) {
    if (!_ZNK12WithMeshClsn10IsOnGroundEv((struct WithMeshClsn *)((char*)self + 0x144))) return;
    struct WithMeshClsn *floor = _ZNK12WithMeshClsn14GetFloorResultEv((struct WithMeshClsn *)((char*)self + 0x144));
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    _ZN5Actor8PoofDustEv(self);
    _ZN9ActorBase18MarkForDestructionEv(self);
}
}
