extern void func_ov006_020d893c(char *c);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020d669c(char *c);
extern void func_ov006_020d63ac(char *c);
extern void _ZN6Player23St_InYoshiMouth_CleanupEv(char *c);
extern void func_ov006_020d5dd4(char *c);
extern void func_ov006_020d5c60(char *c);
extern void func_ov006_020d5b00(char *c);

void func_ov006_020d907c(char *c)
{
    func_ov006_020d893c(c);
    *(short *)(c + 0x62e2) = 0;
    *(short *)(c + 0x62e4) = 0;
    *(short *)(c + 0x62e6) = 0;
    *(unsigned char *)(c + 0x62f6) = 0xff;
    *(unsigned char *)(c + 0x62f7) = 0;
    *(short *)(c + 0x62ea) = 0;
    *(unsigned char *)(c + 0x62f8) = 0;
    *(int *)(c + 0x62d8) = 0;
    *(short *)(c + 0x62f0) = 0;
    *(unsigned char *)(c + 0x62f9) = 0;
    *(unsigned char *)(c + 0x62fb) = 0;
    *(unsigned char *)(c + 0x62fc) = 0;
    func_ov004_020adb1c(0);
    func_ov006_020d669c(c);
    func_ov006_020d63ac(c);
    _ZN6Player23St_InYoshiMouth_CleanupEv(c);
    func_ov006_020d5dd4(c);
    func_ov006_020d5c60(c);
    func_ov006_020d5b00(c);
}
