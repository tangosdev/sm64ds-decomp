extern void func_02017acc();
extern void func_020731dc(void*, void*, void*);
extern void func_02017b4c();
extern int func_02017ab4[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int func_020072c0[];
extern int data_ov021_021149a0[];
extern int data_ov021_021149a8[];
extern int data_ov021_021149b0[];
extern int data_ov021_021149b8[];
extern int data_ov021_021149c0[];
extern int data_ov021_021149e4[];
extern int data_ov021_021149f0[];
extern int data_ov021_021149d8[];
extern int data_ov021_021149cc[];
extern int data_ov021_021149fc[];
extern int data_ov021_02114a08[];
extern int data_ov021_02114a14[];

typedef struct { int x, y, z; } V3;
extern V3 data_ov021_02114a20;
extern V3 data_ov021_02114a2c;
extern V3 data_ov021_02114a38;
extern V3 data_ov021_02114a44;

void __sinit_ov021_02113500(void)
{
    func_02017acc(data_ov021_021149a0, 0x5d7);
    func_020731dc(data_ov021_021149a0, func_02017ab4, data_ov021_021149c0);
    func_02017b4c(data_ov021_021149a8, 0x5d8);
    func_020731dc(data_ov021_021149a8, SharedFilePtr_Destruct_Clsn, data_ov021_02114a14);
    func_02017acc(data_ov021_021149b0, 0x5d5);
    func_020731dc(data_ov021_021149b0, func_02017ab4, data_ov021_021149e4);
    func_02017b4c(data_ov021_021149b8, 0x5d6);
    func_020731dc(data_ov021_021149b8, SharedFilePtr_Destruct_Clsn, data_ov021_021149f0);

    data_ov021_02114a20.x = 0;
    data_ov021_02114a20.y = 0x32000;
    data_ov021_02114a20.z = 0xc8000;
    func_020731dc(&data_ov021_02114a20, func_020072c0, data_ov021_021149fc);

    { volatile V3* p = (volatile V3*)&data_ov021_02114a2c; p->x = 0; p->y = 0x32000; p->z = -0xc8000; }
    func_020731dc(&data_ov021_02114a2c, func_020072c0, data_ov021_02114a08);

    { volatile V3* p = (volatile V3*)&data_ov021_02114a38; p->x = 0xc8000; p->y = 0x32000; p->z = 0; }
    func_020731dc(&data_ov021_02114a38, func_020072c0, data_ov021_021149d8);

    { volatile V3* p = (volatile V3*)&data_ov021_02114a44; p->x = -0xc8000; p->y = 0x32000; p->z = 0; }
    func_020731dc(&data_ov021_02114a44, func_020072c0, data_ov021_021149cc);
}
