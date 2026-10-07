typedef struct C C;
extern unsigned char data_0209d45c[];
extern void UnloadOverlay(int id);
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

void func_ov075_02117bc4(C *c) {
    *data_0209d45c &= ~0xe;
    *(int*)((char*)c + 0x264) = 0x14;
    UnloadOverlay((int)&OVERLAY_100_ID);
    UnloadOverlay((int)&OVERLAY_102_ID);
}
