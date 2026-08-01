extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Clsn(void);
extern int data_ov071_021230d0[];
extern int data_ov071_021230e0[];
extern int data_ov071_021230d8[];
extern int data_ov071_021230ec[];

struct Pair { int a, b; };
extern struct Pair data_ov071_02122e74;
extern struct Pair data_ov071_02122e8c;
extern struct Pair data_ov071_02122e84;
extern struct Pair data_ov071_02122e7c;
extern struct Pair data_ov071_02122ecc;

struct Dst {
    struct Pair p0;   /* 0x0, 0x4 */
    struct Pair p2;   /* 0x8, 0xc */
    int gap;          /* 0x10 */
    struct Pair p5;   /* 0x14, 0x18 */
    struct Pair p7;   /* 0x1c, 0x20 */
};
extern struct Dst data_ov071_02122ecc_d;

void __sinit_ov071_02122a64(void)
{
    func_02017acc(data_ov071_021230d0, 0x5b4);
    func_020731dc(data_ov071_021230d0, &func_02017ab4, data_ov071_021230e0);
    func_02017b4c(data_ov071_021230d8, 0x5b5);
    func_020731dc(data_ov071_021230d8, &SharedFilePtr_Destruct_Clsn, data_ov071_021230ec);
    data_ov071_02122ecc_d.p0 = data_ov071_02122e74;
    data_ov071_02122ecc_d.p2 = data_ov071_02122e8c;
    data_ov071_02122ecc_d.p5 = data_ov071_02122e84;
    data_ov071_02122ecc_d.p7 = data_ov071_02122e7c;
}
