extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 data_ov065_0211d698, data_ov065_0211d6a8, data_ov065_0211d690, data_ov065_0211d6a0;
extern S8 data_ov065_0211d6d4, data_ov065_0211d6b0, data_ov065_0211d6bc, data_ov065_0211d6c8;

extern S8 data_ov065_0211d700[2], data_ov065_0211d710[2], data_ov065_0211d6e0[2], data_ov065_0211d6f0[2];
extern S8 data_ov065_0211cc30, data_ov065_0211cc50, data_ov065_0211cc20, data_ov065_0211cc40;
extern S8 data_ov065_0211cc28, data_ov065_0211cc38, data_ov065_0211cc58, data_ov065_0211cc48;

void __sinit_ov065_0211c2a8(void)
{
    func_02017acc(&data_ov065_0211d698, 0x298);
    func_020731dc(&data_ov065_0211d698, func_02017ab4, &data_ov065_0211d6d4);
    func_02017acc(&data_ov065_0211d6a8, 0x29b);
    func_020731dc(&data_ov065_0211d6a8, func_02017ab4, &data_ov065_0211d6b0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov065_0211d690, 0x299);
    func_020731dc(&data_ov065_0211d690, func_02017984, &data_ov065_0211d6bc);
    _ZN13SharedFilePtr9ConstructEj(&data_ov065_0211d6a0, 0x29a);
    func_020731dc(&data_ov065_0211d6a0, func_02017984, &data_ov065_0211d6c8);

    data_ov065_0211d700[0] = data_ov065_0211cc30; data_ov065_0211d700[1] = data_ov065_0211cc50;
    data_ov065_0211d710[0] = data_ov065_0211cc20; data_ov065_0211d710[1] = data_ov065_0211cc40;
    data_ov065_0211d6e0[0] = data_ov065_0211cc28; data_ov065_0211d6e0[1] = data_ov065_0211cc38;
    data_ov065_0211d6f0[0] = data_ov065_0211cc58; data_ov065_0211d6f0[1] = data_ov065_0211cc48;
}
