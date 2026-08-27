extern unsigned char data_0209f2d8;
extern unsigned char data_ov002_02111148;
extern int GetMinimapID(int a, int b);

int func_02029600(int x)
{
    int b = (int)(data_0209f2d8 == 2);
    if (b == 0) {
        unsigned char saved = data_ov002_02111148;
        int r = GetMinimapID(x, -1);
        if (r == saved)
            return 1;
    }
    return 0;
}
