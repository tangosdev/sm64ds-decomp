extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov090_021345a4[];
extern char data_ov090_021345b4[];
extern char data_ov090_021345ac[];
extern char data_ov090_021345c0[];
extern void func_02017ab4(void);
extern void func_02017984(void);
struct P2 { int a, b; };
extern struct P2 data_ov090_021343a0;
extern struct P2 data_ov090_021343a8;
extern struct P2 data_ov090_021345cc[2];

void __sinit_ov090_02134020(void) {
    func_02017acc(data_ov090_021345a4, 0x325);
    func_020731dc(data_ov090_021345a4, func_02017ab4, data_ov090_021345b4);
    _ZN13SharedFilePtr9ConstructEj(data_ov090_021345ac, 0x326);
    func_020731dc(data_ov090_021345ac, func_02017984, data_ov090_021345c0);
    data_ov090_021345cc[0] = data_ov090_021343a0;
    data_ov090_021345cc[1] = data_ov090_021343a8;
}
