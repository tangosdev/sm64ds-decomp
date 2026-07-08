extern unsigned char NUM_MINIMAP_SCALE_OBJS;
extern short *MINIMAP_SCALE_OBJS_PTR;

int GetMinimapScale(int idx) {
    if (idx >= 0) {
        if (idx < (int)NUM_MINIMAP_SCALE_OBJS) return MINIMAP_SCALE_OBJS_PTR[idx];
    }
    return 0xa00;
}
