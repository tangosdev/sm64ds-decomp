//cpp
// NONMATCHING: different op / idiom (div=112). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Obj {
  virtual void v0(); virtual void v1(); virtual void v2();
  virtual void m_c(int a, int b);
  virtual void m_10(int a, int b);
  virtual int v5();
  virtual int m_18();
  virtual int m_1c();
};
extern Obj* SCENE_FADER;

extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" int IsButtonInputValid(void);
extern "C" int SublevelToLevel(int i);
extern "C" void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
extern "C" void _ZN7Message28DisplayStarNameForStarSelectEj(unsigned int id);
extern "C" void func_02012790(int a);
extern "C" int func_ov003_020adf50(char* c);
extern "C" void func_ov003_020ae358(char* c);

extern signed char NEXT_LEVEL_ID;
extern int GAME_SPEED_RELATED;
extern short DEFAULT_SCENE_FADER;
extern unsigned char NEXT_HAT_CHARACTER_ARR;
extern unsigned char NEXT_HAT_CHARACTER;
extern unsigned char SAVE_DATA[];
extern unsigned char data_020a0e40;
struct Row020a0de8 { unsigned char b0, b1, b2, b3; };
extern struct Row020a0de8 TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[][4];
extern unsigned short BUTTON_INPUT_ARR[];
extern unsigned short data_020a0e5a[];
extern unsigned char NEXT_STAR_ID;

#define U8(o)  (*(unsigned char*)(c+(o)))
#define U16(o) (*(unsigned short*)(c+(o)))
#define S16(o) (*(short*)(c+(o)))

extern "C" int func_ov003_020af038(char* c) {
    if (SCENE_FADER->v5() != 0) {
        DecIfAbove0_Byte((unsigned char*)(c + 0x117));
        DecIfAbove0_Byte((unsigned char*)(c + 0x118));
        if ((U8(0x119) != 0) || ((SublevelToLevel(NEXT_LEVEL_ID) > 0xE) && (IsButtonInputValid() != 0))) {
            if (U8(0x119) != 0) {
                *(unsigned char*)(((long long)(int)(c + 0x119)) & 0xFFFFFFFFFFFFFFFFLL) -= GAME_SPEED_RELATED;
            }
            if ((U8(0x119) == 0) || ((SublevelToLevel(NEXT_LEVEL_ID) > 0xE) && (IsButtonInputValid() != 0))) {
                _ZN5Scene14StartSceneFadeEjjt(3, 0, 0);
                *(short*)((char*)&DEFAULT_SCENE_FADER + 0xc) = 0x7fff;
                if (SublevelToLevel(NEXT_LEVEL_ID) <= 0xE) {
                    NEXT_STAR_ID = U8(0x115) + 1;
                } else {
                    NEXT_STAR_ID = 1;
                }
            }
        } else if ((U8(0x135) != 0) && (IsButtonInputValid() != 0)) {
            unsigned char temp_r1 = U8(0x133);
            if ((temp_r1 == 0) && ((unsigned)U8(0x130) > 1U) && (SAVE_DATA[0x41] == 3)) {
                U8(0x133) = 2;
                func_02012790(0x12E);
            } else if (U8(0x139) == 0) {
                if (temp_r1 == 1) {
                    unsigned char temp_r0 = (unsigned char)func_ov003_020adf50(c);
                    NEXT_HAT_CHARACTER_ARR = temp_r0;
                    NEXT_HAT_CHARACTER = temp_r0;
                    U8(0x132) = temp_r0;
                    U8(0x118) = (unsigned char)(GAME_SPEED_RELATED * 3);
                    U8(0x139) = 2;
                } else {
                    U8(0x133) = 2;
                    U8(0x132) = 3;
                    U8(0x118) = (unsigned char)(GAME_SPEED_RELATED * 6);
                    U8(0x139) = 1;
                }
                U8(0x119) = 0x10;
                func_02012790(SAVE_DATA[0x41] + 0x3C);
            }
        } else if (SublevelToLevel(NEXT_LEVEL_ID) <= 0xE) {
            int var_r4 = U8(0x115);
            int var_lr = 0;
            struct Row020a0de8 *row = &TOUCH_INPUT_ARR[data_020a0e40];
            unsigned char temp_r1_2 = row->b0;
            if (temp_r1_2 != 0) {
                unsigned char temp_r0_2 = U8(0x114);
                int var_ip = 0;
                if ((int)temp_r0_2 > 0) {
                    unsigned char row_b2 = row->b2;
                    unsigned char row_b3 = row->b3;
                    do {
                        if (((unsigned)(unsigned char)((row_b2 - *(unsigned char*)(c + var_ip + 0x11A)) + 8) < 0x10U)
                            && ((unsigned)row_b3 < 0x28U)
                            && ((U8(0x131) >> var_ip) & 1)) {
                            int var_r0;
                            if ((temp_r1_2 != 0) && (data_020a0de9[data_020a0e40][0] != 0)) {
                                var_r0 = 1;
                            } else {
                                var_r0 = 0;
                            }
                            if ((var_r0 != 0) || (var_r4 != var_ip)) {
                                U8(0x117) = (unsigned char)(GAME_SPEED_RELATED * 3);
                            }
                            if (var_r4 != var_ip) {
                                var_r4 = var_ip;
                                func_02012790(0x12E);
                            }
                            U8(0x133) = 0;
                            var_lr = 1;
                            break;
                        }
                        var_ip += 1;
                    } while (var_ip < (int)temp_r0_2);
                }
            }
            if ((var_lr == 0) && (U8(0x135) == 2) && (U8(0x133) == 0) && (SAVE_DATA[0x42] == 0)) {
                if (BUTTON_INPUT_ARR[0] & 0x20) {
                    int temp_r1_3 = BUTTON_INPUT_ARR[1] & 0x20;
                    if ((temp_r1_3 != 0) || (U16(0x104) == 0)) {
                        unsigned short var_r1;
                        if (temp_r1_3 != 0) var_r1 = 0x10; else var_r1 = 8;
                        U16(0x104) = var_r1;
                        if (var_r4 != 0) {
                            unsigned char temp_r1_4 = U8(0x131);
                            var_r4 -= 1;
                            if (!(((int)temp_r1_4 >> var_r4) & 1)) {
                                do {
                                    var_r4 -= 1;
                                } while (!(((int)temp_r1_4 >> var_r4) & 1));
                            }
                            func_02012790(0x12E);
                        }
                    }
                } else {
                    int temp_r1_5 = BUTTON_INPUT_ARR[1] & 0x10;
                    if ((BUTTON_INPUT_ARR[0] & 0x10) && ((temp_r1_5 != 0) || (U16(0x104) == 0))) {
                        unsigned short var_r1_2;
                        if (temp_r1_5 != 0) var_r1_2 = 0x10; else var_r1_2 = 8;
                        U16(0x104) = var_r1_2;
                        if (var_r4 < (int)U8(0x114) - 1) {
                            unsigned char temp_r1_6 = U8(0x131);
                            var_r4 += 1;
                            if (!(((int)temp_r1_6 >> var_r4) & 1)) {
                                do {
                                    var_r4 += 1;
                                } while (!(((int)temp_r1_6 >> var_r4) & 1));
                            }
                            func_02012790(0x12E);
                        }
                    }
                }
            }
            if (U8(0x115) != var_r4) {
                U8(0x115) = var_r4;
                _ZN7Message28DisplayStarNameForStarSelectEj((unsigned int)(short)var_r4);
                U8(0x117) = (unsigned char)(GAME_SPEED_RELATED * 3);
            }
            if (U8(0x139) == 0) {
                func_ov003_020ae358(c);
            }
        }
        if (SublevelToLevel(NEXT_LEVEL_ID) <= 0xE) {
            unsigned char temp_r0_3 = U8(0x135);
            if (temp_r0_3 == 0) {
                if ((IsButtonInputValid() != 0) || ((SAVE_DATA[0x42] == 0) && (*(unsigned short*)((char*)data_020a0e5a + data_020a0e40 * 4) & 0xF0))) {
                    func_02012790(0x12E);
                    if ((SAVE_DATA[0x42] == 0) && (BUTTON_INPUT_ARR[1] & 0x30)) {
                        U8(0x135) = 1;
                    } else {
                        U8(0x135) = 2;
                    }
                    if (SublevelToLevel(NEXT_LEVEL_ID) <= 0xE) {
                        U8(0x133) = 0;
                    } else if (((unsigned)U8(0x130) > 1U) && (SAVE_DATA[0x41] == 3)) {
                        U8(0x133) = 1;
                        if (U8(0x130) != 4) {
                            U8(0x134) = 0;
                        } else {
                            U8(0x134) = 1;
                        }
                    } else {
                        U8(0x133) = 2;
                    }
                }
            } else if (temp_r0_3 == 1) {
                if (BUTTON_INPUT_ARR[1] != 0) {
                    U8(0x135) = 2;
                }
            } else if ((SAVE_DATA[0x42] == 0) && (BUTTON_INPUT_ARR[0] & 0xC0)) {
                if (BUTTON_INPUT_ARR[1] & 0x40) {
                    if (U8(0x133) != 0) {
                        U8(0x133) -= 1;
                        if (U8(0x133) == 1) {
                            unsigned char temp_r2 = U8(0x130);
                            if (((unsigned)temp_r2 > 1U) && (SAVE_DATA[0x41] == 3)) {
                                if (temp_r2 != 4) {
                                    U8(0x134) = 0;
                                } else {
                                    U8(0x134) = 1;
                                }
                            } else {
                                U8(0x133) = 0;
                            }
                        }
                        func_02012790(0x12E);
                    }
                } else if ((BUTTON_INPUT_ARR[1] & 0x80) && (U8(0x133) != 2)) {
                    U8(0x133) += 1;
                    if (U8(0x133) == 1) {
                        unsigned char temp_r2_2 = U8(0x130);
                        if (((unsigned)temp_r2_2 > 1U) && (SAVE_DATA[0x41] == 3)) {
                            if (temp_r2_2 != 4) {
                                U8(0x134) = 0;
                            } else {
                                U8(0x134) = 1;
                            }
                        } else {
                            U8(0x133) = 2;
                        }
                    }
                    func_02012790(0x12E);
                }
            }
            if (((unsigned)U8(0x130) <= 1U) || (SAVE_DATA[0x41] != 3)) {
                if (U16(0x108) != 0) {
                    U16(0x108) -= 1;
                    if (U16(0x108) == 0) {
                        S16(0x10A) = 0;
                        S16(0x10E) = -0x400;
                        S16(0x10C) = S16(0x10E);
                        U8(0x136) = 1;
                    }
                } else {
                    if (!(U8(0x136) & 2)) {
                        *(short*)(((long long)(int)(c + 0x10C)) & 0xFFFFFFFFFFFFFFFFLL) += 0x100;
                        if (S16(0x10C) >= 0) {
                            U8(0x136) = 1;
                        }
                    } else {
                        *(short*)(((long long)(int)(c + 0x10C)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x100;
                        if (S16(0x10C) >= 0) {
                            U8(0x136) = 3;
                        }
                    }
                    *(short*)(((long long)(int)(c + 0x10A)) & 0xFFFFFFFFFFFFFFFFLL) += S16(0x10C);
                    unsigned char temp_r2_3 = U8(0x136);
                    if ((temp_r2_3 == 1) && (S16(0x10A) >= 0)) {
                        U8(0x136) = 2;
                    } else if ((temp_r2_3 == 3) && (S16(0x10A) <= 0)) {
                        *(short*)(((long long)(int)(c + 0x10E)) & 0xFFFFFFFFFFFFFFFFLL) += 0x80;
                        if (S16(0x10E) >= -0x180) {
                            U16(0x108) = 0x78;
                            S16(0x10A) = 0;
                        } else {
                            S16(0x10C) = S16(0x10E);
                            U8(0x136) = 1;
                        }
                    }
                }
            }
        }
    }
    DecIfAbove0_Short((unsigned short*)(c + 0x104));
    return 1;
}
