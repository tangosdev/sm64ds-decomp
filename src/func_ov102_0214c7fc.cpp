//cpp
extern "C" {
struct dActor_c;
struct dBgCh_Actr;
int _ZNK10dBgCh_Actr10IsOnGroundEv(struct dBgCh_Actr *self);
struct dBgCh_Actr *_ZNK10dBgCh_Actr14GetFloorResultEv(struct dBgCh_Actr *self);
int SurfaceInfo_TestFlag0x20(int *p);
void _ZN8dActor_c8PoofDustEv(struct dActor_c *self);
void _ZN7fBase_c18MarkForDestructionEv(struct dActor_c *self);
void func_ov102_0214c7fc(struct dActor_c *self) {
    if (!_ZNK10dBgCh_Actr10IsOnGroundEv((struct dBgCh_Actr *)((char*)self + 0x144))) return;
    struct dBgCh_Actr *floor = _ZNK10dBgCh_Actr14GetFloorResultEv((struct dBgCh_Actr *)((char*)self + 0x144));
    if (!SurfaceInfo_TestFlag0x20((int*)((char*)floor + 4))) return;
    _ZN8dActor_c8PoofDustEv(self);
    _ZN7fBase_c18MarkForDestructionEv(self);
}
}
