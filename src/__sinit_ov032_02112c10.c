extern int func_02017acc(int *p, int n);
extern void func_020731dc(int *a, void *b, void *node);
extern void _ZN13SharedFilePtr9ConstructEj(int *p, unsigned int n);
extern void func_02017ab4(void);
extern void func_02017984(void);

extern int data_ov032_02113a40[];
extern int data_ov032_02113a64[];
extern int data_ov032_02113a50[];
extern int data_ov032_02113a70[];
extern int data_ov032_02113a48[];
extern int data_ov032_02113a58[];

struct P2 { int a, b; };
struct P4 { struct P2 lo, hi; };

extern struct P4 data_ov032_02113a8c;
extern struct P2 data_ov032_0211377c;
extern struct P2 data_ov032_021137ac;
extern struct P4 data_ov032_02113a9c;
extern struct P2 data_ov032_021137b4;
extern struct P2 data_ov032_021137a4;
extern struct P4 data_ov032_02113aac;
extern struct P2 data_ov032_0211379c;
extern struct P2 data_ov032_021137c4;
extern struct P4 data_ov032_02113abc;
extern struct P2 data_ov032_02113784;
extern struct P2 data_ov032_0211378c;
extern struct P4 data_ov032_02113a7c;
extern struct P2 data_ov032_02113794;
extern struct P2 data_ov032_021137bc;

void __sinit_ov032_02112c10(void) {
    func_02017acc(data_ov032_02113a40, 0x295);
    func_020731dc(data_ov032_02113a40, func_02017ab4, data_ov032_02113a64);
    _ZN13SharedFilePtr9ConstructEj(data_ov032_02113a50, 0x297);
    func_020731dc(data_ov032_02113a50, func_02017984, data_ov032_02113a70);
    _ZN13SharedFilePtr9ConstructEj(data_ov032_02113a48, 0x296);
    func_020731dc(data_ov032_02113a48, func_02017984, data_ov032_02113a58);
    data_ov032_02113a8c.lo = data_ov032_0211377c;
    data_ov032_02113a8c.hi = data_ov032_021137ac;
    data_ov032_02113a9c.lo = data_ov032_021137b4;
    data_ov032_02113a9c.hi = data_ov032_021137a4;
    data_ov032_02113aac.lo = data_ov032_0211379c;
    data_ov032_02113aac.hi = data_ov032_021137c4;
    data_ov032_02113abc.lo = data_ov032_02113784;
    data_ov032_02113abc.hi = data_ov032_0211378c;
    data_ov032_02113a7c.lo = data_ov032_02113794;
    data_ov032_02113a7c.hi = data_ov032_021137bc;
}
