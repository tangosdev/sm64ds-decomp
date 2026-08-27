extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *a);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *a);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *a);
extern void func_ov071_02121b08(void *a);

void func_ov071_02121b50(void *self, void *clsn)
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(clsn);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(clsn) || _ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        func_ov071_02121b08(self);
    }
}
