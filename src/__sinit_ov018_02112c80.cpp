//cpp
extern "C" {
int func_02017acc(void*, unsigned int);
int func_020731dc(void*, void*, void*);
int SharedFilePtr_Construct_TexSeq(void*, unsigned int);
int _ZN13SharedFilePtr9ConstructEj(void*, unsigned int);

extern char data_ov036_02113c00[];
extern char data_ov036_02113c28[];
extern char data_ov027_02113bf0[];
extern char data_ov027_02113c10[];
extern char data_ov036_02113c08[];
extern char data_ov018_02113c1c[];
extern char data_ov018_02113bf8[];
extern char data_ov018_02113c34[];
extern char func_ov030_02113be8[];
extern char data_ov018_02113c40[];
extern char func_02017ab4[];
extern char SharedFilePtr_Destruct_Anim[];
extern char SharedFilePtr_Destruct_TexSeq[];

struct S2 { int w[2]; };
extern struct S2 data_ov018_0211394c;
extern struct S2 data_ov018_02113974;
extern struct S2 data_ov018_02113954;
extern struct S2 data_ov018_02113964;
extern struct S2 data_ov018_0211395c;
extern struct S2 data_ov032_0211396c;
struct D { struct S2 a, b, c, d, e, f; };
extern struct D data_ov018_02113c4c;

void __sinit_ov018_02112c80(void) {
    func_02017acc(data_ov036_02113c00, 0x3fb);
    func_020731dc(data_ov036_02113c00, func_02017ab4, data_ov036_02113c28);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113bf0, 0x406);
    func_020731dc(data_ov027_02113bf0, SharedFilePtr_Destruct_Anim, data_ov027_02113c10);
    _ZN13SharedFilePtr9ConstructEj(data_ov036_02113c08, 0x408);
    func_020731dc(data_ov036_02113c08, SharedFilePtr_Destruct_Anim, data_ov018_02113c1c);
    SharedFilePtr_Construct_TexSeq(data_ov018_02113bf8, 0x3fd);
    func_020731dc(data_ov018_02113bf8, SharedFilePtr_Destruct_TexSeq, data_ov018_02113c34);
    SharedFilePtr_Construct_TexSeq(func_ov030_02113be8, 0x407);
    func_020731dc(func_ov030_02113be8, SharedFilePtr_Destruct_TexSeq, data_ov018_02113c40);
    data_ov018_02113c4c.a = data_ov018_0211394c;
    data_ov018_02113c4c.b = data_ov018_02113974;
    data_ov018_02113c4c.c = data_ov018_02113954;
    data_ov018_02113c4c.d = data_ov018_02113964;
    data_ov018_02113c4c.e = data_ov018_0211395c;
    data_ov018_02113c4c.f = data_ov032_0211396c;
}
}
