extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 data_ov062_0211e0fc;
extern S8 data_ov062_0211e114;
extern S8 data_ov062_0211e10c;
extern S8 data_ov062_0211e104;
extern S8 data_ov062_0211e134, data_ov062_0211e140, data_ov062_0211e128, data_ov062_0211e11c;

extern S8 data_ov062_0211e15c[2], data_ov062_0211e17c[2], data_ov062_0211e18c[2], data_ov062_0211e14c[2], data_ov062_0211e16c[2];
extern S8 data_ov062_0211dcd8, data_ov062_0211dcf0, data_ov062_0211dd10, data_ov062_0211dce0, data_ov062_0211dcf8, data_ov062_0211dcd0, data_ov062_0211dd18, data_ov062_0211dd00, data_ov062_0211dce8, data_ov062_0211dd08;

void __sinit_ov062_0211d6fc(void)
{
    func_02017acc(&data_ov062_0211e0fc, 0x348);
    func_020731dc(&data_ov062_0211e0fc, func_02017ab4, &data_ov062_0211e134);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211e114, 0x349);
    func_020731dc(&data_ov062_0211e114, SharedFilePtr_Destruct_Anim, &data_ov062_0211e140);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211e10c, 0x34a);
    func_020731dc(&data_ov062_0211e10c, SharedFilePtr_Destruct_Anim, &data_ov062_0211e128);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211e104, 0x34b);
    func_020731dc(&data_ov062_0211e104, SharedFilePtr_Destruct_Anim, &data_ov062_0211e11c);

    data_ov062_0211e15c[0] = data_ov062_0211dcd8; data_ov062_0211e15c[1] = data_ov062_0211dcf0;
    data_ov062_0211e17c[0] = data_ov062_0211dd10; data_ov062_0211e17c[1] = data_ov062_0211dce0;
    data_ov062_0211e18c[0] = data_ov062_0211dcf8; data_ov062_0211e18c[1] = data_ov062_0211dcd0;
    data_ov062_0211e14c[0] = data_ov062_0211dd18; data_ov062_0211e14c[1] = data_ov062_0211dd00;
    data_ov062_0211e16c[0] = data_ov062_0211dce8; data_ov062_0211e16c[1] = data_ov062_0211dd08;
}
