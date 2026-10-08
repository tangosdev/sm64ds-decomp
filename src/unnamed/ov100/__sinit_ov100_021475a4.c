extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern char data_ov100_021486bc[];
extern char data_ov100_021486a4[];
extern char data_ov100_021486ac[];
extern char data_ov100_021486b4[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern char data_ov100_021486c4[];
extern char data_ov100_021486d0[];
extern char data_ov100_021486dc[];
extern char data_ov100_021486e8[];

struct P2 { int a, b; };
struct P4 { struct P2 lo, hi; };
extern struct P4 data_ov100_021486f4;
extern struct P2 data_ov100_02148000;
extern struct P2 data_ov100_02147ff8;

void __sinit_ov100_021475a4(void)
{
    func_02017acc(data_ov100_021486bc, 0x9c02);
    func_020731dc(data_ov100_021486bc, func_02017ab4, data_ov100_021486c4);
    func_02017acc(data_ov100_021486a4, 0x9c01);
    func_020731dc(data_ov100_021486a4, func_02017ab4, data_ov100_021486d0);
    _ZN13SharedFilePtr9ConstructEj(data_ov100_021486ac, 0x9c04);
    func_020731dc(data_ov100_021486ac, SharedFilePtr_Destruct_Anim, data_ov100_021486dc);
    _ZN13SharedFilePtr9ConstructEj(data_ov100_021486b4, 0x9c03);
    func_020731dc(data_ov100_021486b4, SharedFilePtr_Destruct_Anim, data_ov100_021486e8);
    data_ov100_021486f4.lo = data_ov100_02148000;
    data_ov100_021486f4.hi = data_ov100_02147ff8;
}
