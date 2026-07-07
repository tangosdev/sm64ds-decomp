//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef short s16;
typedef int s32;

extern "C" {
    void ProcessKuppaScript();
    void func_02032f54();
    void func_020199a4();
    void func_02019a58();
    void func_02012790(int a);
    void func_02011c24();
    void func_02030a78();
    void func_020345b0(int a);
    int SublevelToLevel(int i);
    int IsLevelInsideCastle(int level);
    int IsLevelTinyHugeIslandOutside(int level);

    extern u32 data_0209b454;
    extern u32 data_0209b464;
    extern u16 data_0209f300;
    extern u8 data_0209f2c4;
    extern u8 data_0209f22c;
    extern u8 data_0209f2d8;
    extern u8 data_0209fc9c;
    extern u8 data_0209fcc8;
    extern s32 data_0209fc68;
    extern char* data_0209f394[];
    extern u8 data_0209fc50;
    extern u16 data_0209f304;
    extern u8 data_0209f2bc;
    extern s32 data_0208ee44;
    extern u16 data_ov002_02111188;
    extern u8 data_0209f204;
    extern s8 data_02092110;
    extern s32 data_0209d4b0;
    extern u8 data_0209f294;
    extern u8 data_0209f290;
    extern u8 data_02092778;
    extern s8 data_0209f2f8;
    extern s8 data_02092118;
    extern u8 data_0209f268;
    extern u8 data_0209f20c;
    extern u8 data_020a0e40;
    extern u16 data_020a0e58[];
    extern u16 data_020a0e5a[];
    extern u8 data_0209d660;
    extern s32 data_0209caa0[];
    extern s32 data_0209fc48;
    extern u8 data_0209f2a0;
    extern u8 data_0209f218;
}

struct UnkVis {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};
extern UnkVis* data_0209f5bc;

class Sound {
public:
    static void LoadAndSetMusic_Layer1(int a);
    static void StopLoadedMusic_Layer1(unsigned int a);
};
class Scene {
public:
    static void StartSceneFade(unsigned int a, unsigned int b, unsigned short c);
    static void SetSceneToSpawn(unsigned int a, unsigned int b);
};
class Timer {
public:
    void StopTimer();
    u8 unk0[8];
    u8 started;
};
extern Timer data_0209d4c8;
class ShadowModel {
public:
    static void CleanAll();
};

class Stage {
public:
    static void CheckCameraInput();
    void CheckInput();
    static void PS_Cleanup();
    static void UpdateMessage();
    static void VE_Init();
    static void VE_Update();
    static void LC_Update();
    static void PS_Update();
    static int CanPause();
    static int IsPauseDisabled();
    static void PS_Init();
    int Behavior();
};

static inline u32 MaskOff(u32 x, u32 m) { return x & ~m; }

#pragma opt_common_subs off

int Stage::Behavior()
{
    data_0209b464 = data_0209b454;
    CheckCameraInput();
    CheckInput();
    if (data_0209f300 != 0)
        data_0209f300 -= 1;
    if (data_0209f2c4 == 2 && data_0209f22c == 0)
        PS_Cleanup();
    ProcessKuppaScript();
    if (data_0209f5bc->v5() == 0) {
        ShadowModel::CleanAll();
        return 1;
    }
    {
        int b = (data_0209f2d8 == 1);
        if (b == 0) {
            UpdateMessage();
        } else {
            u8 c9c = data_0209fc9c;
            if (c9c != 0) {
                func_02032f54();
                if (data_0209fcc8 >= 7u)
                    func_020199a4();
                return 1;
            }
            if (data_0209fc68 != 0) {
                int b2 = (data_0209fc68 == 6);
                if (b2) {
                    if (c9c == 0) {
                        func_02019a58();
                        func_02012790(0x123);
                        func_02011c24();
                        func_02030a78();
                        func_020345b0(0x13);
                    }
                    return 1;
                }
            }
            {
                int cnt = 0;
                int i;
                for (i = 0; i < 4; i++) {
                    char* p = data_0209f394[i];
                    if (p != 0) {
                        int t = (*(u8*)(p + 0x711) != 0) ? 1 : 0;
                        if (t == 1)
                            cnt++;
                    }
                }
                if (cnt >= data_0209fc50) {
                    if (data_0209f304 == 0x28 && data_0209f2bc == 3)
                        func_02012790(0x2b);
                    if (data_0209f2c4 == 0) {
                        if (data_0209f304 != 0 || data_0209f2bc != 0) {
                            data_0209f304 = data_0209f304 - data_0208ee44;
                            if (data_0209f304 == 0 && data_0209f2bc != 0) {
                                data_0209f2bc -= 1;
                                data_0209f304 = 0x28;
                                if (data_0209f2bc != 0) {
                                    func_02012790(0x2b);
                                } else {
                                    func_02012790(0x2a);
                                    Sound::LoadAndSetMusic_Layer1(0x4d);
                                }
                            }
                        }
                    }
                }
            }
            if (data_ov002_02111188 == 0 && data_0209f204 != 0) {
                if (data_0209fc68 == 0) {
                    if (data_02092110 < 0 && data_0209d4b0 == 0) {
                        if (data_0209f294 == 0 && data_0209f290 == 0)
                            VE_Init();
                        else
                            VE_Update();
                    }
                } else {
                    Scene::StartSceneFade(7, 0, 0);
                    data_02092778 = 1;
                    data_0209d4b0 = 0;
                }
                Sound::StopLoadedMusic_Layer1(0x3c);
            }
        }
    }
    if (data_02092110 >= 0) {
        int lvl = SublevelToLevel(data_02092110);
        int lvl2 = SublevelToLevel(data_0209f2f8);
        int bb = (data_0209f2d8 == 2);
        if (bb == 0 && lvl <= 0xe && lvl != lvl2 && data_02092118 < 0
            && (data_02092110 != 0xc || data_0209f268 != 4)) {
            Scene::SetSceneToSpawn(4, 0);
        } else {
            Scene::SetSceneToSpawn(3, 0);
        }
        if ((IsLevelInsideCastle(data_0209f2f8) == 0 || IsLevelInsideCastle(data_02092110) == 0)
            && (IsLevelTinyHugeIslandOutside(data_0209f2f8) == 0 || IsLevelTinyHugeIslandOutside(data_02092110) == 0)) {
            int bb2 = (data_0209f2d8 == 2);
            if (bb2 == 0)
                Sound::StopLoadedMusic_Layer1(0x3c);
        }
    } else if (data_0209f20c != 0) {
        LC_Update();
    } else if (data_0209f2c4 != 0) {
        PS_Update();
    } else {
        u32 pi = data_020a0e40;
        u16 h1 = *(u16*)((char*)data_020a0e58 + pi * 4);
        if ((((h1 & 0x200) == 0 && (h1 & 0x100) == 0) || (*(u16*)((char*)data_020a0e5a + pi * 4) & 8) == 0)
            && data_0209d660 == 0 && data_0209d4b0 == 0
            && CanPause() != 0 && IsPauseDisabled() == 0
            && (u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0
            && MaskOff(data_0209b454, 0) == 0) {
            u8 st = data_0209f2d8;
            int b1 = (st == 1);
            if ((b1 && data_0209fc68 == 0) || (data_0209caa0[2] & 0x80)) {
                int b2 = (st == 2);
                if (b2 == 0) {
                    int b3 = (data_0209fc48 != 0);
                    if (b3 == 0) {
                        if (data_0209f300 == 0) {
                            u32 pj = data_020a0e40;
                            u16 h2 = *(u16*)((char*)data_020a0e5a + pj * 4);
                            int t8 = h2 & 8;
                            if ((t8 != 0 && (*(u16*)((char*)data_020a0e58 + pj * 4) & 4) == 0)
                                || (b1 == 0 && t8 == 0 && (h2 & 4) != 0)) {
                                data_0209f2c4 = 1;
                                func_02012790(2);
                                if (data_0209d4c8.started != 0) {
                                    data_0209d4c8.StopTimer();
                                    data_0209f2a0 = 1;
                                }
                                if (*(u16*)((char*)data_020a0e58 + data_020a0e40 * 4) & 4)
                                    data_0209f218 = 1;
                                else
                                    data_0209f218 = 0;
                                PS_Init();
                                PS_Update();
                            }
                        }
                    }
                }
            }
        }
    }
    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0) {
        if ((data_0209b454 & ~0x20000000) == 0)
            ShadowModel::CleanAll();
    }
    return 1;
}
