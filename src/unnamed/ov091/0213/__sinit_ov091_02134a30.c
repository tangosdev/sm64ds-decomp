extern int func_02017acc(int *p, int n);
extern void func_020731dc(int *a, void *b, void *node);
extern void _ZN13SharedFilePtr9ConstructEj(int *p, unsigned int n);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern int data_ov091_02135674[];
extern int data_ov091_0213568c[];
extern int data_ov091_0213567c[];
extern int data_ov091_02135698[];
extern int data_ov091_02135684[];
extern int data_ov091_021356a4[];

struct P2 { int a, b; };
struct P4 { struct P2 lo, hi; };

extern struct P4 data_ov091_021356d0;
extern struct P2 data_ov091_02135364;
extern struct P2 data_ov091_0213535c;
extern struct P4 data_ov091_021356b0;
extern struct P2 data_ov091_0213534c;
extern struct P2 data_ov091_02135354;
extern struct P4 data_ov091_021356c0;
extern struct P2 data_ov091_02135344;
extern struct P2 data_ov091_0213533c;

void __sinit_ov091_02134a30(void) {
    func_02017acc(data_ov091_02135674, 0x32f);
    func_020731dc(data_ov091_02135674, func_02017ab4, data_ov091_0213568c);
    _ZN13SharedFilePtr9ConstructEj(data_ov091_0213567c, 0x330);
    func_020731dc(data_ov091_0213567c, SharedFilePtr_Destruct_Anim, data_ov091_02135698);
    _ZN13SharedFilePtr9ConstructEj(data_ov091_02135684, 0x331);
    func_020731dc(data_ov091_02135684, SharedFilePtr_Destruct_Anim, data_ov091_021356a4);
    data_ov091_021356d0.lo = data_ov091_02135364;
    data_ov091_021356d0.hi = data_ov091_0213535c;
    data_ov091_021356b0.lo = data_ov091_0213534c;
    data_ov091_021356b0.hi = data_ov091_02135354;
    data_ov091_021356c0.lo = data_ov091_02135344;
    data_ov091_021356c0.hi = data_ov091_0213533c;
}
