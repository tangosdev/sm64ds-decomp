typedef unsigned short u16;

extern unsigned char data_02086484[];

u16 func_0205b63c(int x)
{
    unsigned char b;
    int sel;
    if (x < -0x2d3)
        x = -0x2d3;
    else if (x > 0)
        x = 0;
    b = data_02086484[x + 0x2d3];
    if (x < -0xf0)
        sel = 3;
    else if (x < -0x78)
        sel = 2;
    else if (x < -0x3c)
        sel = 1;
    else
        sel = 0;
    return (u16)(b | (sel << 8));
}
