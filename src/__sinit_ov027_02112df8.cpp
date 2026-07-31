//cpp
extern "C" {
int func_02017acc(void*, unsigned int);
int func_020731dc(void*, void*, void*);
int SharedFilePtr_Construct_TexSeq(void*, unsigned int);
int func_02017b4c(void*, unsigned int);
int _ZN13SharedFilePtr9ConstructEj(void*, unsigned int);

extern char data_ov027_02113c7c[];
extern char data_ov027_02113c84[];
extern char data_ov027_02113c74[];
extern char data_ov027_02113c8c[];
extern char data_ov027_02113c94[];
extern char data_ov027_02113c6c[];
extern char data_ov027_02113ca8[];
extern char data_ov027_02113c9c[];
extern char data_ov036_02113cb4[];
extern char data_ov036_02113cc0[];
extern char data_ov036_02113ccc[];
extern char data_ov027_02113cd8[];
extern char func_02017ab4[];
extern char func_02017984[];
extern char SharedFilePtr_Destruct_TexSeq[];
extern char SharedFilePtr_Destruct_Clsn[];

struct S2 { int w[2]; };
extern struct S2 data_ov027_021139d4;
extern struct S2 data_ov027_021139cc;
extern struct S2 data_ov027_021139dc;
extern struct S2 data_ov027_021139c4;
struct D { struct S2 a, b, c, d; };
extern struct D data_ov027_02113ce4;

void __sinit_ov027_02112df8(void) {
    func_02017acc(data_ov027_02113c7c, 0x3fb);
    func_020731dc(data_ov027_02113c7c, func_02017ab4, data_ov027_02113ca8);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c84, 0x406);
    func_020731dc(data_ov027_02113c84, func_02017984, data_ov027_02113c9c);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c74, 0x409);
    func_020731dc(data_ov027_02113c74, func_02017984, data_ov036_02113cb4);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c8c, 0x3fe);
    func_020731dc(data_ov027_02113c8c, func_02017984, data_ov036_02113cc0);
    SharedFilePtr_Construct_TexSeq(data_ov027_02113c94, 0x407);
    func_020731dc(data_ov027_02113c94, SharedFilePtr_Destruct_TexSeq, data_ov036_02113ccc);
    func_02017b4c(data_ov027_02113c6c, 0x3fc);
    func_020731dc(data_ov027_02113c6c, SharedFilePtr_Destruct_Clsn, data_ov027_02113cd8);
    data_ov027_02113ce4.a = data_ov027_021139d4;
    data_ov027_02113ce4.b = data_ov027_021139cc;
    data_ov027_02113ce4.c = data_ov027_021139dc;
    data_ov027_02113ce4.d = data_ov027_021139c4;
}
}
