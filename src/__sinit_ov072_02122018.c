extern int func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern void _ZN7Vector3D1Ev(void);
extern char data_ov072_02122b20[];
extern char data_ov072_02122b28[];

struct Pair { int a, b; };
extern struct Pair data_ov072_02122750;
extern struct Pair data_ov072_02122728;
extern struct Pair data_ov072_02122720;
extern struct Pair data_ov072_02122730;
extern struct Pair data_ov072_02122748;
extern struct Pair data_ov072_02122738;
extern struct Pair data_ov072_02122740;
extern struct Pair data_ov072_02122778;
extern struct Pair data_ov072_02122770;
extern struct Pair data_ov072_02122758;
extern struct Pair data_ov072_02122768;
extern struct Pair data_ov072_02122760;

struct Dst {
    struct Pair p0;
    struct Pair p1;
    struct Pair p2;
    struct Pair p3;
    struct Pair p4;
    struct Pair p5;
    struct Pair p6;
    struct Pair p7;
    struct Pair p8;
    struct Pair p9;
    struct Pair p10;
    struct Pair p11;
};
extern struct Dst data_ov072_02122b64;

extern int data_ov072_02122b40[];
extern int data_ov072_02122b58[];

extern int data_ov072_02122b34[];
extern int data_ov072_02122b4c[];

void __sinit_ov072_02122018(void)
{
    func_02017acc(data_ov072_02122b20, 0x2ad);
    func_020731dc(data_ov072_02122b20, &func_02017ab4, data_ov072_02122b28);
    data_ov072_02122b64.p0 = data_ov072_02122750;
    data_ov072_02122b64.p1 = data_ov072_02122728;
    data_ov072_02122b64.p2 = data_ov072_02122720;
    data_ov072_02122b64.p3 = data_ov072_02122730;
    data_ov072_02122b64.p4 = data_ov072_02122748;
    data_ov072_02122b64.p5 = data_ov072_02122738;
    data_ov072_02122b64.p6 = data_ov072_02122740;
    data_ov072_02122b64.p7 = data_ov072_02122778;
    data_ov072_02122b64.p8 = data_ov072_02122770;
    data_ov072_02122b64.p9 = data_ov072_02122758;
    data_ov072_02122b64.p10 = data_ov072_02122768;
    data_ov072_02122b64.p11 = data_ov072_02122760;
    data_ov072_02122b40[0] = -0xad0000;
    data_ov072_02122b40[1] = 0x290000;
    data_ov072_02122b40[2] = -0x9d0000;
    func_020731dc(data_ov072_02122b40, &_ZN7Vector3D1Ev, data_ov072_02122b34);
    data_ov072_02122b58[0] = 0xfeffc000;
    data_ov072_02122b58[1] = 0xffa6f000;
    data_ov072_02122b58[2] = 0x6fe000;
    func_020731dc(data_ov072_02122b58, &_ZN7Vector3D1Ev, data_ov072_02122b4c);
}
