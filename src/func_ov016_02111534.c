typedef short s16;
extern void func_ov018_02111bf0(void *, void *);
extern int data_ov016_02114d8c[];
int func_ov016_02111534(char *c)
{
    if (*(unsigned short *)(c + 0x100) == 0) {
        *(int *)(c + 0x98) = 0;
        *(s16 *)(c + 0x92) = *(s16 *)(c + 0x428);
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x42a);
        *(s16 *)(c + 0x96) = *(s16 *)(c + 0x42c);
        *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
        *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
        *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);
        func_ov018_02111bf0(c, data_ov016_02114d8c);
    }
    return 1;
}
