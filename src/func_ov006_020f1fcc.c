typedef unsigned char u8;
typedef unsigned short u16;

extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern unsigned short data_ov006_0212e848[];

extern void func_ov006_020f1ef8(char *o, int p1);
extern void func_ov006_020f049c(char *c, int i);
extern void func_02012790(int a0);
extern void func_ov006_020f120c(char *base, int idx);
extern void func_020127a4(int a0, int a1, int a2, int a3);

void func_ov006_020f1fcc(char *c)
{
    int idx = data_020a0e40;
    int flag = 0;
    int cur;
    int i;

    if (data_020a0de8[idx][0] != 0) {
        if (data_020a0de9[idx][0] != 0) flag = 1;
    }
    if (flag == 0) return;

    cur = *(u8 *)(c + 0x5456);
    {
        int dx = data_020a0dea[idx][0] - (((int *)(c + 0x47f8))[cur - 1] >> 12);
        int dy = data_020a0deb[idx][0] - (((int *)(c + 0x49d8))[cur - 1] >> 12);
        if (dx <= 0x10 && dx >= -0x10 && dy <= 0x10 && dy >= -0x10) {
            int lvl, cat;

            func_ov006_020f1ef8(c, 1);
            cur = *(u8 *)(c + 0x5456);
            func_ov006_020f049c(c, cur - 1);

            lvl = *(int *)(c + 0xbc);
            cat = 0;
            if (lvl >= 0xa) cat = 2;
            else if (lvl >= 5) cat = 1;
            func_02012790(0x26);

            {
                int tbl = data_ov006_0212e848[cat];
                func_02012790(tbl + ((*(u8 *)(c + 0x545a)) << 3));
            }
            return;
        }
    }

    {
        int off = 0x51fd;

        for (i = 0; i < 0x78; i++) {
            if (*(u8 *)(c + i + 0x52ed) == 1) {
                u8 *p = (u8 *)(((long long)(int)(c + i + off)) & 0xFFFFFFFFFFFFFFFFLL);
                if (*p != 9) {
                    int dx2 = data_020a0de8[idx][2] - (((int *)(c + 0x47f8))[i] >> 12);
                    int dy2 = data_020a0de8[idx][3] - (((int *)(c + 0x49d8))[i] >> 12);
                    if (dx2 <= 0x10 && dx2 >= -0x10 && dy2 <= 0x10 && dy2 >= -0x10) {
                        u16 *arrC;
                        func_ov006_020f120c(c, i);
                        *(u8 *)(c + 0x5459) = 0;
                        *p = 9;
                        arrC = (u16 *)(c + 0x506c);
                        arrC[i] = 0x40;
                        if (*(u16 *)(c + 0x5172) != 0) {
                            int cur2 = *(u8 *)(c + 0x5456);
                            int t = (((int *)(c + 0x47f8))[cur2 - 1] >> 12) - 0x80;
                            int r3 = (t * 0x30) >> 7;
                            int val2b;
                            if (r3 >= 0x30) r3 = 0x30;
                            if (r3 <= -0x30) r3 = -0x30;
                            val2b = *(u8 *)(c + 0x545a);
                            func_020127a4(2, (val2b << 3) + 0xbf, 0xffff, r3);
                            func_02012790(0xe);
                        }
                        return;
                    }
                }
            }
        }
    }
}
