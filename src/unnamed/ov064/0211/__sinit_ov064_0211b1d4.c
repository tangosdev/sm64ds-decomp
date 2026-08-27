extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();
extern char data_ov064_0211c810[];
extern char data_ov064_0211c8ec[];
extern char data_ov064_0211c818[];
extern char data_ov064_0211c8f8[];
extern char data_ov064_0211c7e8[];
extern char data_ov064_0211c868[];
extern char data_ov064_0211c840[];
extern char data_ov064_0211c85c[];
extern char data_ov064_0211c820[];
extern char data_ov064_0211c850[];
extern char data_ov064_0211c830[];
extern char data_ov064_0211c8d4[];
extern char data_ov064_0211c7e0[];
extern char data_ov064_0211c874[];
extern char data_ov064_0211c808[];
extern char data_ov064_0211c880[];
extern char data_ov064_0211c7d8[];
extern char data_ov064_0211c88c[];
extern char data_ov064_0211c7f0[];
extern char data_ov064_0211c898[];
extern char data_ov064_0211c828[];
extern char data_ov064_0211c8a4[];
extern char data_ov064_0211c7f8[];
extern char data_ov064_0211c8b0[];
extern char data_ov064_0211c838[];
extern char data_ov064_0211c8bc[];
extern char data_ov064_0211c848[];
extern char data_ov064_0211c8c8[];
extern char data_ov064_0211c800[];
extern char data_ov064_0211c8e0[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Clsn(void);

typedef struct {
    int a, b;
} Pair;

extern Pair data_ov064_0211bf80;
extern Pair data_ov064_0211bf68;
extern Pair data_ov064_0211bf90;
extern Pair data_ov064_0211bf70;
extern Pair data_ov064_0211bf88;
extern Pair data_ov064_0211bf78;

typedef struct {
    Pair p0, p1, p2, p3, p4, p5;
} Big;

extern Big data_ov064_0211c904;

void __sinit_ov064_0211b1d4(void) {
    func_02017acc(data_ov064_0211c810, 0x5ff);
    func_020731dc(data_ov064_0211c810, func_02017ab4, data_ov064_0211c8ec);
    func_02017acc(data_ov064_0211c818, 0x601);
    func_020731dc(data_ov064_0211c818, func_02017ab4, data_ov064_0211c8f8);
    func_02017acc(data_ov064_0211c7e8, 0x602);
    func_020731dc(data_ov064_0211c7e8, func_02017ab4, data_ov064_0211c868);
    func_02017acc(data_ov064_0211c840, 0x603);
    func_020731dc(data_ov064_0211c840, func_02017ab4, data_ov064_0211c85c);
    func_02017acc(data_ov064_0211c820, 0x604);
    func_020731dc(data_ov064_0211c820, func_02017ab4, data_ov064_0211c850);
    func_02017acc(data_ov064_0211c830, 0x605);
    func_020731dc(data_ov064_0211c830, func_02017ab4, data_ov064_0211c8d4);
    func_02017acc(data_ov064_0211c7e0, 0x606);
    func_020731dc(data_ov064_0211c7e0, func_02017ab4, data_ov064_0211c874);
    func_02017acc(data_ov064_0211c808, 0x607);
    func_020731dc(data_ov064_0211c808, func_02017ab4, data_ov064_0211c880);
    func_02017acc(data_ov064_0211c7d8, 0x608);
    func_020731dc(data_ov064_0211c7d8, func_02017ab4, data_ov064_0211c88c);
    func_02017acc(data_ov064_0211c7f0, 0x609);
    func_020731dc(data_ov064_0211c7f0, func_02017ab4, data_ov064_0211c898);
    func_02017acc(data_ov064_0211c828, 0x60a);
    func_020731dc(data_ov064_0211c828, func_02017ab4, data_ov064_0211c8a4);
    func_02017acc(data_ov064_0211c7f8, 0x60b);
    func_020731dc(data_ov064_0211c7f8, func_02017ab4, data_ov064_0211c8b0);
    func_02017acc(data_ov064_0211c838, 0x60c);
    func_020731dc(data_ov064_0211c838, func_02017ab4, data_ov064_0211c8bc);
    func_02017acc(data_ov064_0211c848, 0x60d);
    func_020731dc(data_ov064_0211c848, func_02017ab4, data_ov064_0211c8c8);
    func_02017b4c(data_ov064_0211c800, 0x600);
    func_020731dc(data_ov064_0211c800, SharedFilePtr_Destruct_Clsn, data_ov064_0211c8e0);

    data_ov064_0211c904.p0 = data_ov064_0211bf80;
    data_ov064_0211c904.p1 = data_ov064_0211bf68;
    data_ov064_0211c904.p2 = data_ov064_0211bf90;
    data_ov064_0211c904.p3 = data_ov064_0211bf70;
    data_ov064_0211c904.p4 = data_ov064_0211bf88;
    data_ov064_0211c904.p5 = data_ov064_0211bf78;
}
