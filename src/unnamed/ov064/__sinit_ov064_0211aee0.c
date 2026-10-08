extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov064_0211c6cc[];
extern char data_ov064_0211c6ec[];
extern char data_ov064_0211c6d4[];
extern char data_ov064_0211c6f8[];
extern char data_ov064_0211c6c4[];
extern char data_ov064_0211c704[];
extern char data_ov064_0211c6dc[];
extern char data_ov064_0211c710[];
extern char data_ov064_0211c6e4[];
extern char data_ov064_0211c71c[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
void __sinit_ov064_0211aee0(void) {
    func_02017acc(data_ov064_0211c6cc, 0x2de);
    func_020731dc(data_ov064_0211c6cc, func_02017ab4, data_ov064_0211c6ec);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c6d4, 0x2e0);
    func_020731dc(data_ov064_0211c6d4, SharedFilePtr_Destruct_Anim, data_ov064_0211c6f8);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c6c4, 0x2e1);
    func_020731dc(data_ov064_0211c6c4, SharedFilePtr_Destruct_Anim, data_ov064_0211c704);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c6dc, 0x2e2);
    func_020731dc(data_ov064_0211c6dc, SharedFilePtr_Destruct_Anim, data_ov064_0211c710);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c6e4, 0x2e3);
    func_020731dc(data_ov064_0211c6e4, SharedFilePtr_Destruct_Anim, data_ov064_0211c71c);
}
