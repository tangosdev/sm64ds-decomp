//cpp
extern "C" {
struct dActor_c;
struct WithMeshClsn;
int _ZNK12WithMeshClsn10IsOnGroundEv(struct WithMeshClsn *self);
struct WithMeshClsn *_ZNK12WithMeshClsn14GetFloorResultEv(struct WithMeshClsn *self);
int SurfaceInfo_TestFlag0x20(int *p);
void _ZN8dActor_c8PoofDustEv(struct dActor_c *self);
void _ZN7fBase_c18MarkForDestructionEv(struct dActor_c *self);
void func_ov102_0214c7fc(struct dActor_c *self) {
    if (!_ZNK12WithMeshClsn10IsOnGroundEv((struct WithMeshClsn *)((char*)self + 0x144))) return;
    struct WithMeshClsn *floor = _ZNK12WithMeshClsn14GetFloorResultEv((struct WithMeshClsn *)((char*)self + 0x144));
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    _ZN8dActor_c8PoofDustEv(self);
    _ZN7fBase_c18MarkForDestructionEv(self);
}
}
