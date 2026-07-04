//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
    void* _ZN5Actor10FindWithIDEj(u32 id);
    void LinkSilverStarAndStarMarker(void* a, void* b);
    int _ZN6Player9IsOnShellEv(void* p);
    void func_02012790(int a);
    void _ZN12CylinderClsn5ClearEv(void* c);
    void func_ov002_020e6fbc(char* c, int arg);
    int func_ov002_020e73ac(char* c);
    int _ZN6Player17SetNoControlStateEhih(void* p, int a, int b, int d);
    void _ZN7Message11PrepareTalkEv(void);
    int func_ov002_020e9630(char* c);
    void _ZN5Event6SetBitEj(u32 a);
    void GiveVsStars(int idx, int delta);
    int IsStarCollectedInCurLevel(int i);
    void CollectStarInCurLevel(int i);
    void _ZN5Actor17TrackInDeathTableEv(void* a);
    int SublevelToLevel(int i);
    int _ZN8SaveData13GetCoinRecordEj(u32 i);
    s16 NumCoins(void);
    void _ZN8SaveData21SetCoinRecordIfHigherEah(int a, u8 b);
    void _ZN6Player4HealEi(void* p, int a);
    void func_ov002_020e9464(char* c);
}

extern u8 data_0209f2d8;
extern u8 data_0209f228;
extern u8 data_0209f2ac;
extern signed char data_0209f2f8;
extern s16 data_0209f358[];
extern u8 data_0209f208;
extern u32 data_0209b454;
extern u8 data_0209d684;

extern "C" int func_ov002_020e8ef0(char* c, void* p)
{
    void* found;
    int r5;
    int r4;
    int sb;

    *(u32*)(c + 0x440) = 0xa;
    found = _ZN5Actor10FindWithIDEj(*(u32*)(c + 0x434));
    if (found) {
        LinkSilverStarAndStarMarker(found, 0);
    }
    *(void**)(c + 0x438) = p;

    if (_ZN6Player9IsOnShellEv(p) != 0) {
        int b = (*(u16*)(c + 0xc) == 0xb3);
        if (b) {
            *(u32*)(c + 0x440) = 0xd;
            *(u16*)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFFLL) &= ~4;
            *(u16*)(c + 0x490) = 0;
            *(u32*)(c + 0x4b8) = 0;
            *(u32*)(c + 0x4b4) = *(u32*)(c + 0x4b8);
            func_02012790(0x2d);
            *(u32*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
            _ZN12CylinderClsn5ClearEv(c + 0x110);
            func_ov002_020e6fbc(c, 0x14);
            *(u8*)(c + 0x49c) = 1;
            return 1;
        }
    }

    r5 = 0;
    r4 = func_ov002_020e73ac(c);
    {
        if (r4 != 0) {
            int t1 = (data_0209f2d8 == 1);
            if (!t1) {
                int t2 = (*(u16*)(c + 0xc) == 0xb3);
                if (!t2) {
                    if (*(u32*)(c + 0x444) != 9) {
                        if (*(u8*)(c + 0x49d) == 0) {
                            sb = _ZN6Player17SetNoControlStateEhih(p, r4, 0x186, 0);
                            _ZN7Message11PrepareTalkEv();
                            r5 = 1;
                        } else if (func_ov002_020e9630(c) != 0) {
                            sb = _ZN6Player17SetNoControlStateEhih(p, r4, 0x187, 0);
                            _ZN7Message11PrepareTalkEv();
                            r5 = 1;
                        } else {
                            sb = _ZN6Player17SetNoControlStateEhih(p, r4, -1, 0);
                        }
                        goto after_sc;
                    }
                }
            }
        }
        sb = _ZN6Player17SetNoControlStateEhih(p, r4, -1, 0);
    after_sc:;

        if (sb == 0) {
            goto ret0;
        }

        {
            int b1 = (data_0209f2d8 == 1);
            if (!b1) {
                _ZN5Event6SetBitEj(0x1e);
            } else {
                GiveVsStars(*(u8*)((char*)p + 0x6d8), 1);
            }
        }
        if (r4 == 0) {
            _ZN5Event6SetBitEj(0x1d);
        }

        {
        int b2 = (data_0209f2d8 == 1);
        int b3;
        if (!b2 && !(b3 = (*(u16*)(c + 0xc) == 0xb3)) &&
            *(u8*)(c + 0x49d) < 8 &&
            *(u32*)(c + 0x444) != 9) {
            data_0209f228 = *(u8*)(c + 0x49d);
            if (IsStarCollectedInCurLevel(*(u8*)(c + 0x49d)) != 0) {
                data_0209f2ac = 0;
            } else {
                data_0209f2ac = 1;
            }
            CollectStarInCurLevel(*(u8*)(c + 0x49d));
            if (r5 != 0) {
                int lvl;
                if ((((u32)(*(u16*)(c + 0x4a2) << 0x13)) >> 0x1f) != 0 && found != 0) {
                    _ZN5Actor17TrackInDeathTableEv(found);
                }
                lvl = SublevelToLevel((signed char)data_0209f2f8);
                if (lvl <= 0xe) {
                    int rec = _ZN8SaveData13GetCoinRecordEj(lvl);
                    if (rec < NumCoins()) {
                        _ZN8SaveData21SetCoinRecordIfHigherEah(
                            lvl,
                            (u8)(data_0209f358[*(u8*)((char*)p + 0x6d8)] & 0xff));
                    }
                }
            }
        }
        }
    }

    if (*(u32*)(c + 0x444) == 9) {
        data_0209f208++;
    }
    func_02012790(0x2d);
    *(u32*)(c + 0x440) = 5;
    *(void**)(c + 0x438) = p;
    {
        int b4 = (data_0209f2d8 == 1);
        if (!b4) {
            void* pp = *(void**)(c + 0x438);
            *(u32*)(((int)pp + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000000;
            *(u32*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000000;
            data_0209b454 |= 0x4000000;
        }
    }
    *(u32*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        int b5 = (*(u16*)(c + 0xc) == 0xb2);
        if (b5 && r4 != 0) {
            _ZN6Player4HealEi(p, 0x880);
        }
    }
    func_ov002_020e9464(c);
    data_0209d684 = 0;
    return 1;
ret0:
    return 0;
}
