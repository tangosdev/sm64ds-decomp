extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017b4c();
extern int func_02017ab4[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int func_02017984[];
extern int data_0211ef80[];
extern int data_0211ef98[];
extern int data_0211ef88[];
extern int data_0211efa4[];
extern int data_0211ef90[];
extern int data_0211efb0[];

struct Pair { int a, b; };
extern struct Pair data_0211ecd8;
extern struct Pair data_0211ece8;
extern struct Pair data_0211ece0;
extern struct Pair data_0211ecf0;
struct Quad { struct Pair p0, p1, p2, p3; };
extern struct Quad data_0211efbc;

void __sinit_ov063_0211e5fc(void)
{
    func_02017acc(data_0211ef80, 0x40a);
    func_020731dc(data_0211ef80, func_02017ab4, data_0211ef98);
    func_02017b4c(data_0211ef88, 0x40b);
    func_020731dc(data_0211ef88, SharedFilePtr_Destruct_Clsn, data_0211efa4);
    _ZN13SharedFilePtr9ConstructEj(data_0211ef90, 0x40c);
    func_020731dc(data_0211ef90, func_02017984, data_0211efb0);
    data_0211efbc.p0 = data_0211ecd8;
    data_0211efbc.p1 = data_0211ece8;
    data_0211efbc.p2 = data_0211ece0;
    data_0211efbc.p3 = data_0211ecf0;
}
