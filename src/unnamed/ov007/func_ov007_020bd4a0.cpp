//cpp


extern void InitialiseVramGlobals(void);
extern int func_02045d10(void);
extern int func_02045cf0(void);
extern "C" {

extern void Texture_InitRegions(int, int);
void func_ov007_020bd4a0(int r0) {
    switch (r0) {
    case 0:
        InitialiseVramGlobals();
        break;
    case 1:
        Texture_InitRegions(func_02045d10(), func_02045cf0());
        break;
    }
}
}
