// NONMATCHING: register allocation (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
extern u8 NUM_BIG_BUTTONS;
extern u16 BG1_BUTTON_TILE_OFFSETS[];
extern u8 SELECTED_BUTTON;
extern u8 MENU_CHANGE_TIMER;
extern u8 GAME_PAUSED;
extern u8 PAUSE_MENU_ID;
extern unsigned short* _ZN3G2S12GetBG1ScrPtrEv(void);

void _ZN5Stage17UpdateMenuButtonsEb(int b)
{
    int i;
    for (i = 0; i < NUM_BIG_BUTTONS; i++) {
        u16* p;
        unsigned int v;
        int j;
        p = _ZN3G2S12GetBG1ScrPtrEv() + BG1_BUTTON_TILE_OFFSETS[i];
        if (b == 0) {
            if (SELECTED_BUTTON != i) {
                v = 0x1000;
            } else if (MENU_CHANGE_TIMER == 0) {
                v = 0x2000;
            } else {
                v = 0x1000;
            }
            v = (u16)v;
        } else {
            v = 0x1000;
        }
        if (GAME_PAUSED != 0) {
            u8 t = (u8)(PAUSE_MENU_ID + 0xf9);
            if (t <= 1) v = (u16)(v + 0x4000);
        }
        for (j = 0; j < 0x20; j++) {
            p[0] = (p[0] & 0x3ff) | v;
            p[0x20] = (p[0x20] & 0x3ff) | v;
            p[0x40] = (p[0x40] & 0x3ff) | v;
            p[0x60] = (p[0x60] & 0x3ff) | v;
            p++;
        }
    }
}
