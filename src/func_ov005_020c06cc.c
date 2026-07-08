typedef unsigned char u8;
extern u8 SCENE_FADER[];
extern u8 data_020a0e40[];
extern u8 TOUCH_INPUT_ARR[];
extern u8 data_020a0de9[];
extern u8 data_0209b304[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int func_ov005_020c00e4(void *this);
extern void func_02012790(int);

void func_ov005_020c06cc(char *c)
{
    int (**vt)(void*);
    void *obj = *(void**)SCENE_FADER;
    vt = *(int(***)(void*))obj;
    if (vt[5](obj) == 0) return;
    if (*(u8*)(c+0xac) != 0) return;
    if (*(u8*)(c+0x54) == 1) return;
    if (*(int*)(c+0x90) > 0) return;
    if (*(int*)(c+0x94) > 0) return;
    if (*(int*)(c+0x98) > 0) return;
    if (func_ov005_020c00e4(c) == 0) return;

    {
        u8 idx = data_020a0e40[0];
        int off = idx*4;
        int has = 0;
        if (TOUCH_INPUT_ARR[off]) {
            if (data_020a0de9[off]) has = 1;
        }
        if (has == 0) return;
        if (data_0209b304[0] == 0) {
            u8 v = data_020a0dea[idx*4];
            if (v < 0xd8) return;
            if (v > 0xf8) return;
            if (data_020a0deb[idx*4] < 0x40) return;
            if (data_020a0deb[idx*4] > 0x80) return;
            func_02012790(0);
            *(int*)(c+0x94) = 0x12;
            return;
        } else {
            u8 v = data_020a0dea[idx*4];
            if (v < 0x38) return;
            if (v > 0x58) return;
            if (data_020a0deb[idx*4] < 0x40) return;
            if (data_020a0deb[idx*4] > 0x80) return;
            func_02012790(0);
            *(int*)(c+0x90) = 0x12;
            return;
        }
    }
}
