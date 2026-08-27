extern int GetGameLanguage(void);
extern void func_ov004_020b1ea4(int x, int a1, int val, int a3, int a4, int mode, int fb);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);

extern int data_0209b308;
extern int *data_ov004_020bbfa8;

void func_ov004_020b1710(int a0, int x, int y, int val)
{
    int idx = GetGameLanguage();
    int mode = (&data_0209b308)[2];
    int *tbl = (int *)(&data_ov004_020bbfa8)[idx];
    int flag = 0;

    if (mode == 5) {
        int major;
        int rem;
        int pct;
        if (val < 0)
            return;
        major = val / 60;
        func_ov004_020b1ea4(x - 0x18, y, major, flag, -1, 1, 0xa);
        Hud_RenderSprite((void *)tbl[7], x - 0x10, y, -1, -1);

        rem = val % 60;
        pct = rem * 100;
        func_ov004_020b1ea4(x - 8, y, pct / 600, 0, -1, 1, 0xa);

        rem = val % 60;
        pct = rem * 100 / 60;
        func_ov004_020b1ea4(x, y, pct % 10, 0, -1, 1, 0xa);
        return;
    }

    if (mode == 3)
        flag = 1;
    switch (mode) {
    case 1:
        Hud_RenderSprite((void *)tbl[9], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    case 2:
        Hud_RenderSprite((void *)tbl[2], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    case 3:
        flag = 1;
        break;
    case 4:
        Hud_RenderSprite((void *)tbl[0x13], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    default:
        break;
    }

    if (flag == 1)
        x -= 4;
    if (val < 0)
        return;
    if (flag == 1)
        func_ov004_020b1ea4(x, y, val, 0, -1, 1, 0xa);
    else
        func_ov004_020b2444(x, y, val, 0, -1, 1, 0);
}
