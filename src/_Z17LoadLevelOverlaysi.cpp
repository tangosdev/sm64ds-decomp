//cpp
extern "C" void Crash(void);
extern "C" void LoadOverlay(int id);
void LoadOrUnloadObjectOverlays(void (*load)(int), int level);

extern unsigned char data_0209f278;
extern int LOADED_LEVEL_OVL_ID;
extern int LEVEL_OVL_MAP[];

void LoadLevelOverlays(int level)
{
    if (data_0209f278) {
        if (LOADED_LEVEL_OVL_ID != LEVEL_OVL_MAP[level])
            Crash();
        data_0209f278 = 0;
        return;
    }
    int ov = LEVEL_OVL_MAP[level];
    if (ov != -1) {
        LoadOverlay(ov);
        LOADED_LEVEL_OVL_ID = ov;
    }
    LoadOrUnloadObjectOverlays(LoadOverlay, level);
}
