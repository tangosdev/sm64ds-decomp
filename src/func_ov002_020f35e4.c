extern void SetSubBg1Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern unsigned char data_0209d454;

void func_ov002_020f35e4(char *c, int i)
{
    int off = i * 0x30;
    *(int *)(c + 0x140 + off) = 0xe0000;
    *(int *)(c + 0x144 + off) = 0x66000;
    *(int *)(c + 0x148 + off) = 0xe0000;
    *(int *)(c + 0x14c + off) = 0x66000;
    *((volatile unsigned int *) 0x4001000) = ((*((volatile unsigned int *) 0x4001000)) & (~0xe000)) | 0x4000;
    *((volatile unsigned short *) 0x4001048) = (unsigned short) (((((*((volatile unsigned short *) 0x4001048)) & (~0x3f00)) | 0x1700) & 0xFFFFFFFFFFFFFFFFull) | 0x2000);
    *((volatile unsigned short *) 0x400104a) = (unsigned short) (((((*((volatile unsigned short *) 0x400104a)) & (~0x3f)) | 0x11) & 0xFFFFFFFFFFFFFFFFull) | 0x20);
    {
        int a = *(int *)(c + 0x140 + off) >> 12;
        int d = *(int *)(c + 0x148 + off) >> 12;
        int e = *(int *)(c + 0x14c + off) >> 12;
        int b = *(int *)(c + 0x144 + off) >> 12;
        *((volatile unsigned short *) 0x4001042) = (unsigned short) (((a << 8) & 0xff00) | (d & 0xff));
        *((volatile unsigned short *) 0x4001046) = (unsigned short) (((b << 8) & 0xff00) | (e & 0xff));
    }
    *(int *)(c + 0x130 + off) = -0x60000;
    *(int *)(c + 0x134 + off) = -0x38000;
    SetSubBg1Offset(*(int *)(c + 0x130 + off) >> 12, *(int *)(c + 0x134 + off) >> 12);
    SetSubBg2Offset(*(int *)(c + 0x130 + off) >> 12, *(int *)(c + 0x134 + off) >> 12);
    data_0209d454 |= 6;
    *(unsigned char *)(c + 0x15e + off) = *(unsigned char *)(c + 0x15e + off) + 1;
}
