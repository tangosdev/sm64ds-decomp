//cpp
extern "C" {
extern unsigned char VS_MODE_COUNTDOWN;
extern unsigned char GAME_PAUSED;
extern unsigned char PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;
extern unsigned char GAME_FROZEN;
extern unsigned short VS_MODE_TIMER_TIMER;
extern int GAME_SPEED_RELATED;
extern unsigned char VS_TIME_UP;
extern unsigned char BOTTOM_SCREEN_RELATED;
void func_02012790(int);
}

struct HUD {
    void UpdateVsTimer();
};

void HUD::UpdateVsTimer()
{
    unsigned short *q;
    unsigned int v;

    if (VS_MODE_COUNTDOWN != 0)
        return;
    if (((GAME_PAUSED | PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE | GAME_FROZEN) & 0xff) != 0)
        return;

    if (VS_MODE_TIMER_TIMER != 0)
        VS_MODE_TIMER_TIMER = VS_MODE_TIMER_TIMER - GAME_SPEED_RELATED;

    if (VS_TIME_UP != 0)
        return;

    if (VS_MODE_TIMER_TIMER == 0)
    {
        VS_MODE_TIMER_TIMER = 0xb4;
        q = (unsigned short *)(((int)this + 0x60) & 0xFFFFFFFFFFFFFFFFULL);
        *q = *q - 1;
        v = *(unsigned short *)((char *)this + 0x60);
        if (v <= 3 && v != 0)
            func_02012790(0x28);
        if (*(unsigned short *)((char *)this + 0x60) == 0)
        {
            VS_TIME_UP = 1;
            VS_MODE_TIMER_TIMER = 0x3c;
            func_02012790(0x29);
            BOTTOM_SCREEN_RELATED &= ~4;
        }
    }

    if (*(unsigned short *)((char *)this + 0x60) > 3)
        return;
    if (*(unsigned short *)((char *)this + 0x66) < 0x30)
    {
        q = (unsigned short *)(((int)this + 0x66) & 0xFFFFFFFFFFFFFFFFULL);
        *q = *q + 4;
    }
    else
    {
        *(unsigned short *)((char *)this + 0x66) = 0x38;
    }
}
