//cpp
extern "C" int _Z15ApproachLinear2Rsss(short *, short, short);
extern "C" int func_ov001_020ab550(void *);
extern unsigned char data_0209b2e4[];
extern "C" int func_ov075_02115e1c(char *c)
{
    if (*(int *)(c + 0x1c) == 0xd) {
        if (*(int *)(c + 0x20) == data_0209b2e4[0]) {
            _Z15ApproachLinear2Rsss((short *)(c + 6), 0x14, 8);
        } else {
            if (_Z15ApproachLinear2Rsss((short *)(c + 6), -0x24, 8) != 0)
                *(int *)(c + 0x20) = data_0209b2e4[0];
        }
    }
    return func_ov001_020ab550(c);
}
