extern int func_ov006_020e6e78(void *p);
extern void SetBg2Offset(int a, int b);
extern unsigned char data_0209d45c[];

int func_ov006_021230e8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    if (!func_ov006_020e6e78(c)) return 0;
    if (*(int *)(c + 0x4628) == 0) {
        if (*(unsigned short *)(c + 0x4664) == 0) {
            data_0209d45c[0] = data_0209d45c[0] & ~8;
            SetBg2Offset(0, 0);
            *(int *)0x4000018 = 0;
        } else {
            data_0209d45c[0] = data_0209d45c[0] | 8;
            SetBg2Offset(0, 0xc0);
            *(int *)0x4000018 = 0xc00000;
        }
    }
    return 1;
}
