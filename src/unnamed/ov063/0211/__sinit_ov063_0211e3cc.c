extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int func_02017ab4();
extern int SharedFilePtr_Destruct_Clsn();

extern int data_ov063_0211eeb8[];
extern int data_ov063_0211ef08[];
extern int data_ov063_0211eeb0[];
extern int data_ov063_0211ef20[];
extern int data_ov063_0211ee98[];
extern int data_ov063_0211ef2c[];
extern int data_ov063_0211eea8[];
extern int data_ov063_0211eed8[];
extern int data_ov063_0211eec0[];
extern int data_ov063_0211ef14[];
extern int data_ov063_0211eea0[];
extern int data_ov063_0211eee4[];
extern int data_ov063_0211eec8[];
extern int data_ov063_0211eef0[];
extern int data_ov063_0211eed0[];
extern int data_ov063_0211eefc[];

struct S2 { int w[2]; };
extern struct S2 data_ov063_0211e9b4;
extern struct S2 data_ov063_0211e9bc;
extern struct S2 data_ov063_0211e9c4;
extern struct S2 data_ov063_0211e9ac;
struct D { struct S2 a, b, c, d; };
extern struct D data_ov063_0211ef38;

void __sinit_ov063_0211e3cc(void)
{
    func_02017acc(data_ov063_0211eeb8, 0x6bb);
    func_020731dc(data_ov063_0211eeb8, func_02017ab4, data_ov063_0211ef08);
    func_02017acc(data_ov063_0211eeb0, 0x6bd);
    func_020731dc(data_ov063_0211eeb0, func_02017ab4, data_ov063_0211ef20);
    func_02017acc(data_ov063_0211ee98, 0x6c1);
    func_020731dc(data_ov063_0211ee98, func_02017ab4, data_ov063_0211ef2c);
    func_02017acc(data_ov063_0211eea8, 0x6c3);
    func_020731dc(data_ov063_0211eea8, func_02017ab4, data_ov063_0211eed8);
    func_02017b4c(data_ov063_0211eec0, 0x6bc);
    func_020731dc(data_ov063_0211eec0, SharedFilePtr_Destruct_Clsn, data_ov063_0211ef14);
    func_02017b4c(data_ov063_0211eea0, 0x6be);
    func_020731dc(data_ov063_0211eea0, SharedFilePtr_Destruct_Clsn, data_ov063_0211eee4);
    func_02017b4c(data_ov063_0211eec8, 0x6c2);
    func_020731dc(data_ov063_0211eec8, SharedFilePtr_Destruct_Clsn, data_ov063_0211eef0);
    func_02017b4c(data_ov063_0211eed0, 0x6c4);
    func_020731dc(data_ov063_0211eed0, SharedFilePtr_Destruct_Clsn, data_ov063_0211eefc);
    data_ov063_0211ef38.a = data_ov063_0211e9b4;
    data_ov063_0211ef38.b = data_ov063_0211e9bc;
    data_ov063_0211ef38.c = data_ov063_0211e9c4;
    data_ov063_0211ef38.d = data_ov063_0211e9ac;
}
