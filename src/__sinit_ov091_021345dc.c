extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern void func_02017ab4();
extern void func_02017b34();
extern int data_ov091_02135498[];
extern int data_ov091_021354bc[];
extern int data_ov091_021354a8[];
extern int data_ov091_021354b0[];
extern int data_ov091_021354a0[];
extern int data_ov091_021354c8[];
extern int data_ov091_02135490[];
extern int data_ov091_021354d4[];
typedef struct { int a, b; } Pair;
extern Pair data_ov091_021354e0[];
extern Pair data_ov091_02134e54[];
extern Pair data_ov091_02134e44[];
extern Pair data_ov091_02134e4c[];
void __sinit_ov091_021345dc(void)
{
    func_02017acc(data_ov091_02135498, 0x683);
    func_020731dc(data_ov091_02135498, func_02017ab4, data_ov091_021354bc);
    func_02017acc(data_ov091_021354a8, 0x669);
    func_020731dc(data_ov091_021354a8, func_02017ab4, data_ov091_021354b0);
    func_02017b4c(data_ov091_021354a0, 0x684);
    func_020731dc(data_ov091_021354a0, func_02017b34, data_ov091_021354c8);
    func_02017b4c(data_ov091_02135490, 0x66a);
    func_020731dc(data_ov091_02135490, func_02017b34, data_ov091_021354d4);
    data_ov091_021354e0[0] = data_ov091_02134e54[0];
    data_ov091_021354e0[1] = data_ov091_02134e44[0];
    data_ov091_021354e0[2] = data_ov091_02134e4c[0];
}
