//cpp
extern "C" void Crash(void);
extern "C" void LoadOverlay(int id);
void LoadOrUnloadObjectOverlays(void (*load)(int), int level);

extern unsigned char data_0209f278;
extern int data_02092130;
extern int data_020758c8[];

void LoadLevelOverlays(int level)
{
    if (data_0209f278) {
        if (data_02092130 != data_020758c8[level])
            Crash();
        data_0209f278 = 0;
        return;
    }
    int ov = data_020758c8[level];
    if (ov != -1) {
        LoadOverlay(ov);
        data_02092130 = ov;
    }
    LoadOrUnloadObjectOverlays(LoadOverlay, level);
}
