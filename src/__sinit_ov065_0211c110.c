extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 data_ov065_0211d610;
extern S8 data_ov065_0211d618;
extern S8 data_ov065_0211d600;
extern S8 data_ov065_0211d608;
extern S8 data_ov065_0211d644, data_ov065_0211d620, data_ov065_0211d62c, data_ov065_0211d638;

extern S8 data_ov065_0211d670[2];
extern S8 data_ov065_0211d680[2];
extern S8 data_ov065_0211d650[2];
extern S8 data_ov065_0211d660[2];

extern S8 data_ov065_0211cb30, data_ov065_0211cb58;
extern S8 data_ov065_0211cb20, data_ov065_0211cb40;
extern S8 data_ov065_0211cb28, data_ov065_0211cb50;
extern S8 data_ov065_0211cb38, data_ov065_0211cb48;

void __sinit_ov065_0211c110(void)
{
    func_02017acc(&data_ov065_0211d610, 0x42f);
    func_020731dc(&data_ov065_0211d610, func_02017ab4, &data_ov065_0211d644);
    func_02017acc(&data_ov065_0211d618, 0x42d);
    func_020731dc(&data_ov065_0211d618, func_02017ab4, &data_ov065_0211d620);
    _ZN13SharedFilePtr9ConstructEj(&data_ov065_0211d600, 0x430);
    func_020731dc(&data_ov065_0211d600, func_02017984, &data_ov065_0211d62c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov065_0211d608, 0x42e);
    func_020731dc(&data_ov065_0211d608, func_02017984, &data_ov065_0211d638);

    data_ov065_0211d670[0] = data_ov065_0211cb30; data_ov065_0211d670[1] = data_ov065_0211cb58;
    data_ov065_0211d680[0] = data_ov065_0211cb20; data_ov065_0211d680[1] = data_ov065_0211cb40;
    data_ov065_0211d650[0] = data_ov065_0211cb28; data_ov065_0211d650[1] = data_ov065_0211cb50;
    data_ov065_0211d660[0] = data_ov065_0211cb38; data_ov065_0211d660[1] = data_ov065_0211cb48;
}
