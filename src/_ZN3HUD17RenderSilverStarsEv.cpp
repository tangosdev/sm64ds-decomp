//cpp
extern "C" {
extern unsigned char CURR_PLAYER_ID[];
extern signed char NUM_VS_STARS_OBTAINED_PLAYER[];
extern int data_ov001_020abac8[];
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void *, int, int, int, int, void *);
void _ZN3HUD17RenderSilverStarsEv(void) {
    int sl = 0;
    unsigned short n = (unsigned short)(short)NUM_VS_STARS_OBTAINED_PLAYER[CURR_PLAYER_ID[0]];
    int sb;
    if (sl < n) {
        sb = 0x10;
        do {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (void *)data_ov001_020abac8, sb, 0xa, -1, 1, (void *)0);
            sl++;
            sb += 0x11;
        } while (sl < n);
    }
}
}
