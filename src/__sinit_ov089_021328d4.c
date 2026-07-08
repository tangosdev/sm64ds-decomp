extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void func_02017984(void);

extern char BOWSER_KEY_MODEL_PTR[];
extern char data_ov089_02132cb0[];
extern char data_ov089_02132c78[];
extern char data_ov089_02132cbc[];
extern char data_ov089_02132c68[];
extern char data_ov089_02132cc8[];
extern char data_ov089_02132c58[];
extern char data_ov089_02132cd4[];
extern char data_ov089_02132c60[];
extern char data_ov089_02132ce0[];
extern char data_ov089_02132c40[];
extern char data_ov089_02132c98[];
extern char data_ov089_02132c70[];
extern char data_ov089_02132c80[];
extern char data_ov089_02132c48[];
extern char data_ov089_02132c8c[];

struct P2 { int a, b; };
extern struct P2 data_ov089_02132cec[];
extern struct P2 data_ov089_02132b28;
extern struct P2 data_ov089_02132b38;
extern struct P2 data_ov089_02132b00;
extern struct P2 data_ov089_02132b30;
extern struct P2 data_ov089_02132b20;
extern struct P2 data_ov089_02132b10;
extern struct P2 data_ov089_02132b08;
extern struct P2 data_ov089_02132b18;

void __sinit_ov089_021328d4(void)
{
    func_02017acc(BOWSER_KEY_MODEL_PTR, 0x44d);
    func_020731dc(BOWSER_KEY_MODEL_PTR, func_02017ab4, data_ov089_02132cb0);
    func_02017acc(data_ov089_02132c78, 0x49c);
    func_020731dc(data_ov089_02132c78, func_02017ab4, data_ov089_02132cbc);
    func_02017acc(data_ov089_02132c68, 0x49d);
    func_020731dc(data_ov089_02132c68, func_02017ab4, data_ov089_02132cc8);
    func_02017acc(data_ov089_02132c58, 0x49e);
    func_020731dc(data_ov089_02132c58, func_02017ab4, data_ov089_02132cd4);
    _ZN13SharedFilePtr9ConstructEj(data_ov089_02132c60, 0x44e);
    func_020731dc(data_ov089_02132c60, func_02017984, data_ov089_02132ce0);
    _ZN13SharedFilePtr9ConstructEj(data_ov089_02132c40, 0x44f);
    func_020731dc(data_ov089_02132c40, func_02017984, data_ov089_02132c98);
    _ZN13SharedFilePtr9ConstructEj(data_ov089_02132c70, 0x450);
    func_020731dc(data_ov089_02132c70, func_02017984, data_ov089_02132c80);
    _ZN13SharedFilePtr9ConstructEj(data_ov089_02132c48, 0x451);
    func_020731dc(data_ov089_02132c48, func_02017984, data_ov089_02132c8c);

    data_ov089_02132cec[0] = data_ov089_02132b28;
    data_ov089_02132cec[1] = data_ov089_02132b38;
    data_ov089_02132cec[2] = data_ov089_02132b00;
    data_ov089_02132cec[3] = data_ov089_02132b30;
    data_ov089_02132cec[4] = data_ov089_02132b20;
    data_ov089_02132cec[5] = data_ov089_02132b10;
    data_ov089_02132cec[6] = data_ov089_02132b08;
    data_ov089_02132cec[7] = data_ov089_02132b18;
}
