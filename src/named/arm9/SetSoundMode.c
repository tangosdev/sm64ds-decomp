extern unsigned char data_0209d4b4;
extern void func_02011d5c(int self);

void SetSoundMode(int mode)
{
    data_0209d4b4 = (unsigned char)mode;
    switch (mode) {
    case 0:
        func_02011d5c(1);
        break;
    case 1:
        func_02011d5c(2);
        break;
    default:
        func_02011d5c(0);
        break;
    }
}
