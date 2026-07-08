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

    extern u32 NEXT_ACTOR_UPDATE_FLAGS;
    extern u32 ACTOR_UPDATE_FLAGS;
    extern u16 STAGE_TIMER;
    extern u8 GAME_PAUSED;
    extern u8 PAUSE_MENU_TIMER;
    extern u8 CURRENT_GAMEMODE;
    extern u8 CONNECTION_ERROR;
    extern u8 data_0209fcc8;
    extern s32 DP_STATE;
    extern char* PLAYER_ARR[];
    extern u8 NUM_PLAYERS_2;
    extern u16 VS_MODE_COUNTDOWN_TIMER;
    extern u8 VS_MODE_COUNTDOWN;
    extern s32 GAME_SPEED_RELATED;
    extern u16 VS_MODE_TIMER_TIMER;
    extern u8 VS_TIME_UP;
    extern s8 NEXT_LEVEL_ID;
    extern s32 KS_FADER;
    extern u8 GAME_FROZEN;
    extern u8 VS_MODE_EXIT_STATE;
    extern u8 DAT_02092778;
    extern s8 LEVEL_ID;
    extern s8 CHECKPOINT_LEVEL_ID;
    extern u8 NEXT_ENTRANCE_ID;
    extern u8 PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE;
    extern u8 data_020a0e40;
    extern u16 BUTTON_INPUT_ARR[];
    extern u16 data_020a0e5a[];
    extern u8 PLAYER_TALKING;
    extern s32 SAVE_DATA[];
    extern s32 RUNNING_KUPPA_SCRIPT;
    extern u8 PAUSED_DURING_TIMER;
    extern u8 PAUSED_WITH_SELECT;
}

struct UnkVis {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};
extern UnkVis* SCENE_FADER;

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
extern Timer TIME_TIMER;
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
    ACTOR_UPDATE_FLAGS = NEXT_ACTOR_UPDATE_FLAGS;
    CheckCameraInput();
    CheckInput();
    if (STAGE_TIMER != 0)
        STAGE_TIMER -= 1;
    if (GAME_PAUSED == 2 && PAUSE_MENU_TIMER == 0)
        PS_Cleanup();
    ProcessKuppaScript();
    if (SCENE_FADER->v5() == 0) {
        ShadowModel::CleanAll();
        return 1;
    }
    {
        int b = (CURRENT_GAMEMODE == 1);
        if (b == 0) {
            UpdateMessage();
        } else {
            u8 c9c = CONNECTION_ERROR;
            if (c9c != 0) {
                func_02032f54();
                if (data_0209fcc8 >= 7u)
                    func_020199a4();
                return 1;
            }
            if (DP_STATE != 0) {
                int b2 = (DP_STATE == 6);
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
                    char* p = PLAYER_ARR[i];
                    if (p != 0) {
                        int t = (*(u8*)(p + 0x711) != 0) ? 1 : 0;
                        if (t == 1)
                            cnt++;
                    }
                }
                if (cnt >= NUM_PLAYERS_2) {
                    if (VS_MODE_COUNTDOWN_TIMER == 0x28 && VS_MODE_COUNTDOWN == 3)
                        func_02012790(0x2b);
                    if (GAME_PAUSED == 0) {
                        if (VS_MODE_COUNTDOWN_TIMER != 0 || VS_MODE_COUNTDOWN != 0) {
                            VS_MODE_COUNTDOWN_TIMER = VS_MODE_COUNTDOWN_TIMER - GAME_SPEED_RELATED;
                            if (VS_MODE_COUNTDOWN_TIMER == 0 && VS_MODE_COUNTDOWN != 0) {
                                VS_MODE_COUNTDOWN -= 1;
                                VS_MODE_COUNTDOWN_TIMER = 0x28;
                                if (VS_MODE_COUNTDOWN != 0) {
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
            if (VS_MODE_TIMER_TIMER == 0 && VS_TIME_UP != 0) {
                if (DP_STATE == 0) {
                    if (NEXT_LEVEL_ID < 0 && KS_FADER == 0) {
                        if (GAME_FROZEN == 0 && VS_MODE_EXIT_STATE == 0)
                            VE_Init();
                        else
                            VE_Update();
                    }
                } else {
                    Scene::StartSceneFade(7, 0, 0);
                    DAT_02092778 = 1;
                    KS_FADER = 0;
                }
                Sound::StopLoadedMusic_Layer1(0x3c);
            }
        }
    }
    if (NEXT_LEVEL_ID >= 0) {
        int lvl = SublevelToLevel(NEXT_LEVEL_ID);
        int lvl2 = SublevelToLevel(LEVEL_ID);
        int bb = (CURRENT_GAMEMODE == 2);
        if (bb == 0 && lvl <= 0xe && lvl != lvl2 && CHECKPOINT_LEVEL_ID < 0
            && (NEXT_LEVEL_ID != 0xc || NEXT_ENTRANCE_ID != 4)) {
            Scene::SetSceneToSpawn(4, 0);
        } else {
            Scene::SetSceneToSpawn(3, 0);
        }
        if ((IsLevelInsideCastle(LEVEL_ID) == 0 || IsLevelInsideCastle(NEXT_LEVEL_ID) == 0)
            && (IsLevelTinyHugeIslandOutside(LEVEL_ID) == 0 || IsLevelTinyHugeIslandOutside(NEXT_LEVEL_ID) == 0)) {
            int bb2 = (CURRENT_GAMEMODE == 2);
            if (bb2 == 0)
                Sound::StopLoadedMusic_Layer1(0x3c);
        }
    } else if (PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE != 0) {
        LC_Update();
    } else if (GAME_PAUSED != 0) {
        PS_Update();
    } else {
        u32 pi = data_020a0e40;
        u16 h1 = *(u16*)((char*)BUTTON_INPUT_ARR + pi * 4);
        if ((((h1 & 0x200) == 0 && (h1 & 0x100) == 0) || (*(u16*)((char*)data_020a0e5a + pi * 4) & 8) == 0)
            && PLAYER_TALKING == 0 && KS_FADER == 0
            && CanPause() != 0 && IsPauseDisabled() == 0
            && (u8)(GAME_FROZEN | (GAME_PAUSED | PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE)) == 0
            && MaskOff(NEXT_ACTOR_UPDATE_FLAGS, 0) == 0) {
            u8 st = CURRENT_GAMEMODE;
            int b1 = (st == 1);
            if ((b1 && DP_STATE == 0) || (SAVE_DATA[2] & 0x80)) {
                int b2 = (st == 2);
                if (b2 == 0) {
                    int b3 = (RUNNING_KUPPA_SCRIPT != 0);
                    if (b3 == 0) {
                        if (STAGE_TIMER == 0) {
                            u32 pj = data_020a0e40;
                            u16 h2 = *(u16*)((char*)data_020a0e5a + pj * 4);
                            int t8 = h2 & 8;
                            if ((t8 != 0 && (*(u16*)((char*)BUTTON_INPUT_ARR + pj * 4) & 4) == 0)
                                || (b1 == 0 && t8 == 0 && (h2 & 4) != 0)) {
                                GAME_PAUSED = 1;
                                func_02012790(2);
                                if (TIME_TIMER.started != 0) {
                                    TIME_TIMER.StopTimer();
                                    PAUSED_DURING_TIMER = 1;
                                }
                                if (*(u16*)((char*)BUTTON_INPUT_ARR + data_020a0e40 * 4) & 4)
                                    PAUSED_WITH_SELECT = 1;
                                else
                                    PAUSED_WITH_SELECT = 0;
                                PS_Init();
                                PS_Update();
                            }
                        }
                    }
                }
            }
        }
    }
    if ((u8)(GAME_FROZEN | (GAME_PAUSED | PAUSE_LEVEL_CLEAR_SAVE_MENU_ACTIVE)) == 0) {
        if ((NEXT_ACTOR_UPDATE_FLAGS & ~0x20000000) == 0)
            ShadowModel::CleanAll();
    }
    return 1;
}
