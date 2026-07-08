extern unsigned char BOTTOM_SCREEN_RELATED;
void func_ov004_020b91fc(char* c)
{
    BOTTOM_SCREEN_RELATED &= ~2;
    *(int*)(c + 0x120) = 0;
    *(unsigned char*)(c + 0x124) = 0;
}
