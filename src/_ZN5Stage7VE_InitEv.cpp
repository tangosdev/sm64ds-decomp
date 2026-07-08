//cpp
extern "C" {
extern unsigned char TOP_SCREEN_RELATED[];
extern unsigned char BOTTOM_SCREEN_RELATED[];
extern unsigned short BG1_BUTTON_TILE_OFFSETS[];
extern unsigned char NUM_BIG_BUTTONS[];
extern unsigned char SELECTED_BUTTON[];
extern unsigned int NEXT_ACTOR_UPDATE_FLAGS[];
extern unsigned int ACTOR_UPDATE_FLAGS[];
extern unsigned char GAME_FROZEN[];
extern unsigned char VS_MODE_EXIT_STATE[];
extern void _ZN7Message17DisplayVsExitTextEt(unsigned short n);
extern void _ZN5Stage17UpdateMenuButtonsEb(bool b);
extern void SetBg2Offset(int a, int b);
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p, int val, short amt);

void _ZN5Stage7VE_InitEv(void)
{
    unsigned int a;
    unsigned int b;

    a = TOP_SCREEN_RELATED[0] ^ 0x10;
    b = BOTTOM_SCREEN_RELATED[0] ^ 0x10;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x1100;
    *(volatile unsigned short *)0x400000c = (*(volatile unsigned short *)0x400000c & 0x43) | 0x1010;
    _ZN7Message17DisplayVsExitTextEt(0x1f);
    BG1_BUTTON_TILE_OFFSETS[0] = 0xe0;
    BG1_BUTTON_TILE_OFFSETS[1] = 0x1a0;
    NUM_BIG_BUTTONS[0] = 2;
    SELECTED_BUTTON[0] = 0;
    _ZN5Stage17UpdateMenuButtonsEb(true);
    SetBg2Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    TOP_SCREEN_RELATED[0] |= 4;
    BOTTOM_SCREEN_RELATED[0] |= 3;
    NEXT_ACTOR_UPDATE_FLAGS[0] |= 0x40000000;
    ACTOR_UPDATE_FLAGS[0] = NEXT_ACTOR_UPDATE_FLAGS[0];
    GAME_FROZEN[0] = 1;
    VS_MODE_EXIT_STATE[0] = 0;
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4000050, a | 0x20, -7);
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4001050, b | 0x20, -7);
}
}
