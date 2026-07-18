typedef int s32;
s32 func_ov004_020ad674(void);
void func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4);
extern char* data_ov004_020bbfa8[];

#define LAU(p) ((int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov004_020b0de0(char* c)
{
    if (*(unsigned char*)(c + 0xc3) == 0)
        return;
    if ((unsigned int)*(unsigned char*)(c + 0xc4) < 4U) {
        unsigned short* p = (unsigned short*)LAU(c + 0xc0);
        *p = (unsigned short)(*p + 1);
        if ((unsigned int)*(unsigned short*)(c + 0xc0) >= 0x30U) {
            *(unsigned short*)(c + 0xc0) = 0U;
            {
                unsigned char* q = (unsigned char*)LAU(c + 0xc4);
                *q = (unsigned char)(*q + 1);
            }
        }
        if ((unsigned int)*(unsigned short*)(c + 0xc0) >= 0x18U)
            return;
    }
    {
        int i = func_ov004_020ad674();
        func_ov004_020afa20(*(int*)(data_ov004_020bbfa8[i] + 0x28), 0xc0, 0xb0, -1, -1);
    }
}
