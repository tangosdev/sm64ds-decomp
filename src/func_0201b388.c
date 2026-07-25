#pragma opt_common_subs off

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 data_0209d668;
extern u8 data_0209d6c0;
extern u8 data_0209d6a8;
extern u8 data_0209d690;
extern u16 data_0209d6e0;
extern u8 data_0209d698;
extern u8 data_0209d65c;
extern u8 data_0209d6a4;
extern int data_0209d74c[];
extern u8 data_0209d710[];
extern int data_0208eeac[];
extern int data_0208ee8c[];

int _ZN3G2S13GetBG0CharPtrEv(void);
void *func_02054ea8(void);
unsigned int func_02054e88(void);
int func_02054d88(void);

void func_0201b388(int a0)
{
    int start;
    int *sl;
    int j;
    u32 sb;
    int i;
    int off;
    u8 n;
    int step;

    if (data_0209d668 != 0)
    {
        sl = (int *)(_ZN3G2S13GetBG0CharPtrEv() + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    }
    else if (data_0209d690 != 0)
    {
        sl = (int *)(_ZN3G2S13GetBG0CharPtrEv() + (data_0209d6e0 << 5));
    }
    else if (data_0209d698 == 2)
    {
        sl = (int *)(((char *)func_02054ea8()) + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    }
    else if (data_0209d698 == 1)
    {
        sl = (int *)(((char *)func_02054e88()) + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    }
    else
    {
        sl = (int *)(((char *)func_02054d88()) + ((data_0209d6c0 * data_0209d6a8 * 2 + 0x200) << 5));
    }

    start = data_0209d65c;
    n = data_0209d6a4;
    step = (a0 - 1) << 5;

    sb = (u32)(((long long)(int)start) & 0xFFFFFFFFFFFFFFFFLL);
    for (j = 0; j < 0x10; j++)
    {
        for (i = 0; i < n; i++)
        {
            int blk = sb >> 3;
            u8 col = sb & 7;
            int row = blk * 8;
            sl[row] = sl[row] | (data_0208eeac[col] & data_0209d74c[j + i * 16]);
            if (col != 0)
            {
                (sl + row)[8] = data_0208ee8c[col] & data_0209d74c[j + i * 16];
            }
            sb = (sb + data_0209d710[i]) & 0xff;
        }
        sl++;
        if (j == 7)
        {
            sl = (int *)(((char *)sl) + step);
        }
        sb = start;
    }

    data_0209d6c0 = data_0209d6c0 + 1;
}