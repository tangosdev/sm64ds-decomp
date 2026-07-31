extern void WithMeshClsn_UpdateDiscreteNoLava_veneer(void *a);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *a);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *a);
extern void func_ov071_02121b08(void *a);

void func_ov071_02121b50(void *self, void *clsn)
{
    WithMeshClsn_UpdateDiscreteNoLava_veneer(clsn);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(clsn) || _ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
        func_ov071_02121b08(self);
    }
}
