//cpp
extern "C" unsigned char data_0209f2d8;
extern "C" unsigned char data_02075998[][7];
struct OvTbl { unsigned int t[7]; };
extern "C" OvTbl data_02075804[];

extern "C" int OVERLAY_60_ID;
extern "C" int OVERLAY_98_ID;
extern "C" int OVERLAY_102_ID;

void LoadOrUnloadObjectOverlays(void (*fn)(int), int idx) {
    unsigned int i;
    int b = (data_0209f2d8 == 1);
    if (b != 0) return;
    for (i = 0; i < 7; i++) {
        b = (data_0209f2d8 == 1);
        if (b == 0 || i == 6) {
            unsigned char v = data_02075998[idx][i];
            if (v != 0) {
                fn(data_02075804[i].t[v]);
            }
        }
    }
    if (idx == 0x24 || idx == 0x26 || idx == 0x28) {
        fn((int)&OVERLAY_60_ID);
        return;
    }
    b = (data_0209f2d8 == 1);
    if (b == 0) {
        fn((int)&OVERLAY_98_ID);
    }
    fn((int)&OVERLAY_102_ID);
}
