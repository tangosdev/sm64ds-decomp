// NONMATCHING: register allocation (div=30). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned char data_0209fca8;
extern void *data_0209fd10;
extern unsigned char data_0209fcd4;
extern unsigned char data_0209fccc;
extern int data_0209285c[];
extern int data_0209fd1c;
extern int data_0209fd5c[];
extern unsigned char TALK_FONT_CHAR_WIDTHS[];
extern unsigned char data_0209fcd8;
extern unsigned char data_0209fc88;
extern unsigned char data_0209fd20[];
unsigned int func_02054e88(void);
int func_02054d88(void);

void func_02031b84(char c)
{
    int i;
    int *ip;
    unsigned int word;
    unsigned int masked;
    int mask;
    int sh;

    if (data_0209fca8 == 2)
        ip = (int *)((char *)data_0209fd10 + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    else if (data_0209fca8 == 1)
        ip = (int *)((char *)func_02054e88() + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    else
        ip = (int *)((char *)func_02054d88() + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));

    mask = data_0209285c[data_0209fcd4];
    sh = data_0209fccc;
    for (i = 0; i < 0x10; i++)
    {
        word = *ip++;
        masked = word & mask;
        if (sh != 0)
        {
            masked = (masked >> ((8 - sh) << 2)) | (masked << (sh << 2));
            data_0209fd5c[data_0209fd1c] = masked;
        }
        else
        {
            data_0209fd5c[data_0209fd1c] = masked;
        }
        if (i == 7)
            ip += 0xf8;
        data_0209fd1c = data_0209fd1c + 1;
    }

    {
        unsigned char width = TALK_FONT_CHAR_WIDTHS[c];
        data_0209fccc = (sh + width) & 7;
        data_0209fd20[data_0209fcd8] = width;
        data_0209fc88 = data_0209fc88 + width;
        data_0209fcd8 = data_0209fcd8 + 1;
    }
}
