//cpp
extern "C" {
extern int func_02029408(void);
extern void _ZN5Stage20RenderBouncingArrowsEv(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);
extern int SublevelToLevel(int);
extern unsigned char CountStarsCollectedInLevelToDisplay(int);
extern int IsStarCollected(int, int);
extern void _ZN3OAM9RenderSubEP7OamAttrii(void*, int, int);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
extern unsigned char _ZN8SaveData13GetCoinRecordEj(unsigned int);
extern void _ZN5Stage12RenderNumberEhiibi(unsigned char, int, int, int, int);
extern int GetOwnerLanguage(void);

extern unsigned char data_0209f250;
extern short data_02092144[];
extern unsigned char data_0209f248;
extern unsigned char data_0209f2d8;
extern int data_0209fc68;
extern signed char data_0209f2f8;
extern unsigned char data_0209f2c8;
extern unsigned char data_0209f2f0;
extern unsigned char data_0209f238;
extern unsigned char data_0209d45c;
extern unsigned char data_0209f2e0;
extern unsigned char data_0209f29c;
extern unsigned char data_0209f2dc;

extern char _ZN3OAM8VS_PAUSEE;
extern char _ZN3OAM19ARROW_POINTING_LEFTE;
extern char _ZN3OAM20ARROW_POINTING_RIGHTE;
extern char _ZN3OAM13MM_SMALL_STARE;
extern char _ZN3OAM5PAUSEE;
extern char _ZN3OAM9TINY_STARE;
extern char _ZN3OAM25SMALL_ARROW_POINTING_LEFTE;
extern char _ZN3OAM26SMALL_ARROW_POINTING_RIGHTE;
extern char _ZN3OAM16SMALL_STAR_EMPTYE;
extern char _ZN3OAM4COINE;
extern char _ZN3OAM5TIMESE;
extern void* data_ov001_020abd98[];
extern void* data_ov001_020ac218[];
extern void* data_ov001_020ac64c[];
extern void* data_ov001_020acb68[];
extern void* _ZN3OAM21CONTROLLER_MODE_TEXTSE[];
}

class Stage {
public:
    static void PS_Render();
};

void Stage::PS_Render()
{
    if (func_02029408() != 0 && data_02092144[data_0209f250] != 0 &&
        data_0209f248 != 0xb && data_0209f248 != 0xe && data_0209f248 != 0xf &&
        ((unsigned int)data_0209f248 < 7U || (unsigned int)data_0209f248 > 8U)) {
        _ZN5Stage20RenderBouncingArrowsEv();
    }

    int r0v = (data_0209f2d8 == 1);
    if (r0v != 0 && data_0209fc68 == 0) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM8VS_PAUSEE, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
    } else if (data_0209f248 == 1 ||
               (SublevelToLevel(data_0209f2f8) == 0x1d &&
                ((unsigned int)data_0209f248 < 7U || (unsigned int)data_0209f248 > 8U))) {
        int var_sl;
        unsigned char var_sb;
        unsigned char var_r8;
        int var_r7;
        int sp18;
        var_r7 = 0;
        sp18 = 0;
        var_r8 = CountStarsCollectedInLevelToDisplay(data_0209f2c8);
        if (IsStarCollected(data_0209f2c8, 0) != 0) {
            var_r8 -= 1;
        }
        if (data_0209f248 == 1 && data_0209f2f0 == 0) {
            if (data_0209f238 == 1) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM19ARROW_POINTING_LEFTE, 0x12, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                _ZN3OAM9RenderSubEP7OamAttrii(&_ZN3OAM19ARROW_POINTING_LEFTE, 0x12, 0x10);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM19ARROW_POINTING_LEFTE, 0x14, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                _ZN3OAM9RenderSubEP7OamAttrii(&_ZN3OAM19ARROW_POINTING_LEFTE, 0x14, 0x10);
            }
            if (data_0209f238 == 2) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM20ARROW_POINTING_RIGHTE, 0xee, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                _ZN3OAM9RenderSubEP7OamAttrii(&_ZN3OAM20ARROW_POINTING_RIGHTE, 0xee, 0x10);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM20ARROW_POINTING_RIGHTE, 0xec, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                _ZN3OAM9RenderSubEP7OamAttrii(&_ZN3OAM20ARROW_POINTING_RIGHTE, 0xec, 0x10);
            }
        }
        if (data_0209f2c8 != 0xf) {
            var_sb = 0x50;
            var_sl = 1;
            if (var_r8 != 0) {
                do {
                    if (IsStarCollected(data_0209f2c8, var_sl) != 0) {
                        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM13MM_SMALL_STARE, var_sb, 0x60, -1, -1, 0);
                        var_r7 = (var_r7 + 1) & 0xff;
                    } else {
                        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM16SMALL_STAR_EMPTYE, var_sb, 0x60, -1, -1, 0);
                        sp18 = 1;
                    }
                    var_sb += 0x10;
                    var_sl += 1;
                } while (var_r7 != var_r8);
            }
            if (sp18 == 0 && var_r8 != 7) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM16SMALL_STAR_EMPTYE, var_sb, 0x60, -1, -1, 0);
            }
            unsigned char r5v = _ZN8SaveData13GetCoinRecordEj(data_0209f2c8);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM4COINE, 0x70, 0x70, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x80, 0x78, -1, -1, 0);
            _ZN5Stage12RenderNumberEhiibi(r5v, 0x88, 0x70, 1, 8);
            if (IsStarCollected(data_0209f2c8, 0) != 0) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM13MM_SMALL_STARE, 0xb0, 0x78, -1, -1, 0);
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM13MM_SMALL_STARE, 0x70, 0x60, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x80, 0x60, -1, -1, 0);
            _ZN5Stage12RenderNumberEhiibi(CountStarsCollectedInLevelToDisplay(0x1d), 0x88, 0x58, 1, 8);
        }
    } else if (data_0209d45c & 8) {
        if ((unsigned int)data_0209f2c8 < 0xfU) {
            unsigned char r4v = CountStarsCollectedInLevelToDisplay(data_0209f2c8);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM13MM_SMALL_STARE, 0x58, 0x8c, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x68, 0x8c, -1, -1, 0);
            _ZN5Stage12RenderNumberEhiibi(r4v, 0x70, 0x84, 1, 1);
            unsigned char r4v2 = _ZN8SaveData13GetCoinRecordEj(data_0209f2c8);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM4COINE, 0x88, 0x84, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x98, 0x8c, -1, -1, 0);
            _ZN5Stage12RenderNumberEhiibi(r4v2, 0xa0, 0x84, 1, 1);
        }
        if (data_0209f248 == 0xb) {
            _ZN3OAM9RenderSubEP7OamAttrii(&_ZN3OAM5PAUSEE, 0x80, 0x60);
        }
    }

    unsigned char f248b = data_0209f248;
    if ((unsigned char)(f248b + 0xf9) <= 1) {
        if (data_0209f2e0 == 3) {
            if (data_0209f29c == 0) {
                if (GetOwnerLanguage() == 5) {
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x46, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                } else if (GetOwnerLanguage() == 3) {
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x4e, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                } else {
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x56, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                }
            } else {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0xce, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x26, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
        }
        if (GetOwnerLanguage() == 5) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020abd98[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 4) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020ac218[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 3) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020ac64c[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020acb68[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM21CONTROLLER_MODE_TEXTSE[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
        return;
    }

    if (f248b != 0xa) {
        return;
    }

    if (data_0209f2e0 == 2) {
        if (data_0209f29c == 0) {
            if (GetOwnerLanguage() == 5) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x46, 0xa8, -1, -1, 0);
            } else if (GetOwnerLanguage() == 3) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x4e, 0xa8, -1, -1, 0);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0x56, 0xa8, -1, -1, 0);
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0xce, 0xa8, -1, -1, 0);
        }
    } else {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM9TINY_STARE, 0xe, ((data_0209f2e0 * 6) + 7) * 8, -1, -1, 0);
    }

    if (data_0209f238 == 2) {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM25SMALL_ARROW_POINTING_LEFTE, 0x62, 0x38, -1, -1, 0);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM25SMALL_ARROW_POINTING_LEFTE, 0x64, 0x38, -1, -1, 0);
    }

    if (data_0209f238 == 1) {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM26SMALL_ARROW_POINTING_RIGHTE, 0xf6, 0x38, -1, -1, 0);
        return;
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM26SMALL_ARROW_POINTING_RIGHTE, 0xf4, 0x38, -1, -1, 0);
}
