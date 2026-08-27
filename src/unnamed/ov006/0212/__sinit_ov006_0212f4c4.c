extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov006_02140330[];
extern char data_ov006_02140338[];
extern char data_ov006_02140340[];
extern char data_ov006_0214034c[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
void __sinit_ov006_0212f4c4(void) {
    func_02017acc(data_ov006_02140330, 0x217);
    func_020731dc(data_ov006_02140330, func_02017ab4, data_ov006_02140340);
    _ZN13SharedFilePtr9ConstructEj(data_ov006_02140338, 0x218);
    func_020731dc(data_ov006_02140338, SharedFilePtr_Destruct_Anim, data_ov006_0214034c);
}
