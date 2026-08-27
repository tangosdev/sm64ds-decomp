extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();
extern void _ZN7Vector3D1Ev();

typedef struct { int a, b; } S8;

extern S8 data_ov081_02128ed4;
extern S8 data_ov081_02128edc;
extern S8 data_ov081_02128ee4;
extern S8 data_ov081_02128ecc;
extern S8 data_ov081_02128ec4;
extern S8 data_ov081_02128f04, data_ov081_02128f10, data_ov081_02128f1c, data_ov081_02128f28, data_ov081_02128f34;

extern S8 data_ov081_02128f40[18];

extern S8 data_ov081_02128b58, data_ov081_02128b68;
extern S8 data_ov081_02128b40, data_ov081_02128b48;
extern S8 data_ov081_02128b38, data_ov081_02128bc0;
extern S8 data_ov081_02128bb8, data_ov081_02128bb0;
extern S8 data_ov081_02128ba8, data_ov081_02128ba0;
extern S8 data_ov081_02128b88, data_ov081_02128b98;
extern S8 data_ov081_02128b90, data_ov081_02128b70;
extern S8 data_ov081_02128b50, data_ov081_02128b60;
extern S8 data_ov081_02128b80, data_ov081_02128b78;

typedef struct { int a, b, c; } S12;
extern S12 data_ov081_02128ef8;
extern S8 data_ov081_02128eec;

void __sinit_ov081_021284f0(void)
{
    func_02017acc(&data_ov081_02128ed4, 0x2fd);
    func_020731dc(&data_ov081_02128ed4, func_02017ab4, &data_ov081_02128f04);
    _ZN13SharedFilePtr9ConstructEj(&data_ov081_02128edc, 0x300);
    func_020731dc(&data_ov081_02128edc, SharedFilePtr_Destruct_Anim, &data_ov081_02128f10);
    _ZN13SharedFilePtr9ConstructEj(&data_ov081_02128ee4, 0x301);
    func_020731dc(&data_ov081_02128ee4, SharedFilePtr_Destruct_Anim, &data_ov081_02128f1c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov081_02128ecc, 0x2fe);
    func_020731dc(&data_ov081_02128ecc, SharedFilePtr_Destruct_Anim, &data_ov081_02128f28);
    _ZN13SharedFilePtr9ConstructEj(&data_ov081_02128ec4, 0x2ff);
    func_020731dc(&data_ov081_02128ec4, SharedFilePtr_Destruct_Anim, &data_ov081_02128f34);

    data_ov081_02128f40[0] = data_ov081_02128b58;
    data_ov081_02128f40[1] = data_ov081_02128b68;
    data_ov081_02128f40[2] = data_ov081_02128b40;
    data_ov081_02128f40[3] = data_ov081_02128b48;
    data_ov081_02128f40[4] = data_ov081_02128b38;
    data_ov081_02128f40[5] = data_ov081_02128bc0;
    data_ov081_02128f40[6] = data_ov081_02128bb8;
    data_ov081_02128f40[7] = data_ov081_02128bb0;
    data_ov081_02128f40[8] = data_ov081_02128ba8;
    data_ov081_02128f40[9] = data_ov081_02128ba0;
    data_ov081_02128f40[10] = data_ov081_02128b88;
    data_ov081_02128f40[11] = data_ov081_02128b98;
    data_ov081_02128f40[12] = data_ov081_02128b90;
    data_ov081_02128f40[13] = data_ov081_02128b70;
    data_ov081_02128f40[14] = data_ov081_02128b50;
    data_ov081_02128f40[15] = data_ov081_02128b60;
    data_ov081_02128f40[16] = data_ov081_02128b80;
    data_ov081_02128f40[17] = data_ov081_02128b78;

    data_ov081_02128ef8.a = 0;
    data_ov081_02128ef8.b = 0x2000;
    data_ov081_02128ef8.c = 0;
    func_020731dc(&data_ov081_02128ef8, _ZN7Vector3D1Ev, &data_ov081_02128eec);
}
