extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 data_ov014_02114968;
extern S8 data_ov014_02114978;
extern S8 data_ov014_02114980;
extern S8 data_ov014_02114970;
extern S8 data_ov014_02114988, data_ov014_02114994, data_ov014_021149a0, data_ov014_021149ac;

typedef struct { S8 a, b; int pad; } S20;
extern S20 data_ov014_0211476c[6];
extern S8 data_ov014_021146e0, data_ov014_021146b8, data_ov014_021146f0, data_ov014_021146c0, data_ov014_021146a8, data_ov014_021146a0, data_ov014_021146f8, data_ov014_021146b0, data_ov014_021146d8, data_ov014_021146c8, data_ov014_021146e8, data_ov014_021146d0;

void __sinit_ov014_02113118(void)
{
    func_02017acc(&data_ov014_02114968, 0x9c02);
    func_020731dc(&data_ov014_02114968, func_02017ab4, &data_ov014_02114988);
    func_02017acc(&data_ov014_02114978, 0x9c01);
    func_020731dc(&data_ov014_02114978, func_02017ab4, &data_ov014_02114994);
    _ZN13SharedFilePtr9ConstructEj(&data_ov014_02114980, 0x9c04);
    func_020731dc(&data_ov014_02114980, SharedFilePtr_Destruct_Anim, &data_ov014_021149a0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov014_02114970, 0x9c03);
    func_020731dc(&data_ov014_02114970, SharedFilePtr_Destruct_Anim, &data_ov014_021149ac);

    data_ov014_0211476c[0].a = data_ov014_021146e0; data_ov014_0211476c[0].b = data_ov014_021146b8;
    data_ov014_0211476c[1].a = data_ov014_021146f0; data_ov014_0211476c[1].b = data_ov014_021146c0;
    data_ov014_0211476c[2].a = data_ov014_021146a8; data_ov014_0211476c[2].b = data_ov014_021146a0;
    data_ov014_0211476c[3].a = data_ov014_021146f8; data_ov014_0211476c[3].b = data_ov014_021146b0;
    data_ov014_0211476c[4].a = data_ov014_021146d8; data_ov014_0211476c[4].b = data_ov014_021146c8;
    data_ov014_0211476c[5].a = data_ov014_021146e8; data_ov014_0211476c[5].b = data_ov014_021146d0;
}
