extern int func_ov006_020e6e78(void *p);
extern void SetBg2Offset(int a, int b);
extern unsigned char TOP_SCREEN_RELATED[];

int func_ov006_021230e8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    if (!func_ov006_020e6e78(c)) return 0;
    if (*(int *)(c + 0x4628) == 0) {
        if (*(unsigned short *)(c + 0x4664) == 0) {
            TOP_SCREEN_RELATED[0] = TOP_SCREEN_RELATED[0] & ~8;
            SetBg2Offset(0, 0);
            *(int *)0x4000018 = 0;
        } else {
            TOP_SCREEN_RELATED[0] = TOP_SCREEN_RELATED[0] | 8;
            SetBg2Offset(0, 0xc0);
            *(int *)0x4000018 = 0xc00000;
        }
    }
    return 1;
}
