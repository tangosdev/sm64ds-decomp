extern int func_ov060_0211469c(void *c);
extern void func_02012694(int a, char *b, int cnt);
extern int func_ov060_021145d4(void *c);
extern int func_ov060_02115a30(void *c);

void func_ov060_02114300(char *c)
{
    unsigned char k = *(unsigned char*)(c + 0x423);
    if (k == 0) {
        if (!func_ov060_0211469c(c)) return;
        {
            int v;
            *(int*)(c + 0xa8) = 0x32000;
            *(int*)(c + 0x98) = 0x19000;
            *(short*)(c + 0x3fe) = 0;
            v = *(unsigned char*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF) + 1;
            *(unsigned char*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF) = (unsigned char)v;
            func_02012694(0xb1, c + 0x74, v);
        }
    } else if (k == 1) {
        if (!func_ov060_021145d4(c)) return;
        {
            unsigned char *p = (unsigned char*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFF);
            *p = *p + 1;
        }
    } else {
        if (func_ov060_02115a30(c)) *(int*)(c + 0x40c) = 0;
    }
}
