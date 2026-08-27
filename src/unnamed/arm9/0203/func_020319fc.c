extern unsigned char data_0209fc94;
extern unsigned char data_0209fcb0;
extern unsigned char data_0209fc78;
extern unsigned char data_0209fc7c;
extern unsigned char data_0209fcd8;
extern unsigned int data_0209fd5c[];
extern unsigned char data_0209fd20[];
extern unsigned int data_0209287c[];
extern unsigned int data_0209283c[];
extern char *_ZN3G2S13GetBG0CharPtrEv(void);
extern char *func_02054d88(void);

void func_020319fc(int n)
{
    unsigned int *dst;
    int i;
    unsigned int col;
    int j;
    int col0;
    int skip;
    int count;
    int blk;
    int wordIdx;
    unsigned char rem;

    if (data_0209fc94 != 0)
        dst = (unsigned int *)(_ZN3G2S13GetBG0CharPtrEv() + (((data_0209fcb0 * data_0209fc78 << 1) + 0x200) << 5));
    else
        dst = (unsigned int *)(func_02054d88() + (((data_0209fcb0 * data_0209fc78 << 1) + 0x200) << 5));

    skip = (n - 1) << 5;
    col0 = data_0209fc7c;
    count = data_0209fcd8;
    col = col0;

    for (i = 0; i < 0x10; i++) {
        for (j = 0; j < count; j++) {
            blk = col >> 3;
            wordIdx = blk << 3;
            rem = col & 7;
            dst[wordIdx] = (data_0209287c[rem] & data_0209fd5c[i + j * 0x10]) | dst[wordIdx];
            if (rem != 0)
                *(unsigned int *)((char *)&dst[wordIdx] + 0x20) =
                    *(unsigned int *)((char *)data_0209283c + (rem << 2)) &
                    *(unsigned int *)((char *)data_0209fd5c + ((i + j * 0x10) << 2));
            col = (col + data_0209fd20[j]) & 0xff;
        }
        dst++;
        if (i == 7)
            dst = (unsigned int *)((char *)dst + skip);
        col = col0;
    }
    data_0209fcb0++;
}
