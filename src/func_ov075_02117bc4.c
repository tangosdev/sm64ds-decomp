typedef struct C C;
extern unsigned char TOP_SCREEN_RELATED[];
extern void UnloadOverlay(int id);
extern int overlay_64;
extern int overlay_66;

void func_ov075_02117bc4(C *c) {
    *TOP_SCREEN_RELATED &= ~0xe;
    *(int*)((char*)c + 0x264) = 0x14;
    UnloadOverlay((int)&overlay_64);
    UnloadOverlay((int)&overlay_66);
}
