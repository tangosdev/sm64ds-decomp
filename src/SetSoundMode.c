extern unsigned char data_0209d4b4;
extern void _Z13func_02011d5ci(int mode);

void SetSoundMode(int mode)
{
    data_0209d4b4 = mode;
    switch (mode) {
    case 0:
        _Z13func_02011d5ci(1);
        break;
    case 1:
        _Z13func_02011d5ci(2);
        break;
    default:
        _Z13func_02011d5ci(0);
        break;
    }
}
