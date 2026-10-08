extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern int data_ov064_0211c96c[];
extern int data_ov064_0211c974[];
extern int data_ov064_0211c964[];
extern int data_ov064_0211c980[];

struct Pair { int a, b; };
extern struct Pair data_ov064_0211c4c4;
extern struct Pair data_ov064_0211c4bc;
extern struct Pair data_ov064_0211c4b4;
extern struct Pair data_ov064_0211c4a4;
extern struct Pair data_ov064_0211c4ac;
extern struct Pair data_ov064_0211c49c;

struct Dst {
    struct Pair p0;
    struct Pair p1;
    struct Pair p2;
    struct Pair p3;
    struct Pair p4;
    struct Pair p5;
};
extern struct Dst data_ov064_0211c98c;

void __sinit_ov064_0211b59c(void)
{
    func_02017acc(data_ov064_0211c96c, 0x4a6);
    func_020731dc(data_ov064_0211c96c, &func_02017ab4, data_ov064_0211c974);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c964, 0x4a7);
    func_020731dc(data_ov064_0211c964, &SharedFilePtr_Destruct_Anim, data_ov064_0211c980);
    data_ov064_0211c98c.p0 = data_ov064_0211c4c4;
    data_ov064_0211c98c.p1 = data_ov064_0211c4bc;
    data_ov064_0211c98c.p2 = data_ov064_0211c4b4;
    data_ov064_0211c98c.p3 = data_ov064_0211c4a4;
    data_ov064_0211c98c.p4 = data_ov064_0211c4ac;
    data_ov064_0211c98c.p5 = data_ov064_0211c49c;
}
