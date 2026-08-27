extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov002_0210dfd4[];
extern int func_02017ab4[];
extern int data_ov002_0210dfdc[];
extern int data_ov002_0210dfc4[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov002_0210dfe8[];
extern int data_ov002_0210dfcc[];
extern int data_ov002_0210dff4[];
extern int data_ov002_0210dfbc[];
extern int data_ov002_0210e000[];
extern int data_ov002_021098b4[];
extern int data_ov002_021098bc[];
extern int data_ov002_0210989c[];
extern int data_ov002_02109884[];
extern int data_ov002_0210988c[];
extern int data_ov002_02109894[];
extern int data_ov002_0210987c[];
extern int data_ov002_021098c4[];
extern int data_ov002_021098ac[];
extern int data_ov002_021098a4[];

struct S2 { int a, b; };
extern struct S2 data_ov002_0210e00c[];

void __sinit_ov002_02101588(void)
{
    func_02017acc(data_ov002_0210dfd4, 0x45c);
    func_020731dc(data_ov002_0210dfd4, func_02017ab4, data_ov002_0210dfdc);
    func_02017b4c(data_ov002_0210dfc4, 0x45d);
    func_020731dc(data_ov002_0210dfc4, SharedFilePtr_Destruct_Clsn, data_ov002_0210dfe8);
    func_02017acc(data_ov002_0210dfcc, 0x48f);
    func_020731dc(data_ov002_0210dfcc, func_02017ab4, data_ov002_0210dff4);
    func_02017b4c(data_ov002_0210dfbc, 0x490);
    func_020731dc(data_ov002_0210dfbc, SharedFilePtr_Destruct_Clsn, data_ov002_0210e000);

    data_ov002_0210e00c[0] = *(struct S2*)data_ov002_021098b4;
    data_ov002_0210e00c[1] = *(struct S2*)data_ov002_021098bc;
    data_ov002_0210e00c[2] = *(struct S2*)data_ov002_0210989c;
    data_ov002_0210e00c[3] = *(struct S2*)data_ov002_02109884;
    data_ov002_0210e00c[4] = *(struct S2*)data_ov002_0210988c;
    data_ov002_0210e00c[5] = *(struct S2*)data_ov002_02109894;
    data_ov002_0210e00c[6] = *(struct S2*)data_ov002_0210987c;
    data_ov002_0210e00c[7] = *(struct S2*)data_ov002_021098c4;
    data_ov002_0210e00c[8] = *(struct S2*)data_ov002_021098ac;
    data_ov002_0210e00c[9] = *(struct S2*)data_ov002_021098a4;
}
