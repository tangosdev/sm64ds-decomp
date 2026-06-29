extern void func_02017acc();
extern void func_020731dc();
extern void func_02017934();
extern int data_ov072_02122bc4[];
extern int data_ov072_02122bd4[];
extern int data_ov072_02122bcc[];
extern int func_02017ab4[];
extern int func_0201791c[];
extern int data_ov072_02122bdc[];
extern int data_ov072_02122be8[];
extern int data_ov072_02122bf4[];
extern int data_ov072_02122874[];
extern int data_ov072_02122854[];
extern int data_ov072_0212286c[];
extern int data_ov072_02122864[];
extern int data_ov072_0212285c[];
extern int data_ov072_02122844[];
extern int data_ov072_0212284c[];
extern int data_ov072_0212283c[];

struct S2 { int a, b; };
extern struct S2 data_ov072_02122c00[];

void __sinit_ov072_021221f8(void)
{
    func_02017acc(data_ov072_02122bc4, 0x2af);
    func_020731dc(data_ov072_02122bc4, func_02017ab4, data_ov072_02122bdc);
    func_02017934(data_ov072_02122bd4, 0x2ae);
    func_020731dc(data_ov072_02122bd4, func_0201791c, data_ov072_02122be8);
    func_02017934(data_ov072_02122bcc, 0x2b0);
    func_020731dc(data_ov072_02122bcc, func_0201791c, data_ov072_02122bf4);

    data_ov072_02122c00[0] = *(struct S2*)data_ov072_02122874;
    data_ov072_02122c00[1] = *(struct S2*)data_ov072_02122854;
    data_ov072_02122c00[2] = *(struct S2*)data_ov072_0212286c;
    data_ov072_02122c00[3] = *(struct S2*)data_ov072_02122864;
    data_ov072_02122c00[4] = *(struct S2*)data_ov072_0212285c;
    data_ov072_02122c00[5] = *(struct S2*)data_ov072_02122844;
    data_ov072_02122c00[6] = *(struct S2*)data_ov072_0212284c;
    data_ov072_02122c00[7] = *(struct S2*)data_ov072_0212283c;
}
