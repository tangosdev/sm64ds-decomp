extern void func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int data_ov080_02128468[];
extern int data_ov080_02128470[];

struct Pair { int a, b; };
extern struct Pair data_ov080_0212814c;
extern struct Pair data_ov080_02128144;
extern struct Pair data_ov080_0212813c;
extern struct Pair data_ov080_02128154;
extern struct Pair data_ov080_0212812c;
extern struct Pair data_ov080_02128134;

struct Dst {
    struct Pair p0;
    struct Pair p1;
    struct Pair p2;
    struct Pair p3;
    struct Pair p4;
    struct Pair p5;
};
extern struct Dst data_ov080_0212847c;

void __sinit_ov080_02127a60(void)
{
    func_02017acc(data_ov080_02128468, 0x2ac);
    func_020731dc(data_ov080_02128468, &func_02017ab4, data_ov080_02128470);
    data_ov080_0212847c.p0 = data_ov080_0212814c;
    data_ov080_0212847c.p1 = data_ov080_02128144;
    data_ov080_0212847c.p2 = data_ov080_0212813c;
    data_ov080_0212847c.p3 = data_ov080_02128154;
    data_ov080_0212847c.p4 = data_ov080_0212812c;
    data_ov080_0212847c.p5 = data_ov080_02128134;
}
