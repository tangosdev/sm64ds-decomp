//cpp
extern "C" {
int func_02017acc(void*, unsigned int);
int func_020731dc(void*, void*, void*);
int SharedFilePtr_Construct_TexSeq(void*, unsigned int);
int _ZN13SharedFilePtr9ConstructEj(void*, unsigned int);

extern char data_ov070_021235fc[];
extern char data_ov070_02123614[];
extern char data_ov070_02123604[];
extern char data_ov070_02123620[];
extern char data_ov070_021235f4[];
extern char data_ov070_0212362c[];
extern char data_ov070_0212360c[];
extern char data_ov070_02123638[];
extern char data_ov070_021235ec[];
extern char data_ov070_02123650[];
extern char func_02017ab4[];
extern char SharedFilePtr_Destruct_Anim[];
extern char SharedFilePtr_Destruct_TexSeq[];
extern char func_020072c0[];
extern char data_ov070_02123644[];

struct S2 { int w[2]; };
extern struct S2 data_ov070_0212320c;
extern struct S2 data_ov070_02123224;
extern struct S2 data_ov070_02123214;
extern struct S2 data_ov070_02123234;
extern struct S2 data_ov070_0212321c;
extern struct S2 data_ov070_0212322c;
struct D { struct S2 a, b, c, d, e, f; };
extern struct D data_ov070_02123668;

extern int data_ov070_0212365c[];

void __sinit_ov070_02122d80(void) {
    func_02017acc(data_ov070_021235fc, 0x2b1);
    func_020731dc(data_ov070_021235fc, func_02017ab4, data_ov070_02123614);
    func_02017acc(data_ov070_02123604, 0x2b3);
    func_020731dc(data_ov070_02123604, func_02017ab4, data_ov070_02123620);
    _ZN13SharedFilePtr9ConstructEj(data_ov070_021235f4, 0x2b2);
    func_020731dc(data_ov070_021235f4, SharedFilePtr_Destruct_Anim, data_ov070_0212362c);
    _ZN13SharedFilePtr9ConstructEj(data_ov070_0212360c, 0x2b5);
    func_020731dc(data_ov070_0212360c, SharedFilePtr_Destruct_Anim, data_ov070_02123638);
    SharedFilePtr_Construct_TexSeq(data_ov070_021235ec, 0x2b4);
    func_020731dc(data_ov070_021235ec, SharedFilePtr_Destruct_TexSeq, data_ov070_02123650);

    data_ov070_02123668.a = data_ov070_0212320c;
    data_ov070_02123668.b = data_ov070_02123224;
    data_ov070_02123668.c = data_ov070_02123214;
    data_ov070_02123668.d = data_ov070_02123234;
    data_ov070_02123668.e = data_ov070_0212321c;
    data_ov070_02123668.f = data_ov070_0212322c;

    data_ov070_0212365c[0] = 0;
    data_ov070_0212365c[1] = -0x28000;
    data_ov070_0212365c[2] = 0;
    func_020731dc(data_ov070_0212365c, func_020072c0, data_ov070_02123644);
}
}
