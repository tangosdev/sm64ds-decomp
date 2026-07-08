typedef unsigned char u8;
extern void func_0201a9ec(void *p);
extern void func_02030838(void);
extern void func_02019ac4(void);
extern void func_0201a4bc(void);
extern void func_020132d8(void);
extern u8 data_0209d4ec;
extern char data_0209d574[];
extern int GAME_SPEED_RELATED;
extern u8 data_0209d4f0;
extern int data_0209d514;
extern u8 data_0209b480;
extern u8 CURRENT_GAMEMODE;

void func_020199a4(void)
{
    int sl = 0xb4;
    if (data_0209d4ec == 7) data_0209d4ec = 0;
    func_0201a9ec(data_0209d574);
    for (;;) {
        int b = (int)(CURRENT_GAMEMODE == 1) ? 0 : 1;
        if (b == 0) func_02030838();
        {
            int dt = GAME_SPEED_RELATED;
            if (sl > 0) sl -= dt;
            else func_02019ac4();
        }
        data_0209d4f0 = 1;
        func_0201a4bc();
        data_0209d4f0 = 0;
        data_0209d514 = 0;
        if (data_0209b480 != 0) func_020132d8();
    }
}
