extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int func_02017ab4[];
extern int SharedFilePtr_Destruct_Clsn[];

extern int data_ov092_02132540[];
extern int data_ov092_0213255c[];
extern int data_ov092_02132548[];
extern int data_ov092_02132550[];

struct P { int a, b; };
extern struct P data_ov092_02132268;
extern struct P data_ov092_02132280;
extern struct P data_ov092_02132258;
extern struct P data_ov092_02132240;
extern struct P data_ov092_02132250;
extern struct P data_ov092_02132260;
extern struct P data_ov092_02132248;
extern struct P data_ov092_02132278;
extern struct P data_ov092_02132270;
extern struct P data_ov092_02132568[9];

void __sinit_ov092_021320cc(void)
{
    func_02017acc(data_ov092_02132540, 0x3c9);
    func_020731dc(data_ov092_02132540, func_02017ab4, data_ov092_0213255c);
    func_02017b4c(data_ov092_02132548, 0x3ca);
    func_020731dc(data_ov092_02132548, SharedFilePtr_Destruct_Clsn, data_ov092_02132550);

    data_ov092_02132568[0] = data_ov092_02132268;
    data_ov092_02132568[1] = data_ov092_02132280;
    data_ov092_02132568[2] = data_ov092_02132258;
    data_ov092_02132568[3] = data_ov092_02132240;
    data_ov092_02132568[4] = data_ov092_02132250;
    data_ov092_02132568[5] = data_ov092_02132260;
    data_ov092_02132568[6] = data_ov092_02132248;
    data_ov092_02132568[7] = data_ov092_02132278;
    data_ov092_02132568[8] = data_ov092_02132270;
}
