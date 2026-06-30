extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern void func_02017ab4();
extern void func_02017b34();
extern int data_ov002_0210e064[];
extern int data_ov002_0210e06c[];
extern int data_ov002_0210e05c[];
extern int data_ov002_0210e078[];
struct S8 { int a, b; };
extern struct S8 data_ov002_0210e084;
extern struct S8 data_ov002_02109a7c;
extern struct S8 data_ov002_02109a64;
extern struct S8 data_ov002_02109a6c;
extern struct S8 data_ov002_02109a84;
extern struct S8 data_ov002_02109a74;
extern struct S8 data_ov002_02109aac;
extern struct S8 data_ov002_02109a94;
extern struct S8 data_ov002_02109a9c;
extern struct S8 data_ov002_02109aa4;
extern struct S8 data_ov002_02109a8c;
void __sinit_ov002_02101738(void)
{
    struct S8 *d = &data_ov002_0210e084;
    func_02017acc(data_ov002_0210e064, 0x491);
    func_020731dc(data_ov002_0210e064, func_02017ab4, data_ov002_0210e06c);
    func_02017b4c(data_ov002_0210e05c, 0x492);
    func_020731dc(data_ov002_0210e05c, func_02017b34, data_ov002_0210e078);
    d[0] = data_ov002_02109a7c;
    d[1] = data_ov002_02109a64;
    d[2] = data_ov002_02109a6c;
    d[3] = data_ov002_02109a84;
    d[4] = data_ov002_02109a74;
    d[5] = data_ov002_02109aac;
    d[6] = data_ov002_02109a94;
    d[7] = data_ov002_02109a9c;
    d[8] = data_ov002_02109aa4;
    d[9] = data_ov002_02109a8c;
}
