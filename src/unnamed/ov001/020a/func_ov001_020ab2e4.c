extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int);
extern unsigned char data_ov001_020ad62c[];
extern unsigned char data_ov001_020ad628[];
extern unsigned char data_ov001_020ad630[];
extern int data_ov001_020ad634[];
extern unsigned char data_ov001_020ad620[];
extern unsigned char data_ov001_020ad624[];

void func_ov001_020ab2e4(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        data_ov001_020ad634[i] = 0;
        data_ov001_020ad62c[i] = 0;
        data_ov001_020ad628[i] = 0;
        if (_ZN8SaveData19IsCharacterUnlockedEj(i))
            data_ov001_020ad628[i] |= 0x10;
        data_ov001_020ad630[i] = 0;
    }
    data_ov001_020ad620[0] = 0;
    data_ov001_020ad624[0] = 0;
}
