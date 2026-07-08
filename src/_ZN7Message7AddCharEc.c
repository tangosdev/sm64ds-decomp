//cpp
// NONMATCHING: register allocation (div=30). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern unsigned char data_0209d698;
extern void *data_0209d6f8;
extern unsigned char data_0209d6c4;
extern unsigned char data_0209d678;
extern int data_0208eecc[];
extern int data_0209d704;
extern int data_0209d74c[];
extern unsigned char TALK_FONT_CHAR_WIDTHS[];
extern unsigned char data_0209d6a4;
extern unsigned char data_0209d6b0;
extern unsigned char data_0209d710[];
unsigned int func_02054e88(void);
int func_02054d88(void);
}

struct Message {
    static void AddChar(char c);
};

void Message::AddChar(char c)
{
    int i;
    int *ip;
    unsigned int word;
    unsigned int masked;
    int mask;
    int sh;

    if (data_0209d698 == 2)
        ip = (int *)((char *)data_0209d6f8 + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    else if (data_0209d698 == 1)
        ip = (int *)((char *)func_02054e88() + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    else
        ip = (int *)((char *)func_02054d88() + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));

    mask = data_0208eecc[data_0209d6c4];
    sh = data_0209d678;
    for (i = 0; i < 0x10; i++)
    {
        word = *ip++;
        masked = word & mask;
        if (sh != 0)
        {
            masked = (masked >> ((8 - sh) << 2)) | (masked << (sh << 2));
            data_0209d74c[data_0209d704] = masked;
        }
        else
        {
            data_0209d74c[data_0209d704] = masked;
        }
        if (i == 7)
            ip += 0xf8;
        data_0209d704 = data_0209d704 + 1;
    }

    {
        unsigned char width = TALK_FONT_CHAR_WIDTHS[c];
        data_0209d678 = (sh + width) & 7;
        data_0209d710[data_0209d6a4] = width;
        data_0209d6b0 = data_0209d6b0 + width;
        data_0209d6a4 = data_0209d6a4 + 1;
    }
}
