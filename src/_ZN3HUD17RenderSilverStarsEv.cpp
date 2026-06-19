//cpp
extern "C" {
extern unsigned char data_0209f250[];
extern signed char data_0209f310[];
extern int data_ov001_020abac8[];
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void *, int, int, int, int, void *);
void _ZN3HUD17RenderSilverStarsEv(void) {
    int sl = 0;
    unsigned short n = (unsigned short)(short)data_0209f310[data_0209f250[0]];
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
