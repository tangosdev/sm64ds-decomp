extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, unsigned int);
extern void func_02017ab4(void);
extern void func_02017984(void);

extern void* data_ov080_021283c0;
extern void* data_ov080_02128408;
extern void* data_ov080_021283c8;
extern void* data_ov080_02128420;
extern void* data_ov080_021283e8;
extern void* data_ov080_0212842c;
extern void* data_ov080_021283d0;
extern void* data_ov080_02128414;
extern void* data_ov080_021283d8;
extern void* data_ov080_021283f0;
extern void* data_ov080_021283e0;
extern void* data_ov080_021283fc;

struct S2 { int w[2]; };
extern struct S2 data_ov080_02127fa0;
extern struct S2 data_ov080_02127f88;
extern struct S2 data_ov080_02127fa8;
extern struct S2 data_ov080_02127f80;
extern struct S2 data_ov080_02127f98;
extern struct S2 data_ov080_02127f90;
struct D { struct S2 a, b, c, d, e, f; };
extern struct D data_ov080_02128438;

void __sinit_ov080_021278c0(void)
{
    func_02017acc(&data_ov080_021283c0, 0x2d1);
    func_020731dc(&data_ov080_021283c0, (void*)&func_02017ab4, (void**)&data_ov080_02128408);
    func_02017acc(&data_ov080_021283c8, 0x2d6);
    func_020731dc(&data_ov080_021283c8, (void*)&func_02017ab4, (void**)&data_ov080_02128420);
    _ZN13SharedFilePtr9ConstructEj(&data_ov080_021283e8, 0x2d2);
    func_020731dc(&data_ov080_021283e8, (void*)&func_02017984, (void**)&data_ov080_0212842c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov080_021283d0, 0x2d3);
    func_020731dc(&data_ov080_021283d0, (void*)&func_02017984, (void**)&data_ov080_02128414);
    _ZN13SharedFilePtr9ConstructEj(&data_ov080_021283d8, 0x2d4);
    func_020731dc(&data_ov080_021283d8, (void*)&func_02017984, (void**)&data_ov080_021283f0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov080_021283e0, 0x2d5);
    func_020731dc(&data_ov080_021283e0, (void*)&func_02017984, (void**)&data_ov080_021283fc);
    data_ov080_02128438.a = data_ov080_02127fa0;
    data_ov080_02128438.b = data_ov080_02127f88;
    data_ov080_02128438.c = data_ov080_02127fa8;
    data_ov080_02128438.d = data_ov080_02127f80;
    data_ov080_02128438.e = data_ov080_02127f98;
    data_ov080_02128438.f = data_ov080_02127f90;
}
