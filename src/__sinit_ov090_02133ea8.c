extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov090_02134524[];
extern char data_ov090_02134534[];
extern char data_ov090_0213452c[];
extern char data_ov090_02134540[];
extern void func_02017ab4(void);
extern void func_02017984(void);
struct P2 { int a, b; };
extern struct P2 data_ov090_021341ec;
extern struct P2 data_ov090_021341e4;
extern struct P2 data_ov090_0213454c[2];

void __sinit_ov090_02133ea8(void) {
    func_02017acc(data_ov090_02134524, 0x39d);
    func_020731dc(data_ov090_02134524, func_02017ab4, data_ov090_02134534);
    _ZN13SharedFilePtr9ConstructEj(data_ov090_0213452c, 0x39e);
    func_020731dc(data_ov090_0213452c, func_02017984, data_ov090_02134540);
    data_ov090_0213454c[0] = data_ov090_021341ec;
    data_ov090_0213454c[1] = data_ov090_021341e4;
}
