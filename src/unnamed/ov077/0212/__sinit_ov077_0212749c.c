extern int _ZN13SharedFilePtr9ConstructEj();
extern void func_020731dc();
extern void SharedFilePtr_Destruct_Anim(void);
extern char data_ov077_02127c14[];
extern char data_ov077_02127c1c[];

struct Pair { int a, b; };
extern struct Pair data_ov077_02127928;
extern struct Pair data_ov077_02127910;
extern struct Pair data_ov077_02127908;
extern struct Pair data_ov077_021278f0;
extern struct Pair data_ov077_02127918;
extern struct Pair data_ov077_021278f8;
extern struct Pair data_ov077_021278e8;
extern struct Pair data_ov077_02127940;
extern struct Pair data_ov077_02127938;
extern struct Pair data_ov077_02127930;
extern struct Pair data_ov077_02127900;
extern struct Pair data_ov077_02127920;

struct Dst {
    struct Pair p0;
    struct Pair p1;
    struct Pair p2;
    struct Pair p3;
    struct Pair p4;
    struct Pair p5;
    struct Pair p6;
    struct Pair p7;
    struct Pair p8;
    struct Pair p9;
    struct Pair p10;
    struct Pair p11;
};
extern struct Dst data_ov077_02127c28;

void __sinit_ov077_0212749c(void)
{
    _ZN13SharedFilePtr9ConstructEj(data_ov077_02127c14, 0x42b);
    func_020731dc(data_ov077_02127c14, &SharedFilePtr_Destruct_Anim, data_ov077_02127c1c);
    data_ov077_02127c28.p0 = data_ov077_02127928;
    data_ov077_02127c28.p1 = data_ov077_02127910;
    data_ov077_02127c28.p2 = data_ov077_02127908;
    data_ov077_02127c28.p3 = data_ov077_021278f0;
    data_ov077_02127c28.p4 = data_ov077_02127918;
    data_ov077_02127c28.p5 = data_ov077_021278f8;
    data_ov077_02127c28.p6 = data_ov077_021278e8;
    data_ov077_02127c28.p7 = data_ov077_02127940;
    data_ov077_02127c28.p8 = data_ov077_02127938;
    data_ov077_02127c28.p9 = data_ov077_02127930;
    data_ov077_02127c28.p10 = data_ov077_02127900;
    data_ov077_02127c28.p11 = data_ov077_02127920;
}
