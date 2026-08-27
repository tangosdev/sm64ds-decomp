extern unsigned char data_0209d454;
void func_ov004_020b91fc(char* c)
{
    data_0209d454 &= ~2;
    *(int*)(c + 0x120) = 0;
    *(unsigned char*)(c + 0x124) = 0;
}
