extern int _Z14ApproachLinearRsss();
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j();
extern int _ZNK12WithMeshClsn10IsOnGroundEv();
extern int func_ov073_0211f2c0();
extern int func_02012694();
extern int func_ov073_0212157c();
extern int data_ov073_021233b0[];

int func_ov073_02120844(int *t)
{
    _Z14ApproachLinearRsss((char*)t + 0x8c, -0x4000, 0x400);
    t[0x140] = _ZN5Sound8PlayLongEjjjRK7Vector3j(t[0x140], 3, 0x170, (char*)t + 0x74, 0);
    if (t[0xf7] > t[0x18] && _ZNK12WithMeshClsn10IsOnGroundEv((char*)t + 0x150)) {
        func_ov073_0211f2c0(t, 0x7d0000);
        func_02012694(0x16c, (char*)t + 0x74);
        t[0x26] = 0;
        func_ov073_0212157c(t, data_ov073_021233b0);
    }
    return 1;
}
