//cpp
/* dMeter_c -- the in-game HUD (METER), ov002 0x020fb8f8..0x020fe154.
 *
 * One translation unit, nineteen functions, the way the cartridge's own build
 * had it. This replaces nineteen one-function shards. Their bodies are
 * unchanged apart from spelling: the four members include/dMeter_c.h did not
 * declare are now real methods rather than hand-mangled extern "C" free
 * functions, the offset casts that reached this object's fields are now the
 * field names, and the destructor is inline in the header.
 *
 * dMeter_c derives from dBase_c, and the cartridge says so itself: the
 * __si_class_type_info record _ZTI8dMeter_c at ov002:0x0210c224 points its name
 * word at _ZTS8dMeter_c (0x0210c218, the ten bytes "8dMeter_c" and a NUL) and
 * its base word at arm9:0x02086e78, which is _ZTI7dBase_c. So the class name is
 * the ROM's own, not a coinage. _ZTV8dMeter_c is an eighteen-slot table, the
 * fBase_c shape dBase_c passes down unchanged, and six of those slots plus both
 * destructor variants resolve into this range.
 *
 * THIS TU OWNS THE CLASS VTABLE. The destructor is inline in the class body, so
 * the key function is InitResources -- the first virtual this class declares out
 * of line -- and that anchors _ZTV8dMeter_c (ov002 public address point
 * 0x0210c2c8, storage 0x0210c2c0) here, together with the typeinfo and type-name
 * records it drags in for the whole ancestor chain (dMeter_c, dBase_c, fBase_c).
 * None of that data lies inside this entry's licensed .text range, and it could
 * not be licensed as one interval even if it were: the cartridge puts four
 * unnamed sprite tables between this class's typeinfo pair at 0x0210c218 and the
 * vtable storage at 0x0210c2c0, and g_profile_METER immediately below the pair.
 * So dsd keeps supplying the cartridge's own bytes and production isolation
 * discards the emitted duplicates; the manifest's compiler_only_output block
 * licenses each one at its measured ROM home.
 *
 * Inlining the destructor is also what puts D1 and D0 in cartridge order.
 * Written out of line, mwcc emits the synthesized D0 ahead of the written D1 and
 * the licensed .text is no longer ROM-ascending; inlining additionally deletes
 * the homeless D2 that no module gives a symbol to.
 *
 * The factory is NOT in this TU. dMeter_c_classInit begins at 0x020fe154,
 * immediately past this entry's end, and keeps its own source. dMap_c's
 * translation unit ends immediately below, at 0x020fb8f8.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address ROM
 * function is written first. Do not reorder.
 */

#include "dMeter_c.h"
#include "Timer.h"
#include "decl_common.h"
#include "types.h"
#include "decl_Player.h"
#include "Player.h"
#include "Stage.h"

/* The declarations below are the union of what the nineteen shards declared
 * locally, minus everything a shared header already supplies. What is left is
 * the sprite layer and a handful of globals the tree has not named yet; each
 * entry says why it cannot come from a shared header. */

struct OamAttr; struct Matrix2x2;

/* Stands in for include/OAM.h, which declares Render and the five-argument
 * RenderSub but none of these OamAttr statics. The legacy sources spelled the
 * same twelve objects this way, and a static data member cannot be added to a
 * class from outside it, so moving them would mean editing a shared header that
 * every sprite-drawing TU consumes. The `namespace OAM` spelling one shard used
 * is folded in here; both mangle to _ZN3OAM...E. */
class OAM {
public:
    static OamAttr CAM_BUTTON_L;
    static OamAttr CAM_BUTTON_L_PRESSED;
    static OamAttr CAM_BUTTON_R;
    static OamAttr CAM_BUTTON_R_PRESSED;
    static OamAttr S_CAM_BUTTON_L;
    static OamAttr S_CAM_BUTTON_L_PRESSED;
    static OamAttr S_CAM_BUTTON_R;
    static OamAttr S_CAM_BUTTON_R_PRESSED;
    static OamAttr CAM_ZOOM_BUTTON;
    static OamAttr CAM_ZOOM_BUTTON_PRESSED;
    static OamAttr S_CAM_ZOOM_BUTTON;
    static OamAttr S_CAM_ZOOM_BUTTON_PRESSED;
    static void RenderSub(OamAttr *attr, int x, int y, int a, int b);
    static void Render(bool vis, OamAttr* attr, int x, int y, int a, int b, Matrix2x2* m);
};

/* One row of the pad-state table at 0209f49c and 0209f4ae. */
typedef struct { u8 pad[0x18]; } Rec18;

/* The camera record data_0209f318 points at; only its flag word is read. */
typedef struct { u8 pad[0x154]; u32 flags; } S154;

/* The object data_0209f5bc points at, reached through slot 5 alone. */
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};

namespace GX { void LoadOBJPltt(const void*, u32, u32); }

namespace GXS { void LoadOBJPltt(const void*, u32, u32); }





extern "C" {
extern unsigned char data_ov002_02111184;
extern Timer data_0209d4c8;
extern struct OamAttr data_ov002_0210ce80;
extern struct OamAttr _ZN3OAM4TIMEE;
extern struct OamAttr _ZN3OAM7MINUTESE;
extern struct OamAttr data_ov002_0210c6c0;
/* ov001's OAM::NUMBERS, not ov000's -- ov000 shares this load window with ov002 and
* so can never be the module this reaches. Sits with OAM::TIME and OAM::MINUTES above. */
extern struct OamAttr* _ZN3OAM7NUMBERSE[];
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, struct OamAttr* attr, int x, int y, int a, int b, int sx, int sy, int c, int d);
extern volatile unsigned short data_ov002_0210c208[];
extern unsigned char data_0209f250;
extern unsigned char data_ov002_02111178;
extern signed char data_0209f2f4;
struct OamAttr;
extern OamAttr* _ZN3OAM10LIFE_ICONSE[];
extern OamAttr _ZN3OAM5TIMESE;
struct HUDInfo {
char pad[0x6d9];
unsigned char field_6d9;
};
extern HUDInfo* data_0209f394[];
extern u8 data_020a0e40;
extern u8 data_ov002_02111180;
extern S154 *data_0209f318;
extern Rec18 data_0209f4ae[];
extern int data_0208ee44;
extern u32 data_0209b454;
extern Rec18 data_0209f49c[];
extern u8 data_0209d660;
extern signed char data_0209f310[];
extern int data_ov001_020abac8[];
extern unsigned char data_0209f2d8;
extern unsigned char data_0209f2fc;
extern unsigned char data_0209f2ac;
extern unsigned char data_0209f2d4;
extern int data_020a0db0;
extern struct OamAttr _ZN3OAM10POWER_STARE;
extern unsigned char NumStars(void);
extern signed char data_0209f30c[];
extern struct OamAttr data_ov002_0210c6b8;
extern signed char data_0209f2f8;
extern OamAttr _ZN3OAM4COINE;
extern int SublevelToLevel(int i);
extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern unsigned char data_0209f204;
extern struct OamAttr data_ov002_0210d290;
extern struct OamAttr data_ov002_0210d2c0;
extern struct OamAttr data_ov002_0210d300;
extern struct OamAttr data_ov002_0210d338;
extern struct OamAttr _ZN3OAM7VS_TIMEE;
extern struct OamAttr* _ZN3OAM17VS_YELLOW_NUMBERSE[];
extern struct OamAttr* data_ov002_0210ca0c[];
extern struct OamAttr data_ov002_0210d2a0;
extern struct OamAttr data_ov002_0210d2d0;
extern struct OamAttr data_ov002_0210d308;
extern struct OamAttr data_ov002_0210d348;
extern struct OamAttr _ZN3OAM10VS_TIME_UPE;
extern int GetOwnerLanguage(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, struct OamAttr* attr, int x, int y, int a, int b, int s, int c);
extern unsigned char data_0209f2bc;
extern unsigned short data_ov002_02111188;
extern unsigned char data_0209d454;
void func_02012790(int);
extern unsigned char data_ov002_0211117c[];
extern struct OamAttr* data_ov002_0210c254[];
extern struct OamAttr* data_ov002_0210c278[];
extern struct OamAttr* data_ov002_0210c230[];
extern struct OamAttr data_ov002_0210d5d0;
extern struct OamAttr data_ov002_0210d4f0;
extern struct OamAttr data_ov002_0210c690;
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
extern int _ZN5Event6GetBitEj(unsigned int bit);
extern int data_0209caa0[];
extern unsigned char data_0209f284;
extern VObj *data_0209f5bc;
void* LoadFile(int handle);
void DecompressLZ16(void* handle, int addr);
void Deallocate(void* handle);
void func_0203da4c(void);
/* reconciled: data_ov002_0211117c is declared above as `unsigned char []`. The legacy file for InitResources spelled it `u8 data_ov002_0211117c[0]`, a zero-length
 * array; the unsized form is the one every use here needs -- the element reads at
 * [0] and the byte read at line 663 takes its address directly. */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN8dMeter_c13InitResourcesEv, 0x020fda04, size 0x750 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c13InitResourcesEv
int dMeter_c::InitResources()
{
    char* vram_a = (char*)0x6400000;
    char* vram_b = (char*)0x6600000;
    int cond1 = (data_0209f2d8 == 1);
    if (cond1 != 0) {
        void* h;
        if (GetOwnerLanguage() == 5) {
            h = LoadFile(0xB003);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xB007);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else if (GetOwnerLanguage() == 4) {
            h = LoadFile(0xAC03);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xAC07);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else if (GetOwnerLanguage() == 3) {
            h = LoadFile(0xA803);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xA807);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else if (GetOwnerLanguage() == 2) {
            h = LoadFile(0xA403);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xA407);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else {
            h = LoadFile(0xA003);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xA007);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        }
        h = LoadFile(0x8000);
        DecompressLZ16(h, (int)(vram_b + 0x2000));
        Deallocate(h);
        h = LoadFile(0x8002);
        GX::LoadOBJPltt(h, 0, 0x120);
        GXS::LoadOBJPltt(h, 0, 0x120);
        Deallocate(h);
        h = LoadFile(0x8003);
        GX::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
        h = LoadFile(0x8001);
        GXS::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
    } else {
        void* h;
        if (GetOwnerLanguage() == 5) h = LoadFile(0x270);
        else if (GetOwnerLanguage() == 4) h = LoadFile(0x26E);
        else if (GetOwnerLanguage() == 3) h = LoadFile(0x26C);
        else if (GetOwnerLanguage() == 2) h = LoadFile(0x26A);
        else h = LoadFile(0x269);
        DecompressLZ16(h, 0x6400000);
        DecompressLZ16(h, 0x6600000);
        Deallocate(h);
        if (GetOwnerLanguage() == 5) h = LoadFile(0xB00E);
        else if (GetOwnerLanguage() == 4) h = LoadFile(0xAC0E);
        else if (GetOwnerLanguage() == 3) h = LoadFile(0xA80E);
        else if (GetOwnerLanguage() == 2) h = LoadFile(0xA40E);
        else h = LoadFile(0xA00E);
        DecompressLZ16(h, (int)(vram_a + 0x2000));
        Deallocate(h);
        h = LoadFile(0x229);
        DecompressLZ16(h, (int)(vram_b + 0x2000));
        Deallocate(h);
        if (GetOwnerLanguage() == 5) h = LoadFile(0xB00A);
        else if (GetOwnerLanguage() == 4) h = LoadFile(0xAC0A);
        else if (GetOwnerLanguage() == 3) h = LoadFile(0xA80A);
        else if (GetOwnerLanguage() == 2) h = LoadFile(0xA40A);
        else h = LoadFile(0xA00A);
        GX::LoadOBJPltt(h, 0, 0x120);
        GXS::LoadOBJPltt(h, 0, 0x120);
        Deallocate(h);
        h = LoadFile(0x980F);
        GX::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
        h = LoadFile(0x22A);
        GXS::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
    }

    int var_r2 = 0;
    unk62 = 0;
    unk64 = 0xB4;
    mVsTimerY = 0xA;
    if (data_0209f2d8 == 1) var_r2 = 1;
    if (var_r2 != 0) {
        if (data_0209f2f8 == 0x33) mVsTimer = 0x1E;
        else if (data_0209f2f8 == 0x2B) *(volatile u16 *)&mVsTimer = 0x1E;
        else mVsTimer = 0x1E;
    } else {
        mVsTimer = 0x1E;
    }

    data_ov002_02111188 = 0xB4;
    data_ov002_02111184 = 0;
    {
        u8 idx = *(volatile u8*)&data_0209f250;
        data_ov002_0211117c[0] = ((Player*)((idx ? (void* volatile*)data_0209f394 : (void* volatile*)data_0209f394)[idx]))->GetHealth();
    }
    if (data_ov002_0211117c[0] == 8) {
        mHealthMeterY = -0x18;
        unk72 = 0;
        mHealthMeterState = 0;
    } else {
        mHealthMeterY = 0x18;
        mHealthMeterState = 1;
    }

    int var_r0_2 = 0;
    int var_r1;
    if (data_0209f2d8 == 1) var_r1 = 1; else var_r1 = 0;
    if (var_r1 == 0) {
        int n = NumStars();
        if (n > 0x63) var_r0_2 = 0x1B;
        else if (n > 9) var_r0_2 = 0x12;
        else var_r0_2 = 9;
    }

    int var_r1_2;
    if (data_0209f2d8 == 1) var_r1_2 = 1; else var_r1_2 = 0;
    if (var_r1_2 != 0) {
        data_ov002_02111178 = 0;
        mLifeCountX = 0x10;
        mStarCountX = 0xF0;
        mStarCountSlideIn = 0;
    } else if (!(data_0209caa0[2] & 0x80)) {
        data_ov002_02111178 = 6;
        mLifeCountX = -0x3A;
        mStarCountX = (s16)(var_r0_2 + 0x120);
        mStarCountSlideIn = 1;
    } else if (data_0209f2fc == 2) {
        data_ov002_02111178 = 1;
        mLifeCountX = -0x3A;
        mStarCountX = 0xF0;
        mStarCountSlideIn = 0;
    } else if (data_0209f2fc == 1) {
        data_ov002_02111178 = 2;
        mLifeCountX = -0x3A;
        mStarCountX = (s16)(var_r0_2 + 0x120);
        mStarCountSlideIn = 0;
    } else {
        data_ov002_02111178 = 0;
        mLifeCountX = 0x10;
        mStarCountX = 0xF0;
        mStarCountSlideIn = 0;
    }

    func_0203da4c();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN8dMeter_c8BehaviorEv, 0x020fd7a4, size 0x260 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c8BehaviorEv
int dMeter_c::Behavior()
{
    int t;

    if (data_0209f5bc->v5() == 0)
        goto ret;
    t = (data_0209f2d8 == 1);
    if (t != false)
        goto body;
    if ((data_0209caa0[2] & 0x80) != 0)
        goto body;
ret:
    return 1;
body:
    if (t != false) {
        UpdateVsTimer();
    } else {
        int flags;
        int n;

        UpdateHealthMeter();
        n = NumStars();
        flags = 0;
        if (n > 0x63)
            n = 0x1b;
        else if (n > 9)
            n = 0x12;
        else
            n = 9;

        switch (data_ov002_02111178) {
        case 2:
            data_ov002_02111178 = 3;
            break;
        case 3:
            if (mLifeCountX >= 0x10) {
                mLifeCountX = 0x10;
                flags = 1;
            } else {
                mLifeCountX += 4;
            }
            if (data_0209f2fc == 1) {
                if (mStarCountX <= 0xf0) {
                    mStarCountX = 0xf0;
                    flags |= 2;
                } else {
                    mStarCountX -= 4;
                }
            } else {
                flags |= 2;
            }
            if (flags == 3)
                data_ov002_02111178 = 4;
            break;
        case 5:
            if (mHealthMeterHoldTimer == 0) {
                if (mLifeCountX < -0x38) {
                    flags = 1;
                } else {
                    mLifeCountX -= 4;
                }
                if (data_0209f2fc == 1) {
                    if (mStarCountX <= n + 0x120) {
                        mStarCountX += 4;
                    } else {
                        flags |= 2;
                    }
                } else {
                    flags |= 2;
                }
                if (flags == 3)
                    data_ov002_02111178 = 6;
            }
            break;
        case 6:
            if (mLifeCountX >= 0x10) {
                mLifeCountX = 0x10;
                flags = 1;
            } else {
                mLifeCountX += 4;
            }
            if (data_0209f2fc == 1 || mStarCountSlideIn != 0) {
                if (mStarCountX <= 0xf0) {
                    mStarCountX = 0xf0;
                    flags |= 2;
                } else {
                    mStarCountX -= 4;
                }
            } else {
                flags |= 2;
            }
            if (flags == 3) {
                data_ov002_02111178 = 0;
                mStarCountSlideIn = 0;
            }
            break;
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN8dMeter_c6RenderEv, 0x020fd5e0, size 0x1c4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// _ZN8dMeter_c6RenderEv at 0x020fd5e0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int dMeter_c::Render()
{
    int b = (data_0209f2d8 == 1);
    if (b) {
        if (data_0209fc9c != 0) goto end;
        if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0) {
            RenderVsTimer();
            RenderStarCount();
            RenderCoinCount();
            RenderCameraButtons();
        } else {
            if (data_0209f204 != 0) {
                RenderVsTimer();
            }
        }
    } else {
        if ((data_0209caa0[2] & 0x80) == 0) goto end;
        unsigned char v = data_0209f20c;
        if (((data_0209f2c4 | v | data_0209f294) & 0xff) == 0) {
            RenderHealthMeter();
            if (_ZN5Event6GetBitEj(0x1d) == 0) {
                RenderCoinCount();
                RenderRedCoins();
                RenderSilverStars();
                RenderTimeTimer();
            }
            if (data_0209f284 != 0) {
                Stage::RenderBouncingArrows();
            }
        } else {
            if (v != 0) {
                RenderLifeCount();
                RenderStarCount();
            }
        }
        unsigned char v2 = data_0209f20c;
        if ((data_0209f2c4 | v2 | data_0209f294) & 0xff) {
            if (v2 == 0) goto end;
            if (data_0209f2d4 >= 3) goto end;
        }
        RenderStarCount();
        RenderCameraButtons();
        RenderLifeCount();
    }
end:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN8dMeter_c16OnPendingDestroyEv, 0x020fd5dc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method */
/* dMeter_c::OnPendingDestroy() at 0x020fd5dc (ov002) -- vtable slot 12.
 * Empty override; the dMeter_c does nothing when marked for destruction.
 */
void dMeter_c::OnPendingDestroy()
{
    (void)this;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN8dMeter_c16CleanupResourcesEv, 0x020fd5d4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
/* dMeter_c::CleanupResources() at 0x020fd5d4 (ov002) -- vtable slot 3.
 * Returns VS_FAIL (1); the dMeter_c holds no SharedFilePtr/heap resources to
 * release on death. dMeter_c : dBase_c : fBase_c.
 */
s32 dMeter_c::CleanupResources()
{
    (void)this;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN8dMeter_c17UpdateHealthMeterEv, 0x020fd218, size 0x3bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c17UpdateHealthMeterEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
void dMeter_c::UpdateHealthMeter()
{
    int chr = data_0209f250;
    char *player = (char *)data_0209f394[chr];
    unsigned int health = (data_02092144[chr] >> 8) & 0xff;
    if ((unsigned char)(data_0209f2c4 | data_0209f20c | data_0209f294) != 0)
        return;
    if (mHealthMeterHoldTimer != 0)
        mHealthMeterHoldTimer -= data_0208ee44;
    if (mHealthTickTimer != 0)
        mHealthTickTimer -= data_0208ee44;
    if (_ZN5Event6GetBitEj(0x1d)) {
        unsigned char s = mHealthMeterState;
        if (s != 0) {
            if (s < 5)
                mHealthMeterState = 5;
        }
    }
    switch (mHealthMeterState) {
    case 0:
        if (*(unsigned char *)(player + 0x706) != 0) {
            mHealthMeterY = 0x39;
            mHealthMeterState = 1;
            return;
        }
        if (data_ov002_0211117c[0] <= health)
            return;
        mHealthMeterY = 0x39;
        data_ov002_0211117c[0] = *(volatile unsigned char *)&data_ov002_0211117c - 1;
        mHealthTickTimer = 0x1e;
        mHealthMeterHoldTimer = 0x5a;
        mHealthMeterState = 1;
        return;
    case 1:
        if (mHealthMeterHoldTimer == 0) {
            if (mHealthMeterY != 0x19) {
                mHealthMeterY -= 4;
                if (mHealthMeterY <= 0x19)
                    mHealthMeterY = 0x19;
            }
        }
        if (mHealthTickTimer != 0)
            return;
        {
            unsigned char cur = data_ov002_0211117c[0];
            if (cur < health) {
                if ((int)(health - cur) > 1) {
                    data_ov002_0211117c[0] = cur + 1;
                    func_02012790(0xd);
                    mHealthTickTimer = 0x10;
                    return;
                }
                data_ov002_0211117c[0] = health;
                func_02012790(0xd);
                if (*(unsigned char *)(player + 0x706) != 0)
                    return;
                if (health != 8)
                    return;
                if (data_ov002_02111178 == 1) {
                    data_ov002_02111178 = 3;
                    mHealthMeterState = 2;
                } else {
                    mHealthMeterHoldTimer = 0x3c;
                    mHealthMeterState = 4;
                }
                return;
            }
            if (cur > health) {
                data_ov002_0211117c[0] = cur - 1;
                mHealthTickTimer = 0x1e;
                return;
            }
            if (cur != 8)
                return;
            if (*(unsigned char *)(player + 0x706) != 0)
                return;
            if (((Player *)player)->IsInAir())
                return;
            if (data_ov002_02111178 == 1) {
                data_ov002_02111178 = 3;
                mHealthMeterState = 2;
            } else {
                mHealthMeterHoldTimer = 0x3c;
                mHealthMeterState = 4;
            }
        }
        return;
    case 2:
        if (data_ov002_02111178 == 4) {
            mHealthMeterHoldTimer = 0x3c;
            mHealthMeterState = 3;
        }
        return;
    case 3:
        if (mHealthMeterHoldTimer != 0)
            return;
        GiveLives(-1);
        func_02012790(0x40);
        data_ov002_02111178 = 5;
        mHealthMeterHoldTimer = 0x3c;
        mHealthMeterState = 4;
        return;
    case 4:
    case 5:
        if (mHealthMeterHoldTimer != 0)
            return;
        mHealthMeterY -= 4;
        if (mHealthMeterY >= -0x18)
            return;
        mHealthMeterY = -0x18;
        if (mHealthMeterState == 4)
            mHealthMeterState = 0;
        else
            mHealthMeterState = 6;
        return;
    case 6:
        if (_ZN5Event6GetBitEj(0x1d) == 0)
            mHealthMeterState = 1;
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN8dMeter_c17RenderHealthMeterEv, 0x020fcfec, size 0x22c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c17RenderHealthMeterEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// _ZN8dMeter_c17RenderHealthMeterEv at 0x020fcfec
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
void dMeter_c::RenderHealthMeter()
{
    if (_ZN6Player16IsInsideOfCannonEv(data_0209f394[data_0209f250])) return;

    if (GetOwnerLanguage() == 5) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov002_0210c254[data_ov002_0211117c[0]], 0x80, *(short*)((char*)&mHealthMeterY), -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, &data_ov002_0210d5d0, 0x80, *(short*)((char*)&mHealthMeterY), -1, 1, 0x1000, 0x1000, 0, -1);
    } else if (GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov002_0210c278[data_ov002_0211117c[0]], 0x80, *(short*)((char*)&mHealthMeterY), -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, &data_ov002_0210d4f0, 0x80, *(short*)((char*)&mHealthMeterY), -1, 1, 0x1000, 0x1000, 0, -1);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov002_0210c230[data_ov002_0211117c[0]], 0x80, *(short*)((char*)&mHealthMeterY), -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, &data_ov002_0210c690, 0x80, *(short*)((char*)&mHealthMeterY), -1, 1, 0x1000, 0x1000, 0, -1);
    }

    _ZN2GX11LoadOBJPlttEPKvjj(
        &data_ov002_0210c310[data_ov002_0210c29c[data_ov002_0211117c[0]]], 0x60, 0x20);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN8dMeter_c13UpdateVsTimerEv, 0x020fce9c, size 0x150 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c13UpdateVsTimerEv
/* include/dMeter_c.h does not declare this member, and a member cannot be added to
 * a class from outside it, so it takes the hand-mangled free-function form.
 * Same ABI: the object still arrives in r0. */
void dMeter_c::UpdateVsTimer()
{
    unsigned short *q;
    unsigned int v;

    if (data_0209f2bc != 0)
        return;
    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) != 0)
        return;

    if (data_ov002_02111188 != 0)
        data_ov002_02111188 = data_ov002_02111188 - data_0208ee44;

    if (data_0209f204 != 0)
        return;

    if (data_ov002_02111188 == 0)
    {
        data_ov002_02111188 = 0xb4;
        q = &mVsTimer;
        *q = *q - 1;
        v = mVsTimer;
        if (v <= 3 && v != 0)
            func_02012790(0x28);
        if (mVsTimer == 0)
        {
            data_0209f204 = 1;
            data_ov002_02111188 = 0x3c;
            func_02012790(0x29);
            data_0209d454 &= ~4;
        }
    }

    if (mVsTimer > 3)
        return;
    if (mVsTimerY < 0x30)
    {
        q = &mVsTimerY;
        *q = *q + 4;
    }
    else
    {
        mVsTimerY = 0x38;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN8dMeter_c13RenderVsTimerEv, 0x020fca18, size 0x484 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c13RenderVsTimerEv
/* recovered: named members + shared header, real C++ method
 *
 * The VS-mode countdown: a localised "TIME" caption, and below it either the
 * remaining seconds or a localised "TIME UP".
 *
 * Both captions are chosen by a GetOwnerLanguage() ladder testing 5, 4, 3, 2
 * and falling through to OAM::VS_TIME / OAM::VS_TIME_UP -- so the OAM symbols
 * are the DEFAULT artwork and the four data_ov002_* blocks are translations.
 * The ladder re-calls GetOwnerLanguage() at every rung rather than caching it;
 * that is the ROM's own shape and is preserved rather than tidied.
 *
 * Digit layout is not uniform, which is the interesting part:
 *   >= 10   two glyphs at x=0x74 and x=0x84, recoloured to palette 0xb once
 *           mVsTimer <= 5 -- the last-five-seconds warning
 *   4..9    ONE glyph at x=0x7c, centred, there being no tens digit
 *   0..3    not a digit at all: data_ov002_0210ca0c[3 - ones], a separate
 *           countdown artwork set, indexed backwards
 *
 * Only the caption is drawn when any of data_0209f2c4 / f20c / f294 is set;
 * they are OR-ed and tested as a single byte.
 */
void dMeter_c::RenderVsTimer()
{
    int t = mVsTimer;

    if ((unsigned char)(data_0209f2c4 | data_0209f20c | data_0209f294) == 0) {
        if (GetOwnerLanguage() == 5) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d290, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 4) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d2c0, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 3) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d300, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d338, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM7VS_TIMEE, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (data_0209f204 == 0) {
        int pal = (mVsTimer <= 5) ? 0xb : -1;
        unsigned short tens = t / 10;
        if (tens != 0) {
            OAM::Render(false, _ZN3OAM17VS_YELLOW_NUMBERSE[tens], 0x74, mVsTimerY, pal, -1, 0);
            OAM::Render(false, _ZN3OAM17VS_YELLOW_NUMBERSE[t % 10], 0x84, mVsTimerY, pal, -1, 0);
        } else {
            int ones = t % 10;
            if (ones > 3) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, _ZN3OAM17VS_YELLOW_NUMBERSE[ones], 0x7c, mVsTimerY, -1, -1, 0x1000, 0);
            } else {
                OAM::Render(false, data_ov002_0210ca0c[3 - ones], 0x84, mVsTimerY, -1, -1, 0);
            }
        }
    } else {
        if (GetOwnerLanguage() == 5) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d2a0, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 4) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d2d0, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 3) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d308, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d348, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM10VS_TIME_UPE, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- _ZN8dMeter_c15RenderCoinCountEv, 0x020fc81c, size 0x1fc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c15RenderCoinCountEv
/* _ZN8dMeter_c15RenderCoinCountEv at 0x020fc81c (ov002), size 0x1fc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
void dMeter_c::RenderCoinCount()
{
    int t = (data_0209f2d8 == 1);
    if (t != false) {
        int sb = 0xf0;
        CalculateDigits(data_0209f358[data_0209f250]);
        for (int i = 2; i >= 0; i--) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(true, _ZN3OAM7NUMBERSE[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(true, &_ZN3OAM5TIMESE, sb, 0xa, -1, 1, 0);
        OAM::Render(true, &_ZN3OAM4COINE, sb - 0x10, 2, -1, 1, 0);
    } else {
        if (SublevelToLevel(data_0209f2f8) == 0x1d)
            return;
        int i;
        int sb = 0xf0;
        CalculateDigits(data_0209f358[data_0209f250]);
        for (i = 2; i >= 0; i--) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(false, _ZN3OAM7NUMBERSE[d], sb, 2, -1, 1, 0);
                sb -= 9;
            }
        }
        OAM::Render(false, &_ZN3OAM5TIMESE, sb, 0xa, -1, 1, 0);
        OAM::Render(false, &_ZN3OAM4COINE, sb - 0x10, 2, -1, 1, 0);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN8dMeter_c14RenderRedCoinsEv, 0x020fc77c, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c14RenderRedCoinsEv
/* recovered: shared header, real C++ method
 *
 * Draws one sprite per red coin still to collect, stepping 0xb across from
 * x=0x10. Both the count (data_0209f30c) and the row selector (data_0209f310)
 * are indexed by the same data_0209f250.
 *
 * That row selector is the interesting bit: it is the SAME byte
 * dMeter_c::RenderSilverStars reads as its star COUNT. Non-zero there means silver
 * stars are being drawn at y=0xa, so the red coins drop to y=0x1c to make room.
 * One byte serving as both a count and an occupancy flag for a shared row.
 *
 * The count is read through an explicit unsigned char cast off a signed char
 * array -- kept as the ROM spells it, since the same array is read signed by
 * RenderSilverStars.
 *
 * TOUCHES NO FIELD; the ROM body takes no `this` and now ignores the one it is
 * passed. Byte-free.
 */
void dMeter_c::RenderRedCoins()
{
  int idx = data_0209f250;
  int flag = data_0209f310[idx];
  int count = (unsigned char)data_0209f30c[idx];
  int sl = flag ? 0x1c : 0xa;
  for(int i=0;i<count;i++){
    OAM::Render(false, &data_ov002_0210c6b8, 0x10 + i*0xb, sl, -1, 1, (struct Matrix2x2*)0);
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN8dMeter_c15RenderStarCountEv, 0x020fc458, size 0x324 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c15RenderStarCountEv
/* recovered: named members + shared header, real C++ method
 *
 * Draws the star counter: the star icon, an "x", and up to three digits laid
 * out RIGHT to LEFT from mStarCountX, skipping the -1 slots CalculateDigits
 * blanked.
 *
 * Three paths, and what differs between them is only WHICH count is shown and
 * on WHICH screen:
 *
 *   data_0209f2d8 == 1   a per-slot count out of data_0209f310, main screen
 *   the middle path      NumStars(), main screen, but only during a specific
 *                        cutscene window (data_ov002_02111178) and suppressed
 *                        entirely by the data_0209f2ac / data_0209f2d4 test
 *   otherwise            NumStars(), SUB screen -- the leading argument to
 *                        OAM::Render flips 0 -> 1
 *
 * The three render blocks are deliberately left duplicated rather than folded
 * into a helper: they are three separate tails in the ROM, and hoisting them
 * would be a rewrite, not a substitution.
 *
 * The digit loop runs i = 2 down to 0 -- least significant first -- which is
 * why the cursor moves left.
 */
void dMeter_c::RenderStarCount()
{
    int x = mStarCountX;
    int b = (data_0209f2d8 == 1);
    int i;

    if (b) {
        CalculateDigits((unsigned short)data_0209f310[data_0209f250]);
        for (i = 2; i >= 0; i--) {
            if (mDigits[i] >= 0) {
                OAM::Render(false, _ZN3OAM7NUMBERSE[mDigits[i]], x, 2, -1, 1, 0);
                x -= 9;
            }
        }
        OAM::Render(false, &_ZN3OAM5TIMESE, x, 10, -1, 1, 0);
        OAM::Render(false, &_ZN3OAM10POWER_STARE, x - 16, 10, -1, 1, 0);
        return;
    }

    CalculateDigits(NumStars());

    {
        unsigned char m = data_0209f2fc;
        if ((m != 2 && data_ov002_02111178 == 4) ||
            (m == 1 && data_ov002_02111178 >= 3 && data_ov002_02111178 < 6)) {
            int flag;
            if (data_0209f2ac != 0) {
                if (data_0209f2d4 == 3 && (data_020a0db0 & 0x18) == 0) {
                    flag = 0;
                } else {
                    flag = 1;
                }
            } else {
                flag = 1;
            }
            if (flag == 0) {
                return;
            }
            for (i = 2; i >= 0; i--) {
                if (mDigits[i] >= 0) {
                    OAM::Render(false, _ZN3OAM7NUMBERSE[mDigits[i]], x, 2, -1, 1, 0);
                    x -= 9;
                }
            }
            OAM::Render(false, &_ZN3OAM5TIMESE, x, 10, -1, 1, 0);
            OAM::Render(false, &_ZN3OAM10POWER_STARE, x - 16, 10, -1, 1, 0);
            return;
        }
    }

    for (i = 2; i >= 0; i--) {
        if (mDigits[i] >= 0) {
            OAM::Render(true, _ZN3OAM7NUMBERSE[mDigits[i]], x, 2, -1, 1, 0);
            x -= 9;
        }
    }
    OAM::Render(true, &_ZN3OAM5TIMESE, x, 10, -1, 1, 0);
    OAM::Render(true, &_ZN3OAM10POWER_STARE, x - 16, 10, -1, 1, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN8dMeter_c17RenderSilverStarsEv, 0x020fc3c4, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c17RenderSilverStarsEv
/* recovered: shared header, real C++ method
 *
 * Draws one sprite per silver star collected, in a row starting at x=0x10 and
 * stepping 0x11 across at y=0xa. The count comes from data_0209f310 indexed by
 * data_0209f250 -- the same byte dMeter_c::RenderRedCoins reads, where a non-zero
 * value instead pushes the red-coin row from y=0xa down to y=0x1c. So this row
 * and that one share a slot, and the red coins move when silver stars occupy it.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this` at all; as a method it now
 * receives one in r0 and ignores it, which is byte-free here. Nothing about the
 * ROM says this was a static -- unlike OAM's methods, whose call sites pass no
 * `this` -- so it is left a normal member.
 */
void dMeter_c::RenderSilverStars()
{
    int sl = 0;
    unsigned short n = (unsigned short)(short)data_0209f310[data_0209f250];
    int sb;
    if (sl < n) {
        sb = 0x10;
        do {
            OAM::Render(0, (OamAttr*)data_ov001_020abac8, sb, 0xa, -1, 1, (Matrix2x2*)0);
            sl++;
            sb += 0x11;
        } while (sl < n);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN8dMeter_c19RenderCameraButtonsEv, 0x020fc04c, size 0x378 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c19RenderCameraButtonsEv
/* Takes the object only because its caller in this TU passes it; the body
 * never reads it, which is why the legacy source could spell it static. */
void dMeter_c::RenderCameraButtons()
{
    int idx = data_020a0e40;
    u8 mode = *(u8 *)&data_0209f4ae[idx];
    S154 *p = data_0209f318;
    u8 t = data_ov002_02111180;

    if (t != 0)
        data_ov002_02111180 = t - data_0208ee44;

    if (data_0209b454 & 0x40000000)
        return;

    if (p->flags & 0x1000) {
        if (mode == 0) {
            if (*(u16 *)&data_0209f49c[idx] & 0x200) {
                OAM::RenderSub(&OAM::CAM_BUTTON_L_PRESSED, 0x19, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_L_PRESSED, 0xbf, 0xa5, -1, 1);
            } else {
                OAM::RenderSub(&OAM::CAM_BUTTON_L, 0x19, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_L, 0xbf, 0xa5, -1, 1);
            }
            if (*(u16 *)&data_0209f49c[data_020a0e40] & 0x100) {
                OAM::RenderSub(&OAM::CAM_BUTTON_R_PRESSED, 0x41, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_R_PRESSED, 0xe7, 0xa5, -1, 1);
            } else {
                OAM::RenderSub(&OAM::CAM_BUTTON_R, 0x41, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_R, 0xe7, 0xa5, -1, 1);
            }
        } else if (mode == 2) {
            if (*(u16 *)&data_0209f49c[idx] & 0x200) {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_L_PRESSED, 0xbf, 0xad, -1, 1);
            } else {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_L, 0xbf, 0xad, -1, 1);
            }
            if (*(u16 *)&data_0209f49c[data_020a0e40] & 0x100) {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_R_PRESSED, 0xe7, 0xad, -1, 1);
            } else {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_R, 0xe7, 0xad, -1, 1);
            }
        }
    }

    if (mode == 0)
        return;
    if (data_0209d660 != 0)
        return;

    int y = (mode == 1) ? 0xa5 : 0x85;
    if (mode == 1) {
        if (data_ov002_02111180 != 0) {
            OAM::RenderSub(&OAM::CAM_ZOOM_BUTTON_PRESSED, 0xe7, y, -1, 1);
            return;
        }
        OAM::RenderSub(&OAM::CAM_ZOOM_BUTTON, 0xe7, y, -1, 1);
        return;
    }
    if (data_ov002_02111180 != 0) {
        OAM::RenderSub(&OAM::S_CAM_ZOOM_BUTTON_PRESSED, 0xe7, y, -1, 1);
        return;
    }
    OAM::RenderSub(&OAM::S_CAM_ZOOM_BUTTON, 0xe7, y, -1, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN8dMeter_c15RenderLifeCountEv, 0x020fbe38, size 0x214 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c15RenderLifeCountEv
void dMeter_c::RenderLifeCount()
{
    unsigned char idx = data_0209f250;
    HUDInfo* info = data_0209f394[idx];
    unsigned char state = data_ov002_02111178;

    if (state >= 3 && state < 6) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM10LIFE_ICONSE[info->field_6d9], mLifeCountX, 0xa, -1, 1, 0x1000, 0x1000, 0, -1);
        OAM::Render(false, &_ZN3OAM5TIMESE, mLifeCountX + 0x10, 0xa, -1, 1, 0);
        CalculateDigits((unsigned short)data_0209f2f4);
        int x = mLifeCountX + 0x18;
        for (int i = 0; i < 3; i++) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(false, _ZN3OAM7NUMBERSE[d], x, 2, -1, 1, 0);
                x += 9;
            }
        }
    } else {
        OAM::RenderSub(_ZN3OAM10LIFE_ICONSE[info->field_6d9], mLifeCountX, 0xa, -1, 1);
        OAM::Render(true, &_ZN3OAM5TIMESE, mLifeCountX + 0x10, 0xa, -1, 1, 0);
        CalculateDigits((unsigned short)data_0209f2f4);
        int i;
        int x = mLifeCountX + 0x18;
        for (i = 0; i < 3; i++) {
            signed char d = mDigits[i];
            if (d >= 0) {
                OAM::Render(true, _ZN3OAM7NUMBERSE[d], x, 2, -1, 1, 0);
                x += 9;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN8dMeter_c15CalculateDigitsEt, 0x020fbdac, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c15CalculateDigitsEt
/* recovered: named members + shared header, real C++ method
 *
 * Splits a count into the three decimal digits the dMeter_c draws, most significant
 * first, and blanks leading zeros by writing -1 rather than a digit.
 *
 * The suppression stops at i != 2, so the units digit is always drawn: a value
 * of 0 renders "0", not blank. `flag` latches on the first non-zero digit, so
 * an interior zero (as in 101) is drawn normally.
 *
 * The divisor table is volatile in the ROM's own spelling and stays that way --
 * it forces the 100/10/1 loads to be re-read per iteration instead of folded
 * into immediates, which is what makes this a loop at all.
 */
void dMeter_c::CalculateDigits(unsigned short value)
{
    int flag = 0;
    int i;
    for (i = 0; i < 3; i++) {
        int digit = value / data_ov002_0210c208[i];
        if (digit == 0 && flag == 0 && i != 2) {
            mDigits[i] = -1;
        } else {
            mDigits[i] = digit;
            flag = 1;
        }
        value = value % data_ov002_0210c208[i];
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8dMeter_c15RenderTimeTimerEv, 0x020fb96c, size 0x440 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_c15RenderTimeTimerEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// _ZN8dMeter_c15RenderTimeTimerEv at 0x020fb96c (ov002)
void dMeter_c::RenderTimeTimer()
{
    unsigned long long t;
    unsigned long long min;
    unsigned long long sec;
    unsigned long long centi;

    if (data_ov002_02111184 == 0)
        return;

    t = data_0209d4c8.GetTime() << 6;
    sec = t / 33514000;
    centi = t / 33514 % 1000 / 10;
    min = sec / 60;
    sec = sec % 60;

    if (min >= 100) {
        min = 99;
        sec = 59;
        centi = 99;
    }

    if (min / 10 != 0) {
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210ce80, 0xa4, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM4TIMEE, 0xa4, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        }
        OAM::Render(false, _ZN3OAM7NUMBERSE[min / 10], 0xb8, 0x16, -1, 1, 0);
    } else {
        if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210ce80, 0xac, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM4TIMEE, 0xac, 0x1e, -1, 1, 0x1000, 0x1000, 0, -1);
        }
    }

    OAM::Render(false, _ZN3OAM7NUMBERSE[min % 10], 0xc0, 0x16, -1, 1, 0);
    OAM::Render(false, &_ZN3OAM7MINUTESE, 0xc4, 0x1e, -1, 1, 0);
    OAM::Render(false, _ZN3OAM7NUMBERSE[sec / 10], 0xcf, 0x16, -1, 1, 0);
    OAM::Render(false, _ZN3OAM7NUMBERSE[sec % 10], 0xd7, 0x16, -1, 1, 0);
    OAM::Render(false, &data_ov002_0210c6c0, 0xdb, 0x1e, -1, 1, 0);
    OAM::Render(false, _ZN3OAM7NUMBERSE[centi / 10], 0xe8, 0x16, -1, 1, 0);
    OAM::Render(false, _ZN3OAM7NUMBERSE[centi % 10], 0xf0, 0x16, -1, 1, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN8dMeter_cD0Ev, 0x020fb928, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_cD0Ev
/* No body here, and none is possible: D0 is the deleting destructor and the
 * compiler writes all of it. The inline `~dMeter_c() {}` in include/dMeter_c.h
 * is the whole source, and mwccarm emits D1 and D0 from it in that order, which
 * is cartridge order, and emits no D2 at all.
 *
 * What the cartridge's 0x44 bytes do: the same two vptr stores and the same
 * fBase_c chain call as D1 below, then `ldr r1, =GAME_HEAP_PTR; ldr r1, [r1]`
 * and `bl _ZN6Memory10DeallocateEPvP4Heap` (arm9 0x0203c1e8) before returning
 * the object in r0. So the deallocation is fBase_c's operator delete inlined
 * here, and it does name a heap -- the global game heap, loaded from
 * arm9 0x020a0eac.
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN8dMeter_cD1Ev, 0x020fb8f8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8dMeter_cD1Ev
/* No body here either, for the same reason. The cartridge's 0x30 bytes are two
 * vtable stores and one call, and every one of the three follows from
 * `struct dMeter_c : dBase_c`:
 *
 *     str r2, [r4]     ; _ZTV8dMeter_c   0x0210c2c8
 *     str r1, [r4]     ; _ZTV7dBase_c    0x0208e4b8  -- dBase_c's dtor, INLINED
 *     bl  0x02043d48   ; _ZN7fBase_cD2Ev
 *
 * The second store is there because include/dBase_c.h defines that class's
 * destructor in its class body, so it inlines into every derived destructor.
 * There is no third store and no second call: dMeter_c adds no member with a
 * destructor of its own.
 *
 * An earlier revision of this comment described a dBgActor_c base with Model
 * and dBgW_KcMbg members. That was inherited from a sibling class and is wrong
 * for dMeter_c; _ZTI8dMeter_c's base word names dBase_c and the bytes above
 * show the single chain call.
 */
