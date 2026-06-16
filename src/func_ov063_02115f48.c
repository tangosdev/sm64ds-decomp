extern int _ZN11ShadowModelD1Ev();
extern int _ZN5ModelD1Ev();
extern int _ZN9ModelAnimD1Ev();
extern int _ZN12WithMeshClsnD1Ev();
extern int _ZN25MovingCylinderClsnWithPosD1Ev();
extern int func_ov002_020aedbc();
extern int _ZN6Memory10DeallocateEPvP4Heap();
extern int data_ov063_0211e828[];
extern int *data_020a0eac;

int *func_ov063_02115f48(int *t)
{
    *t = (int)data_ov063_0211e828;
    _ZN11ShadowModelD1Ev((char *)t + 0x45c);
    _ZN11ShadowModelD1Ev((char *)t + 0x434);
    _ZN5ModelD1Ev((char *)t + 0x3e4);
    _ZN9ModelAnimD1Ev((char *)t + 0x380);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1c4);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x184);
    func_ov002_020aedbc(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
