extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void func_02017984();

typedef struct { int a, b; } S8;

extern S8 data_ov090_021344a0;
extern S8 data_ov090_02134488;
extern S8 data_ov090_02134480;
extern S8 data_ov090_02134490;
extern S8 data_ov090_02134498;

extern S8 data_ov090_021344c0, data_ov090_021344d8, data_ov090_021344a8, data_ov090_021344cc, data_ov090_021344b4;

extern S8 data_ov090_021344f4[2], data_ov090_02134504[2], data_ov090_02134514[2], data_ov090_021344e4[2];
extern S8 data_ov090_02134110, data_ov090_02134108, data_ov090_02134118, data_ov090_021340f8;
extern S8 data_ov090_021340e8, data_ov090_021340e0, data_ov090_021340f0, data_ov090_02134100;

void __sinit_ov090_02133ce8(void)
{
    func_02017acc(&data_ov090_021344a0, 0x39f);
    func_020731dc(&data_ov090_021344a0, func_02017ab4, &data_ov090_021344c0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov090_02134488, 0x3a0);
    func_020731dc(&data_ov090_02134488, func_02017984, &data_ov090_021344d8);
    _ZN13SharedFilePtr9ConstructEj(&data_ov090_02134480, 0x3a1);
    func_020731dc(&data_ov090_02134480, func_02017984, &data_ov090_021344a8);
    _ZN13SharedFilePtr9ConstructEj(&data_ov090_02134490, 0x3a2);
    func_020731dc(&data_ov090_02134490, func_02017984, &data_ov090_021344cc);
    _ZN13SharedFilePtr9ConstructEj(&data_ov090_02134498, 0x3a3);
    func_020731dc(&data_ov090_02134498, func_02017984, &data_ov090_021344b4);

    data_ov090_021344f4[0] = data_ov090_02134110; data_ov090_021344f4[1] = data_ov090_02134108;
    data_ov090_02134504[0] = data_ov090_02134118; data_ov090_02134504[1] = data_ov090_021340f8;
    data_ov090_02134514[0] = data_ov090_021340e8; data_ov090_02134514[1] = data_ov090_021340e0;
    data_ov090_021344e4[0] = data_ov090_021340f0; data_ov090_021344e4[1] = data_ov090_02134100;
}
