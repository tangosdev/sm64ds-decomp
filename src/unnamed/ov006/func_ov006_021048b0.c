extern void SetBg2Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);

void func_ov006_021048b0(char *o)
{
    *(unsigned char *)(o + 0x46a4) = 0;
    SetBg2Offset(0, 0);
    SetSubBg2Offset(0, 0);
}
