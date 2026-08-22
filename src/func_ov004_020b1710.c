#include "types.h"
extern s32 GetGameLanguage(void);
extern void Hud_RenderSprite(void* fn, s32 a, s32 b, s32 c, s32 d);
extern void func_ov004_020b1ea4(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
extern void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
extern void** data_ov004_020bbfa8[];
extern int data_0209b308[];

void func_ov004_020b1710(void* self, int x, int y, int v)
{
    void** t;
    int mode;
    int r;
    int flag;

    t = data_ov004_020bbfa8[GetGameLanguage()];
    mode = data_0209b308[2];
    flag = 0;

    if (mode == 5) {
        if (v < 0) return;
        func_ov004_020b1ea4(x - 0x18, y, v / 60, flag, -1, 1, 10);
        Hud_RenderSprite(t[7], x - 0x10, y, -1, -1);
        r = v % 60 * 100;
        func_ov004_020b1ea4(x - 8, y, r / 600, 0, -1, 1, 10);
        func_ov004_020b1ea4(x, y, r / 60 % 10, 0, -1, 1, 10);
        return;
    }

    if (mode == 3) flag = 1;

    switch (mode) {
    case 1:
        Hud_RenderSprite(t[9], 0x58, y, -1, -1);
        Hud_RenderSprite(t[1], 0x68, y, -1, -1);
        break;
    case 2:
        Hud_RenderSprite(t[2], 0x58, y, -1, -1);
        Hud_RenderSprite(t[1], 0x68, y, -1, -1);
        break;
    case 3:
        flag = 1;
        break;
    case 4:
        Hud_RenderSprite(t[19], 0x58, y, -1, -1);
        Hud_RenderSprite(t[1], 0x68, y, -1, -1);
        break;
    }

    if (flag == 1) x -= 4;
    if (v < 0) return;
    if (flag == 1) {
        func_ov004_020b1ea4(x, y, v, 0, -1, 1, 10);
        return;
    }
    func_ov004_020b2444(x, y, v, 0, -1, 1, 0);
}
