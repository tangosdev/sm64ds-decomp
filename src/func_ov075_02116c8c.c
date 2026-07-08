typedef unsigned int u32;
extern void PrepareVsMode(void);
extern void LoadLevelNoReturn(u32 levelID, u32 arg1, u32 arg2, u32 arg3);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
extern unsigned char data_0209b2e4[];
extern signed char data_ov075_0211c6ec[];
extern unsigned char DAT_02092778[];
void func_ov075_02116c8c(void) {
    PrepareVsMode();
    unsigned char idx = data_0209b2e4[0];
    LoadLevelNoReturn((u32)(signed int)data_ov075_0211c6ec[idx], 0, 2, 0);
    _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
    DAT_02092778[0] = 1;
}
