extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern int data_ov084_02130da4[];
extern int data_ov084_02130dac[];
extern int data_ov084_02130d9c[];
extern int data_ov084_02130db8[];

struct Pair { int a, b; };
extern struct Pair data_ov084_021309dc;
extern struct Pair data_ov084_021309ec;
extern struct Pair data_ov084_021309cc;
extern struct Pair data_ov084_021309e4;
extern struct Pair data_ov084_021309c4;
extern struct Pair data_ov084_021309d4;

struct Dst {
    struct Pair p0;
    struct Pair p1;
    struct Pair p2;
    struct Pair p3;
    struct Pair p4;
    struct Pair p5;
};
extern struct Dst data_ov084_02130dc4_d;

void __sinit_ov084_02130558(void)
{
    func_02017acc(data_ov084_02130da4, 0x2b9);
    func_020731dc(data_ov084_02130da4, &func_02017ab4, data_ov084_02130dac);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130d9c, 0x2ba);
    func_020731dc(data_ov084_02130d9c, &SharedFilePtr_Destruct_Anim, data_ov084_02130db8);
    data_ov084_02130dc4_d.p0 = data_ov084_021309dc;
    data_ov084_02130dc4_d.p1 = data_ov084_021309ec;
    data_ov084_02130dc4_d.p2 = data_ov084_021309cc;
    data_ov084_02130dc4_d.p3 = data_ov084_021309e4;
    data_ov084_02130dc4_d.p4 = data_ov084_021309c4;
    data_ov084_02130dc4_d.p5 = data_ov084_021309d4;
}
