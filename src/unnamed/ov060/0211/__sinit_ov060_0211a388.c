extern void func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int data_ov060_0211b1c4[];
extern int data_ov060_0211b1cc[];
struct W2 { int a, b; };
extern struct W2 data_ov060_0211aa30;
extern struct W2 data_ov060_0211aa48;
extern struct W2 data_ov060_0211aa40;
extern struct W2 data_ov060_0211aa38;
extern struct W2 data_ov060_0211b1d8[];
void __sinit_ov060_0211a388(void)
{
    func_02017acc(data_ov060_0211b1c4, 0x382);
    func_020731dc(data_ov060_0211b1c4, &func_02017ab4, data_ov060_0211b1cc);
    data_ov060_0211b1d8[0] = data_ov060_0211aa30;
    data_ov060_0211b1d8[1] = data_ov060_0211aa48;
    data_ov060_0211b1d8[2] = data_ov060_0211aa40;
    data_ov060_0211b1d8[3] = data_ov060_0211aa38;
}
