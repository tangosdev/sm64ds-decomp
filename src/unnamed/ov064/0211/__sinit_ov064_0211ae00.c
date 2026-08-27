extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov064_0211c668[];
extern char data_ov064_0211c688[];
extern char data_ov064_0211c670[];
extern char data_ov064_0211c694[];
extern char data_ov064_0211c660[];
extern char data_ov064_0211c6a0[];
extern char data_ov064_0211c678[];
extern char data_ov064_0211c6ac[];
extern char data_ov064_0211c680[];
extern char data_ov064_0211c6b8[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
void __sinit_ov064_0211ae00(void) {
    func_02017acc(data_ov064_0211c668, 0x2df);
    func_020731dc(data_ov064_0211c668, func_02017ab4, data_ov064_0211c688);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c670, 0x2e0);
    func_020731dc(data_ov064_0211c670, SharedFilePtr_Destruct_Anim, data_ov064_0211c694);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c660, 0x2e1);
    func_020731dc(data_ov064_0211c660, SharedFilePtr_Destruct_Anim, data_ov064_0211c6a0);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c678, 0x2e2);
    func_020731dc(data_ov064_0211c678, SharedFilePtr_Destruct_Anim, data_ov064_0211c6ac);
    _ZN13SharedFilePtr9ConstructEj(data_ov064_0211c680, 0x2e3);
    func_020731dc(data_ov064_0211c680, SharedFilePtr_Destruct_Anim, data_ov064_0211c6b8);
}
