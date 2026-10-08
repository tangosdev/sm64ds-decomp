extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov002_0210e104[];
extern char data_ov002_0210e10c[];
extern char data_ov002_0210e0fc[];
extern char data_ov002_0210e118[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
void __sinit_ov002_02101900(void) {
    func_02017acc(data_ov002_0210e104, 0x470);
    func_020731dc(data_ov002_0210e104, func_02017ab4, data_ov002_0210e10c);
    _ZN13SharedFilePtr9ConstructEj(data_ov002_0210e0fc, 0x471);
    func_020731dc(data_ov002_0210e0fc, SharedFilePtr_Destruct_Anim, data_ov002_0210e118);
}
