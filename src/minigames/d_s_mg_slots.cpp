//cpp
/* Slot_machine scenes: Slot1 (three-reel slots) and Slot3 (the lamp/reel
 * variant), ov006 .text 0x0210a8c0..0x0210d740 -- the whole run, both
 * classes' every function, Slot1's factory included.
 *
 * _ZTS12dScMgSlot1_c 0x0213e5bc. _ZTI12dScMgSlot1_c 0x0213e5a0.
 * _ZTV12dScMgSlot1_c 0x0213eb40. _ZTS12dScMgSlot3_c 0x0213e5ac.
 * _ZTI12dScMgSlot3_c 0x0213e588. _ZTV12dScMgSlot3_c 0x0213eaa8.
 * Slot1: D1 at 0x0210a8c0, D0 at 0x0210a900. Slot3: D1 at 0x0210a954,
 * D0 at 0x0210a9a8.
 *
 * With defer_codegen off the optimizer brackets below bind to the functions
 * they enclose, and .text is emitted in source order, so this file is
 * written in ascending ROM order. Keep it.
 */

// leftovers:
// - Slot1 InitResources fills the reel strips through a raw char* cursor
//   over 0x46c0..0x46ff: the member-indexed form DIFFs (measured), so only
//   the scalar stores use the mReelStop/mReelScroll/mReelSpinning members.
// - The SlotReels view (reel strips, stops, matchSymbol, wilds) is local:
//   the strip lengths and win state on dScMgSlot1_c.h/dScMgSlot3_c.h are
//   still pads, so the reel helpers index through it (callers: the Slot3
//   state-table handlers dispatching through sSlot3StateTable).
// - The ROM PMF constants the Slot3 state table copies (data_ov006_0213e5xx)
//   come from unlicensed .data, in retail copy order.
// - SaveScreenForMenu/RestoreScreenFromMenu keep slot-identity names from
//   the vtable; the OAM SECONDSE symbol read at Render is a link artifact of
//   the address, not a proven field.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgSlots_AssetId {
    ASSET_D_2D_BADWIZER_NSC_BIN = 0x0001,
    ASSET_D_2D_GRADE_NSC_BIN = 0x0004,
    ASSET_D_2D_MG_BG_PAUSE_NSC_BIN = 0x0067,
    ASSET_D_2D_MG_BG_SLOT_DOWN_NCG_BIN = 0x0072,
    ASSET_D_2D_MG_BG_SLOT_DOWN_NCL_BIN = 0x0073,
    ASSET_D_2D_MG_BG_SLOT_DOWN_NSC_BIN = 0x0074,
    ASSET_D_2D_MG_BG_SLOT_FACE02_NSC_BIN = 0x0075,
    ASSET_D_2D_MG_BG_SLOT_FACE_DOWN_NSC_BIN = 0x0076,
    ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN = 0x0077,
    ASSET_D_2D_MG_BG_SLOT_FACE_NCL_BIN = 0x0078,
    ASSET_D_2D_MG_BG_SLOT_FACE_NSC_BIN = 0x0079,
    ASSET_D_2D_MG_BG_SLOT_FACE_NUKI_NSC_BIN = 0x007a,
    ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN = 0x007b,
    ASSET_D_2D_MG_BG_SLOT_SCORE_UP_NSC_BIN = 0x007c,
    ASSET_D_2D_MG_BG_SLOT_UP_NCG_BIN = 0x007d,
    ASSET_D_2D_MG_BG_SLOT_UP_NCL_BIN = 0x007e,
    ASSET_D_2D_MG_BG_SLOT_UP_NSC_BIN = 0x007f,
    ASSET_D_2D_MG_SLOT_FACE_NCG_BIN = 0x00eb,
    ASSET_D_2D_MG_SLOT_FACE_NCL_BIN = 0x00ec,
    ASSET_D_2D_MG_SLOT_NCL_BIN = 0x00f1
};

#include "dScMgSlot1_c.h"
#include "dScMgSlot3_c.h"
#include "Sound.h"
#include "OAM.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "private/ov006_slotgrid.h"
#include "types.h"
#include "PlayerInput.h"
#include "dWipe_c.h"
#include "G2x.h"

#pragma defer_codegen off
/* Codegen is deferred by default in mwccarm 2004/b56, which is why its
 * optimisation pragmas are otherwise file-global last-wins. With it off, the
 * positional brackets below bind to the functions they enclose -- and .text is
 * then emitted in SOURCE order, so this file is written in ASCENDING ROM order.
 */

struct G2 { static void* GetBG1ScrPtr(); };

typedef void (dScMgSlot3_c::*SlotState)();

/* One player's 4-byte touch record (PlayerInput.h): Slot1 Behavior reads
 * x/y through this view so the compiler, not the source, hoists them. */
struct TouchRec { u8 held; u8 edge; u8 x; u8 y; };

struct SlotReels {
    u8 pad[0x46c0];
    u8 reels[3][0x15];  /* symbol strip per reel */
    u8 stops[3];        /* where each reel stopped */
    u8 pad2[6];
    u8 speedLevel;
    u8 matchSymbol;
    u8 pad3;
    u8 wilds;           /* count of symbol 5, which matches anything */
};

extern "C" {
extern int sMgMainBannerY;
void SetBg1Offset(int a, int b);
int GetGameLanguage(void);
void* func_02054ea8(void);
// Retained caller ABI: narrowing to the definition's u16 adds a mask in InitMainScreenBg.
unsigned int LoadCompressedFileAt(int fileID, void* target);
extern unsigned char data_0209d45c;
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
void Mg_AddCredits(void* c, int delta);
void Mg_SubCredits(void* c, int val);
extern unsigned int func_02012790(unsigned int arg);
extern void Mg_SubmitScore(int self);
extern int data_ov006_0213e948[];
extern void Slot3_ReelDigits(char *c, int i);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int Sound_PlayIfNotActive(int, int, int, int);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void Mg_DrawCredits(char *a0, int a1, int a2, int a3);
void Mg_DrawScoreTally(char *a0, int a1, int a2, int a3);
extern void **sSlotLangSprites[];
extern SlotState sSlot3StateTable[];
/* The ROM PMF constants the Slot3 state table copies from (unlicensed
 * .data), in retail copy order. */
extern SlotState pmf_Slot3_StateIntro;
extern SlotState pmf_Slot3_StateSpin;
extern SlotState pmf_Slot3_StateJudge;
extern SlotState pmf_Slot3_StatePayout;
extern SlotState pmf_Slot3_StatePayout_2;
extern SlotState pmf_Slot3_StateWait;
extern SlotState pmf_Slot3_StateGameOver;
extern SlotState pmf_Slot3_StateDone;
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
extern u8 data_0209d454;
extern int data_0208ee44;
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern int Mascot_Construct(char *t);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMgSlot3_c[];
extern void RenderOamMainScreen(void*, int, int, int, int);
extern void *sSlotsSlotBlinkSprites;
extern void Mg_LoseCredits(void *c);
extern void func_ov001_020ab3f0(void *c);
void Mg_RenderSubSpriteMtx(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
int TouchArea_Update(void *c, int x);
void SlotBet_OnTouch(void *c);
void Mg_GainCredits(int x);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern int Mg_GetSavedRecord0(void);
extern int sSlot1ReelPos[][2];
extern void *sSlot1Sprites[];
extern unsigned char sSlot1Payouts[];
extern void Mg_DrawBigNumber(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void Mg_RenderSubSpritePalMtx(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void SlotBlink_Draw(unsigned char* o);
extern void Mg_SaveRecordPair(int a, int b);
extern void Mg_PlayJingle(void *c);
extern void SlotLamp_Start(void *o);
extern void SlotLamp_Tick(void *o);
extern void SlotBlink_Start(void *o, s16 x, s16 y);
extern void SlotBlink_Tick(void *o);
extern void SlotReel_SetSpeed(void *o, int v);
extern int SlotReels_CheckStopped(void *self);
extern int sSlotReelScrollSpeeds[];
extern u8 sSlot1Payouts[];
extern s16 sSlotMatchBlinkX[][2];
extern s16 sSlotMatchBlinkY[][2];
extern s16 sSlotWildBlinkX[][2];
extern s16 sSlotWildBlinkY[][2];
extern dWipe_c data_0209f61c;
extern int _ZTV12dScMgSlot1_c[];
extern int _ZTVN12dScMgSlot1_c9betIcon_cE[];
extern int _ZTV9dThIcon_c[];
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern char *_ZN2G212GetBG3ScrPtrEv(void);
extern char *_ZN3G2S13GetBG0CharPtrEv(void);
extern char *_ZN3G2S12GetBG0ScrPtrEv(void);
extern char *_ZN3G2S12GetBG1ScrPtrEv(void);
extern char *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void Mg_RenderSubSpriteScaled(void* a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern void MultiStore16(u16 val, char *dst, int nbytes);
}

// @symbol _ZN12dScMgSlot1_cD1Ev
/* ~dScMgSlot1_c() (D1, complete-object destructor). The empty body is the
   original ownership model: the compiler destroys the typed mBetIcon member,
   emitting betIcon_c's then dThIcon_c's vtable writes, before chaining to the
   dScMgBase_c base destructor. */
dScMgSlot1_c::~dScMgSlot1_c()
{
}

// @symbol _ZN12dScMgSlot1_cD0Ev
/* The compiler emits D0 from the complete-object destructor above. */

// @symbol _ZN12dScMgSlot3_cD1Ev
dScMgSlot3_c::~dScMgSlot3_c()
{
    Mascot_Destruct((char *)this + 0x4f38);
}

// @symbol _ZN12dScMgSlot3_cD0Ev
/* The compiler emits D0 from the complete-object destructor above. */

// @symbol _ZN12dScMgSlot3_c21RestoreScreenFromMenuEv
/* dScMgSlot3_c::RestoreScreenFromMenu - slot 30 override: puts BG1CNT
   back, then runs the base restore. This body is the arity witness cited
   in include/dScMgBase_c.h's slot-30 block.  The read-modify-write below clobbers
   r1 before the tail call, so the base cannot be taking a second argument. */
void dScMgSlot3_c::RestoreScreenFromMenu()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1118;
    ((dScMgBase_c *)a)->dScMgBase_c::RestoreScreenFromMenu();
}

// @symbol _ZN12dScMgSlot3_c17SaveScreenForMenuEv
/* dScMgSlot3_c::SaveScreenForMenu - slot 29 override.

   This body is the third and strongest witness that slot 29 takes no
   explicit parameter.  It CLOBBERS r1 -- the masked read-modify-write of the
   sub display-control register below compiles to `ldrh r1,[r2]; and; orr;
   strh` -- and only then tail-branches into the base with `bx ip`.  A second
   argument passed in r1 would reach dScMgBase_c::SaveScreenForMenu as a
   display-control word, so the base cannot be reading one. */
int dScMgSlot3_c::SaveScreenForMenu()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1000;
    return ((dScMgBase_c *)a)->dScMgBase_c::SaveScreenForMenu();
}

// @symbol _ZN12dScMgSlot3_c16InitMainScreenBgEv
/* dScMgSlot3_c::InitMainScreenBg - slot 32, the only override of it in the family.

   Not an AfterClsn: the base slot has no ROM name at all, and the one the
   recovery pass borrowed belongs to dPathLiftActor_c, two forks away.  See the
   slot-32 block in include/dScMgBase_c.h.

   The base's body verbatim -- main BG1CNT reduced to 0x1000, scroll reset, BG1
   cleared from the main BG-enable shadow, this class's own language table and
   the shared screen map -- and then one more write, leaving BG1CNT at 0x1118
   instead of 0x1000: the same layer, pointed at this minigame's own character
   and screen base blocks. */
void dScMgSlot3_c::InitMainScreenBg()
{
    int idx;

    *(volatile unsigned short*)0x400000a &= ~3;
    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short*)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    data_0209d45c &= ~2;

    idx = GetGameLanguage();
    LoadCompressedFileAt(sSlotPauseBgNcgByLanguage[idx], func_02054ea8());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_PAUSE_NSC_BIN, G2::GetBG1ScrPtr());

    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1118;
}

// @symbol Slot3_ReelDigits
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Slot3_ReelDigits(char *c, int idx);
void Slot3_ReelDigits(char *c, int idx){
    int d = (*(int*)(c + idx*4 + 0x4fe4) >> 12) / 80;
    int i;
    char *dst = c + idx*3;
    char *src = c + idx*5;
    for (i = 0; i < 3; i++) {
        *(unsigned char*)(dst + 0x5031) = *(unsigned char*)(src + d + 0x501c);
        d = (d + 1) % *(unsigned char*)(c + 0x503a);
        dst += 1;
    }
}
}

// @symbol Slot3_StateDone
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Slot3_StateDone(void) {}
}

// @symbol Slot3_StateGameOver
extern "C" {  /* .c-derived member: C linkage for the whole block */

void Slot3_StateGameOver(char *c) {
    dScMgSlot3_c *self = (dScMgSlot3_c *)c;
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        self->mState = 7;
        Mg_SetSharedState(MG_STATE_GAME_OVER);
        self->mDifficulty = 0;
    }
    if (self->mTimer >= 0x3c)
        return;
    {
        int* a = &self->mReelWinPos[0];
        int* b = &self->mReelWinPos[1];
        int* d = &self->mReelWinPos[2];
        *a = *a + 0x10000;
        *b = *b - 0x10000;
        *d = *d + 0x10000;
    }
}
}

// @symbol Slot3_StateWait
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Slot3_StateWait(void) {}
}

// @symbol Slot3_StatePayout
extern "C" void Slot3_StatePayout(char *c) {
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        if (*(int*)(c + 0x5000) == 3) {
            if (*(int*)(c + 0xb4) < 0x270f)
                *(int*)(((int)c + 0xb4)) += 1;
            if (*(int*)(c + 0xb4) > *(int*)(c + 0xb8))
                *(int*)(c + 0xb8) = *(int*)(c + 0xb4);
            ((dScMgSlot3_c*)c)->StartRound(4);
        } else {
            if (*(int*)(c + 0xa8) <= 0) {
                *(unsigned char*)(c + 0x503e) = 0x78;
                *(int*)(c + 0x5000) = 6;
            } else {
                ((dScMgSlot3_c*)c)->StartRound(5);
            }
        }
    }

    if (*(int*)(c + 0x5000) == 3) {
        if (*(unsigned char*)(c + 0x503e) != 0x14)
            return;
        if (*(int*)(c + 0x5014) <= 0)
            return;
        *(int*)(((int)c + 0x5014)) -= 1;
        Mg_AddCredits(c, 1);
        Sound::PlayBank2_2D(0x149);
        *(unsigned char*)(((int)c + 0x503e)) += 5;
        return;
    }

    if (*(int*)(c + 0x5000) != 4)
        return;

    {
        unsigned char b = *(unsigned char*)(c + 0x503e);
        if (b != 0x14 && b != 0x28 && b != 0x3c)
            return;
    }

    Mg_SubCredits(c, 1);
    Sound::PlayBank2_2D(0x14a);
}

// @symbol Slot3_StateJudge
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Slot3_StateJudge(char *c) {
    unsigned char buf[3];
    int i;
    int total;
    int arg;
    char *self;

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x503e)) != 0)
        return;

    self = c;
    for (i = 0; i < 3; i++) {
        int j;
        buf[i] = *(unsigned char *)(self + 0x5031 + i);
        j = 1;
        do {
            int rem = (i + (((int *)(self + 0x4fe4))[j] >> 12) / 80) % *(unsigned char *)(self + 0x503a);
            if (buf[i] != ((SlotGrid *)self)->sym[j][rem]) {
                buf[i] = 5;
                break;
            }
        } while (++j < 3);
        self = (char *)((unsigned)self | (unsigned)(i - i));
    }

    *(int *)(c + 0x5010) = -1;
    total = 0;
    {
        int k;
        for (k = 0; k < 3; k++) {
            if (buf[k] == *(unsigned char *)(c + 0x503b)) {
                if (k == 1) {
                    *(int *)(c + 0x5010) = k;
                    total += 6;
                } else if (k == 0 || k == 2) {
                    *(int *)(c + 0x5010) = k;
                    total += 3;
                }
                break;
            }
        }
    }

    arg = *(int *)(c + 0xb4);
    if (total > 0) {
        *(int *)(c + 0x5014) = total;
        *(int *)(c + 0x5000) = 3;
        func_02012790(0x26);
        *(unsigned char *)(c + 0x503e) = 0x28;
        arg += 1;
    } else {
        *(int *)(c + 0x5000) = 4;
        func_02012790(0xe);
        *(unsigned char *)(c + 0x503e) = 0x50;
    }
    Mg_SubmitScore(arg);

    *(unsigned char *)(c + 0xc3) = 0;
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off
// @symbol Slot3_StateSpin
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Slot3_StateSpin(char *c) {
    u8 idx;
    int ok;
    int off;
    int i;
    int new_var;
    int *tab;
    idx = gActivePlayerSlot;
    ok = 0;
    off = idx * 4;
    new_var = 1;
    if (gTouchHeld[idx * 4]) {
        if (gTouchEdge[off]) {
            ok = 1;
        }
    }
    if (ok != 0) {
        unsigned int v = gTouchY[idx * 4];
        if ((v >= 0x40) && (v < 0x58)) {
            if ((*((u8 *)(c + 0x502b))) == new_var) {
                *((u8 *)(c + 0x502b)) = 0;
            }
        } else if ((v >= 0x58) && (v < 0x70)) {
            if ((*((u8 *)(c + 0x502c))) == 1) {
                *((u8 *)(c + 0x502c)) = 0;
            }
        } else if ((v >= 0x70) && (v < 0x88)) {
            if ((*((u8 *)(c + 0x502d))) == new_var) {
                *((u8 *)(c + 0x502d)) = 0;
            }
        }
    }
    i = 0;
    for (; i < 3; i++) {
        if ((*((u8 *)((int)(((long long)((int)((c + i) + 0x502e))))))) == 0) {
            int *p = (int *)(c + (i * 4));
            int val;
            int q;
            int q2;
            u8 *dir;
            tab = data_ov006_0213e948;
            p = (int *)((int)(((long long)((int)(((int)p) + 0x4fe4)))));
            val = *p;
            ;
            q = (val >> 12) / 0x50;
            if ((*((u8 *)((int)(((long long)((int)((c + i) + 0x5040))))))) == 1) {
                *p = val - tab[((*((u8 *)(c + 0x503d))) * 3) + i];
                if ((*p) < 0) {
                    *p += *((int *)(c + 0x4ff0));
                }
            } else {
                *p = val + tab[((*((u8 *)(c + 0x503d))) * 3) + i];
                if ((*p) >= (*((int *)(c + 0x4ff0)))) {
                    *p -= *((int *)(c + 0x4ff0));
                }
            }
            if ((*((u8 *)((c + i) + 0x502b))) == 0) {
                q2 = ((*p) >> 12) / 0x50;
                if (q2 != q) {
                    if ((*((u8 *)((int)(((long long)((int)((c + i) + 0x5040))))))) == new_var) {
                        *p = (q * 0x50) << 12;
                    } else {
                        *p = (q2 * 0x50) << 12;
                    }
                    Slot3_ReelDigits(c, i);
                    *((u8 *)((int)(((long long)((int)((c + i) + 0x502e)))))) = 1;
                    *((u8 *)((int)(((long long)((int)(c + 0x503d)))))) += new_var;
                    *((u8 *)(c + 0x503f)) = 0;
                    _ZN5Sound12PlayBank2_2DEj(0x1a9);
                }
            }
        }
    }

    *((int *)(c + 0x5008)) = Sound_PlayIfNotActive(*((int *)(c + 0x5008)), 2, 0x1a8, 0);
    if ((*((u8 *)(c + 0x503d))) >= 3u) {
        *((u8 *)(c + 0x503e)) = 0x1e;
        *((int *)(c + 0x5000)) = 2;
    }
}
}

#pragma opt_loop_invariants on
#pragma opt_strength_reduction on

// @symbol Slot3_StateIntro
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Mascot_LookAt(char *c, int *src);
void Mg_ShowBanner(int c, int a1, int a2, int a3, int arg5, short arg6);
void Mascot_StopLooking(int *p);
void FreeGfxSlotsById(int arg);

void Slot3_StateIntro(char *p) {
    if (*(int *)(p + 0x500c) == 0x50) {
        Mascot_LookAt(p + 0x4f38, 0);
        Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    }

    if (*(int *)(p + 0x500c) > 0) {
        *(int *)(((int)p + 0x500c)) -= 1;
        if (*(int *)(p + 0x500c) != 0)
            return;
        *(unsigned char *)(p + 0x503b) = *(unsigned char *)(p + 0x503c);
        Mascot_StopLooking((int *)(p + 0x4f38));
        return;
    }

    *(unsigned char *)(((int)p + 0x503e)) -= 1;
    if (*(unsigned char *)(p + 0x503e) != 0)
        return;

    FreeGfxSlotsById(0x1d);

    {
        int *q = (int *)(p + 0x5000);
        if (*(unsigned char *)(p + 0xc4) == 0) {
            *(unsigned char *)(p + 0xc3) = 1;
            *(unsigned char *)(p + 0xc4) = 1;
            *(short *)(p + 0xc0) = 0;
        }
        *q += 1;
    }
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot3_c10StartRoundEi
void dScMgSlot3_c::StartRound(int mode)
{
    char *c = (char *)this;

    if (mode == 3 || mode == 0x12) {
        mCredits = 0xc;
        mCreditsShown = mCredits;
        mLevel = mDifficulty;
        if (mLevel > 0x270e) {
            mLevel = 0x270e;
        }
        mHudScore = 0;
        Mg_SubmitScore(mHudScore);
        Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    } else if (mode == 4) {
        if (mDifficulty < 5) {
            *(int *)(int)(c + 0x5004) += 1;
        }
        *(int *)(int)(c + 0xbc) += 1;
        if (mLevel > 0x270e) {
            mLevel = 0x270e;
        }
    }

    if (mode == 3 || mode == 0x12 || mode == 4) {
        int v;
        int v2;
        int i;
        int j;
        u8 k;
        int m;

        v = mDifficulty;
        if (v <= 0) {
            mStripLength = 3;
        } else if (v <= 2) {
            mStripLength = 4;
        } else {
            mStripLength = 5;
        }

        v2 = mDifficulty;
        if (v2 <= 0 || v2 == 1 || v2 == 3) {
            mReelReverse0 = 0;
            mReelReverse1 = 1;
            mReelReverse2 = 0;
        } else if (v2 == 2 || v2 == 4) {
            mReelReverse0 = 1;
            mReelReverse1 = 0;
            mReelReverse2 = 1;
        } else {
            for (i = 0; i < 3; i++) {
                if ((u8)(((u32)RandomIntInternal(&data_0209e650) >> 16) & 1)) {
                    *(u8 *)(c + i + 0x5040) = 1;
                } else {
                    *(u8 *)(c + i + 0x5040) = 0;
                }
            }
        }

        mTargetFace = ((u32)RandomIntInternal(&data_0209e650) >> 16) % mStripLength;
        if (mode != 4) {
            mReelDrawY = 0;
            mWinSymbol = mTargetFace;
        } else {
            mReelDrawY = 0x50;
            func_02012790(0x1aa);
        }

        {
            char *w = c;
            for (j = 0; j < 3; j++, w += 5) {
                for (k = 0; k < mStripLength; k++) {
                    if (j == 1) {
                        *(u8 *)(c + k + 0x5021) = mStripLength - k - 1;
                    } else {
                        *(u8 *)(w + k + 0x501c) = k;
                    }
                }
            }
        }
        mStripSpan = (mStripLength * 0x50) << 12;
    }

    {
        int j2;
        int k2;
        int m2;
        char *w2 = c;
        for (j2 = 0; j2 < 3; j2++, w2 += 3) {
            *(u8 *)(c + j2 + 0x502b) = 1;
            *(u8 *)(c + j2 + 0x502e) = 0;
            for (k2 = 0; k2 < 3; k2++) {
                *(u8 *)(w2 + k2 + 0x5031) = mStripLength;
            }
        }
        for (m2 = 0; m2 < 3; m2++) {
            mReelWinPos[m2] = 0;
        }
        mRowsStopped = 0;
        mTimer = 0x3c;
        mState = 0;
        mWinColumn = -1;
        mPayoutLeft = 0;
    }
}

#pragma opt_strength_reduction on

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot3_c6RenderEv
/* dScMgSlot3_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the second of the two slots where this class's
 * table differs from dScMgSingle3DBase_c's. The old file's `recovered name:`
 * comment agreed, and here it is right.
 *
 * Draws the three reels -- the win-line pass at state 6 and the scrolling pass
 * otherwise -- then the payout markers, the dMeter_c, and the two swinging lamps whose
 * angles are mLamp1Angle/mLamp2Angle.
 *
 * The pragma is load-bearing, not tidying; the `(int)` launder on the two indexed
 * reads at 0x4fe4/0x4ff4 is there for the same reason. Only the receiver changed:
 * `self` is `this` now instead of a char* parameter cast back to the class. */
s32 dScMgSlot3_c::Render()
{
    char *c = (char *)this;
    struct dScMgSlot3_c *self = this;
    int i, j;

    Camera_InitSlotsView(c + 0x4660);
    Mascot_Draw(c + 0x4f38);

    if (self->mReelDrawY > 0) {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
                0, sSlot3FaceSprites[self->mWinSymbol * 3 + i], 0x80, self->mReelDrawY + 0x10, -1,
                2, 0x1000, 0x1000, 0, 1);
        }
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
                0, sSlot3FaceSprites[self->mTargetFace * 3 + i], 0x80, self->mReelDrawY + 0x60, -1,
                2, 0x1000, 0x1000, 0, 1);
        }
    } else {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0,
                                                        sSlot3FaceSprites[self->mWinSymbol * 3 + i],
                                                        0x80, 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    }

    if (self->mState != 7) {
        if (self->mState == 6) {
            int i2, j2;
            char *p = c;
            for (i2 = 0; i2 < 3; i2++) {
                int y;
                for (j2 = 0, y = 0x30; j2 < 3; j2++) {
                    Hud_RenderSprite(sSlot3FaceSprites[*(u8 *)(p + j2 + 0x5031) * 3 + i2],
                                     y - (*(int *)((int)c + i2 * 4 + 0x4ff4) >> 12), 0x60, -1, -1);
                    y += 0x50;
                }
                p += 3;
            }
        } else {
            char *p = c;
            int row, rem;
            int i2, j2;
            for (i2 = 0; i2 < 3; i2++) {
                int a = *(int *)(((int)c + i2 * 4 + 0x4fe4)) >> 12;
                int y;
                row = a / 0x50;
                rem = a % 0x50;
                for (j2 = 0, y = 0x30; j2 < 4; j2++) {
                    Hud_RenderSprite(sSlot3FaceSprites[*(u8 *)(p + row + 0x501c) * 3 + i2], y - rem,
                                     0x60, -1, -1);
                    row = (row + 1) % self->mStripLength;
                    y += 0x50;
                }
                p += 5;
            }
        }
    }

    if (self->mRowsStopped < 3 && (self->mFrameCounter & 0x20)) {
        int slot = 3;
        int ok = 1;
        int i3, j3;
        char *p = c;
        for (i3 = 0; i3 < 3; i3++) {
            if (*(u8 *)(c + i3 + 0x502e) != 0) {
                if (slot < 3) {
                    if (self->mWinSymbol != *(u8 *)(p + slot + 0x5031))
                        ok = 0;
                } else {
                    u8 t = self->mWinSymbol;
                    for (j3 = 0; j3 < 3; j3++) {
                        if (t == *(u8 *)(p + j3 + 0x5031)) {
                            slot = j3;
                            break;
                        }
                        if (j3 == 2)
                            ok = 0;
                    }
                }
            }
            if (ok == 0)
                break;
            p += 3;
        }
        if (ok != 0 && slot < 3) {
            int count = 0;
            int i4;
            int y = 0x4c;
            for (i4 = 0; i4 < 3; i4++) {
                if (*(u8 *)(c + i4 + 0x502e) != 0) {
                    Hud_RenderSprite(&data_ov006_0213e5dc, slot * 0x50 + 0x30, y, -1, -1);
                    count++;
                }
                y += 0x18;
            }
            if ((self->mFrameCounter & 0x3f) == 0x20) {
                _ZN5Sound12PlayBank2_2DEj(count > 1 ? 0x1ac : 0x1ab);
            }
        }
    }

    Mg_DrawCredits(c, 0xc, 0xc, 0);
    Mg_DrawScoreTally(c, 0xe0, 0x14, 1);

    if (self->mState == 3 && self->mWinColumn >= 0) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2],
                              self->mWinColumn * 0x50 + 0x20, 0x28, 0);
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][13],
                              self->mWinColumn * 0x50 + 0x30, 0x28, 0);
        if (self->mWinColumn == 1) {
            Mg_DrawBigNumber(self->mWinColumn * 0x50 + 0x40, 0x28, 6, 0, 0, 0, 0x14);
        } else {
            Mg_DrawBigNumber(self->mWinColumn * 0x50 + 0x40, 0x28, 3, 0, 0, 0, 0x14);
        }
    } else if (self->mState == 4 && self->mWinColumn < 0) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2], 0x70, 0x28, 0);
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][14], 0x80, 0x28, 0);
        Mg_DrawBigNumber(0x90, 0x28, 3, 0, 0, 0, 0x28);
    }

    Mg_RenderMainSpriteScaled(sSlotLamp1Sprite, 0x18, 0x30, -1, 0, 0x1000, self->mLamp1Angle);
    Mg_RenderMainSpriteScaled(sSlotLamp2Sprite, 0x40, 0x10, -1, 0, 0x1000, self->mLamp2Angle);

    return 1;
}

#pragma opt_strength_reduction on

// @symbol _ZN12dScMgSlot3_c8BehaviorEv
/* dScMgSlot3_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the two slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Steps the reel state machine through the pointer-to-member table at
 * sSlot3StateTable, ticks the frame counter at 0x503f, spins the two reel
 * offsets while state 1 is running, and flips the blend bit in data_0209d45c off a
 * coin toss so the machine's lights flicker.
 *
 * The state table uses this class's pointer-to-member type.
 */
s32 dScMgSlot3_c::Behavior()
{
    dScMgSlot3_c *self = this;
    int i;
    unsigned char t;

    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;

    (self->*sSlot3StateTable[self->mState])();

    pc = (unsigned char *)((char *)self + 0x503f);
    *pc = *pc + 1;
    Mascot_Update((char *)self + 0x4f38);

    for (i = 0; i < 3; i++) {
        if (self->mState == 1) {
            ph1 = (unsigned short *)((char *)self + 0x5018);
            ph2 = (unsigned short *)((char *)self + 0x501a);
            *ph1 = *ph1 - 0x200;
            *ph2 = *ph2 - 0x400;
            break;
        }
    }

    t = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1;
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= ~2;

    return 1;
}

// @symbol _ZN12dScMgSlot3_c13InitResourcesEv
/* Load the reel assets and initialise the scene state. */
s32 dScMgSlot3_c::InitResources()
{
    char *c = (char *)this;

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1118;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1218;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1418;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~3;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~3;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN, (void *)func_02054d88());

    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_FACE_NCL_BIN);
        _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE02_NSC_BIN, _ZN2G212GetBG1ScrPtrEv());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NUKI_NSC_BIN, _ZN2G212GetBG2ScrPtrEv());

    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_FACE_NSC_BIN);
        func_02056314((void *)h, 0, 0x800);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(ASSET_D_2D_MG_SLOT_FACE_NCG_BIN, (void *)0x6400000);

    data_0209d45c = 0x1f;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_NCG_BIN, _ZN3G2S13GetBG2CharPtrEv());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_FACE_DOWN_NSC_BIN, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(ASSET_D_2D_MG_SLOT_FACE_NCG_BIN, (void *)0x6600000);

    {
        int h = LoadFile(ASSET_D_2D_MG_SLOT_FACE_NCL_BIN);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)h, 0xe0);
        _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x14;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xc, 0xc, 4);

    Mascot_Load(c + 0x4f38);
    Camera_InitSlotsView(c + 0x4660);

    mDifficulty = (int)mLevel;

    ((dScMgSlot3_c *)c)->StartRound(3);

    mWinSymbol = mTargetFace;
    mReelDrawY = 0;

    for (int i = 0; i < 3; i++) {
        int rnd = RandomIntInternal(&data_0209e650);
        u32 divisor = mStripLength;
        u32 val = ((u32)rnd >> 16) % divisor;
        ((u32 *)(c + 0x4fe4))[i] = val * 0x50000;
    }

    sMgWinBannerX = 0x80;
    sMgWinBannerY = 0xa0;
    sMgLoseBannerX = 0x80;
    sMgLoseBannerY = 0xa0;
    sMgMainBannerX = 0x80;
    sMgMainBannerY = 0x60;
    sMgResultsBannerX = 0x80;
    sMgResultsBannerY = ~0x1b;

    Mg_SetTopScreenYOffset(0x20);

    data_0208ee44 = 1;

    mLamp1Angle = 0;
    mLamp2Angle = 0;
    return 1;
}

// @symbol dScMgSlot3_c_classInit
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  1 func_ov006_*                unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  3 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 *  2 _ZTV vptr store(s)          stands in for the ctor that would emit it.
 */
/* Reconstructed source-style name: SM64DS proves dScMgSlot3_c through RTTI,
 * allocation size, vtable identity, and the MG_SLOT3 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0210c120. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *dScMgSlot3_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x5044);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);

        p[0] = (int)&_ZTV12dScMgSlot3_c[2];
        Mascot_Construct((char *)p + 0x4f38);
    }
    return p;
}
}

// @symbol SlotLamp_Start
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotLamp_Start(void *raw){
    char *blink = (char *)raw;
    *(int *)blink = 0x48;
    *(u8 *)(blink + 4) = 1;
    data_0209d454 |= 2;
}
}

// @symbol SlotLamp_Tick
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotLamp_Tick(void *o_)
{
    int *timer = (int *)o_;
    if (*timer <= 0)
        return;
    *timer -= 1;
    if ((*timer & 7) != 0)
        return;
    *((u8 *)timer + 4) ^= 1;
    if (*((u8 *)timer + 4) != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}
}

// @symbol SlotLamp_Clear
/* 0x469c blinks bit 1 of data_0209d454 the same way: a timer and an on
 * flag. This clears it, 0210c180 starts it and 0210c1a8 ticks it. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotLamp_Clear(char *blink)
{
    *(int *)(blink + 0x0) = 0;
    *(char *)(blink + 0x4) = 0;
}
}

// @symbol SlotBlink_Start
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBlink_Start(void *raw, s16 x, s16 y)
{
    u8 *sprite = (u8 *)raw;
    *(u16 *)(sprite + 0) = x;
    *(u16 *)(sprite + 2) = y;
    *(int *)(sprite + 4) = 0x48;
    *(u8 *)(sprite + 8) = 1;
}
}

// @symbol SlotBlink_Draw
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBlink_Draw(unsigned char* raw){
    if (*(u8 *)(raw + 8) == 0) return;
    RenderOamMainScreen(&sSlotsSlotBlinkSprites, *(short *)raw, *(short *)(raw + 2), -1, -1);
}
}

// @symbol SlotBlink_Tick
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBlink_Tick(void *o_)
{
    u8 *raw = (u8 *)o_;
    if (*(int *)(raw + 4) <= 0) return;
    int *timer = (int *)(raw + 4);
    *timer -= 1;
    if ((*(int *)(raw + 4) & 7) == 0) {
        *(raw + 8) ^= 1;
    }
}
}

// @symbol SlotBlink_Clear
/* 0x4684 and 0x4690 each hold a blinking sprite: s16 x and y, a timer at
 * +4 and a visible flag at +8. This clears one, 0210c218 starts one, 0210c278
 * ticks it and 0210c234 draws it. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBlink_Clear(char *sprite)
{
    *(int *)(sprite + 0x4) = 0;
    *(char *)(sprite + 0x8) = 0;
}
}

// @symbol SlotReel_SetSpeed
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotReel_SetSpeed(void *raw, int value) {
    int *words = (int *)raw;
    words[8] = (words[7] * value) << 2;
}
}

// @symbol SlotBet_OnTouch
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBet_OnTouch(void *c_)
{
    dThIcon_c *icon = (dThIcon_c *)c_;
    void *scene;
    int count;
    unsigned int sid;

    if (icon->unk_01c >= 3) {
        return;
    }
    if (icon->unk_010 != 0) {
        return;
    }
    scene = gMgScene;
    count = (scene != 0) ? ((dScMgBase_c *)scene)->mCredits : 0;
    if (count <= 0) {
        return;
    }
    icon->unk_01c += 1;
    Mg_LoseCredits((void *)1);
    func_ov001_020ab3f0(icon);
    sid = 0x163;
    Sound::PlayBank2_2D(sid);
}
}

// @symbol SlotBet_Reset
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBet_Reset(void *raw)
{
    dThIcon_c *icon = (dThIcon_c *)raw;
    icon->unk_01c = 0;
    icon->unk_020 = 0;
    if (icon->unk_014 >= 1)
        icon->unk_011 = 1;
}
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
void dScMgSlot1_c::betIcon_c::Render()
{
    OAM::RenderSub((OamAttr *)data_ov006_0213e528[unk_010], unk_004, unk_006, -1, 1);
    int i;
    int y = 0xb0;
    int zero = 0;
    for (i = 0; i < unk_01c; i++) {
        Mg_RenderSubSpriteMtx(sSlotLangSprites[GetGameLanguage()][2], 0xb0 + i * 0x10, y,
                              (void *)zero);
    }
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
void dScMgSlot1_c::betIcon_c::Behavior()
{
    int val;
    if (TouchArea_Update(this, -1))
        SlotBet_OnTouch(this);
    val = unk_020;
    if (val != 0) {
        if ((val & 3) == 0) {
            Mg_GainCredits(1);
            Sound::PlayBank2_2D(0x149);
        }
        unk_020 -= 1;
    }
    dThIcon_c::Behavior();
}

// @symbol SlotBet_Place
extern "C" {  /* .c-derived member: C linkage for the whole block */
void SlotBet_Place(char *icon) {
    func_ov001_020ab5b0(icon, 1, 0x90, 0xb0, 0x10, 8);
    *(int *)(icon + 0x18) = 6;
}
}

// @symbol _ZN12dScMgSlot1_c9CloseMenuEv

int dScMgSlot1_c::CloseMenu()
{
    dScMgBase_c::CloseMenu();
    SetSubBg1Offset(0x100, 0);
}

// @symbol _ZN12dScMgSlot1_c8OpenMenuEv
/* Slot 27. Takes no argument: the ROM body never reads r1. The qualified
 * base call is the ROM's direct branch. */
void dScMgSlot1_c::OpenMenu()
{
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OpenMenu();
}

// @symbol SlotReels_CheckStopped
extern "C" {  /* .c-derived member: C linkage for the whole block */
int SlotReels_CheckStopped(void *self)
{
    struct SlotReels *p = (struct SlotReels *)self;
    int ok;
    int lead;
    int i;

    p->wilds = 0;
    ok = 1;
    for (i = 0; i < 3; i++) {
        if (p->reels[i][p->stops[i]] == 5) {
            p->wilds++;
        } else {
            lead = p->reels[i][p->stops[i]];
            break;
        }
    }
    if (p->wilds >= 3) {
        lead = 5;
    } else if (i < 2) {
        for (i = i + 1; i < 3; i++) {
            u8 sym = p->reels[i][p->stops[i]];
            if (sym == 5) {
                p->wilds++;
            } else if (lead != sym) {
                ok = 0;
            }
        }
    }
    if (ok) {
        if (p->speedLevel < 2) {
            u8 *q = (u8 *)p + 0x4708;
            *q += 1;
        }
        p->matchSymbol = lead;
    }
    return ok;
}
}

// @symbol Slot1_InitHud
extern "C" {  /* .c-derived member: C linkage for the whole block */
void Slot1_InitHud(void *self)
{
    char *raw = (char *)self;
    *(s8 *)(raw + 0x4706) = -1;
    *(u8 *)(raw + 0x4707) = 1;
    *(u8 *)(raw + 0x4708) = 0;
    dScMgSlot1_c *scene = (dScMgSlot1_c *)self;
    scene->mCredits = Mg_GetSavedRecord0();
    scene->mCreditsShown = scene->mCredits;
}
}

// @symbol _ZN12dScMgSlot1_c10StartRoundEi
/* Slot 18. 0x4706 is padding in the header, so it is written by offset. */
void dScMgSlot1_c::StartRound(int i)
{
    char *raw = (char *)this;

    if (i == 4) {
        *(u8 *)(raw + 0x4706) = mMatchSymbol;
    } else if (i == 3) {
        Slot1_InitHud(raw);
    }
    SlotBet_Reset(&mBetIcon);
    mStep = 0;
}

#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot1_c6RenderEv
/* dScMgSlot1_c::Render -- the recovered symbol at this address
   (_ZN3OAM7SECONDSE, i.e. "OAM::SECONDS") was a name-recovery-heuristic
   artifact, not a real name; this function genuinely is Render (see
   include/dScMgSlot1_c.h's file banner). mBetIcon.Render() is the ROM's
   virtual call through the typed nested betIcon_c member at 0x4660.
   `this+0xa8` is inherited from further up the hierarchy than dScMgBase_c,
   so it stays a raw offset on an unsigned char* cast. */
s32 dScMgSlot1_c::Render()
{
    unsigned char* t = (unsigned char*)this;
    int i, pos, x, j;
    unsigned char* cur = t;
    unsigned char b;
    for (i = 0; i < 3; i++) {
        pos = *(t + i + 0x46ff);
        x = sSlot1ReelPos[i][1] - (*(int *)(t + (i << 2) + 0x46a4) >> 12);
        for (j = 0; j < 2; j++) {
            Hud_RenderSprite(sSlot1Sprites[*(cur + pos + 0x46c0)], sSlot1ReelPos[i][0], x, -1, 3);
            pos--;
            x += 0x40;
            if (pos < 0)
                pos += 0x15;
        }
        cur += 0x15;
    }
    mBetIcon.Render();
    Mg_DrawBigNumber(0x70, 0xb0, mCredits, 0, 1, 1, 0x14);
    if (mStep >= 5 && (*(t + 0x470a) != 0 || (*(t + 0x470b) != 0 && *(t + 0x470b) < 3))) {
        b = *(t + 0x470b);
        if (b != 0 && b < 3) {
            i = 0;
            if ((int)b > 0) {
                pos = 0x18;
                do {
                    Mg_RenderSubSpriteScaled(sSlot1Sprites[0xb], pos, 0x30, -1, 2, 0x1000, 0);
                    pos += 0x10;
                    i++;
                } while (i < *(t + 0x470b));
            }
            Mg_RenderSubSpritePalMtx(sSlotLangSprites[GetGameLanguage()][1], 0x50, 0x30, -1, 2, 0);
            Mg_DrawBigNumber(0x60, 0x30, *(t + 0x470b) * 2, 0, 2, 2, 0x14);
        }
        if (*(t + 0x470a) != 0) {
            i = 0;
            pos = 0x18;
            for (; i < 3; i++) {
                Mg_RenderSubSpriteScaled(sSlot1Sprites[*(t + 0x4709) + 6], pos, 0x40, -1, 2, 0x1000,
                                         0);
                pos += 0x10;
            }
            Mg_RenderSubSpritePalMtx(sSlotLangSprites[GetGameLanguage()][1], 0x50, 0x40, -1, 2, 0);
            Mg_DrawBigNumber(0x60, 0x40, sSlot1Payouts[*(t + 0x4709)], 0, 2, 2, 0x14);
        }
    }
    SlotBlink_Draw(t + 0x4684);
    SlotBlink_Draw(t + 0x4690);
    return 1;
}

#pragma opt_strength_reduction on

// @symbol _ZN12dScMgSlot1_c8BehaviorEv
/* Mario Slot's per-frame state machine (vtable slot 6). Byte-exact since
 * 2026-10-12; the 19-word residue banked as a compiler floor was a source
 * artefact. The object is spelled ONE way here, through members: a draft
 * that read some fields as members and others through `char *c =
 * (char *)this` with literal offsets made 2004/b56 form the hoisted touch
 * record address as one folded `add r0, r0, r2, lsl #2` (0x818); with a
 * single name it materializes `lsl r1, r2, #2` / `add r0, r0, r1` as the
 * cartridge does (+0x268). Case 7 is the one deliberate exception: the ROM
 * walks the reel-symbol rows with a strided copy of `this` (mov r5, r4).
 * The touch x/y are read INSIDE the hit loop through TouchRec, not hoisted
 * by hand; the compiler hoists them and that colours the loop. */
s32 dScMgSlot1_c::Behavior()
{
    int i;

    switch (mStep) {
    case 0:
        for (i = 0; i < 3; i++) {
            mReelSpinning[i] = 1;
        }
        mReelsStopped = 0;
        mLineMatched = 0;
        mWilds = 0;
        mStep = 1;
        SlotBet_OnTouch(&mBetIcon);
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        mTimer = 0x3c;
        break;
    case 1:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            Sound::PlayBank2_2D(0x160);
            mTimer = 0x1e;
            mStep = 2;
        }
        break;
    case 2:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            mStep = 3;
        }
        break;
    case 3: {
        int moving = 0;
        for (i = 0; i < 3; i++) {
            if (mReelSpinning[i] == 1 || mReelScroll[i] != 0) {
                moving++;
                mReelScroll[i] -= sSlotReelScrollSpeeds[mSpeedLevel + i];
                if ((int)mReelScroll[i] < 0) {
                    if (mReelSpinning[i] == 1) {
                        mReelStop[i]++;
                        mReelStop[i] %= 21;
                        mReelScroll[i] += 0x40000;
                    } else {
                        mReelScroll[i] = 0;
                    }
                }
            }
        }
        if (moving != 0) {
            mSpinSound = Sound_PlayIfNotActive(mSpinSound, 2, 0x161, 0);
        }
        if (mReelsStopped < 3) {
            int hit = 0;
            u8 idx = gActivePlayerSlot;
            if (gTouchHeld[idx * 4] != 0) {
                if (gTouchEdge[idx * 4] != 0) {
                    hit = 1;
                }
            }
            if (hit != 0) {
                for (i = 0; i < 3; i++) {
                    if (mReelSpinning[i] == 1) {
                        int dx = ((TouchRec *)gTouchHeld)[idx].x - sSlot1ReelPos[i][0];
                        int dy = ((TouchRec *)gTouchHeld)[idx].y - sSlot1ReelPos[i][1];
                        if (dx <= 0x20 && dx >= -0x20 && dy <= 0x20 && dy >= -0x20) {
                            mReelSpinning[i] = 0;
                            mReelsStopped++;
                            if (mBetIcon.unk_011 != 0) {
                                mBetIcon.unk_011 = 0;
                                Mg_SaveRecordPair(mCredits, mHudScore);
                            }
                            func_02012718(0x162, sSlot1ReelPos[i][0] << 12);
                            break;
                        }
                    }
                }
            }

        } else {
            int stopped = 1;
            for (i = 0; i < 3; i++) {
                if (mReelScroll[i] != 0) {
                    stopped = 0;
                }
            }
            if (stopped == 1) {
                mLineMatched = SlotReels_CheckStopped(this);
                mPayout = 0;
                if (mLineMatched != 0) {
                    mPayout += sSlot1Payouts[mMatchSymbol];
                }
                if (mWilds != 0 && mWilds < 3) {
                    mPayout += mWilds << 1;
                }
                Mg_SaveRecordPair(mCredits + mPayout * mBetIcon.unk_01c, mHudScore);
                mStep = 4;
                mTimer = 0x3c;
            }
        }
        break;
    }
    case 4:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            if (mLineMatched != 0) {
                SlotBlink_Start(pad_4684, sSlotMatchBlinkX[mMatchSymbol][0],
                                sSlotMatchBlinkY[mMatchSymbol][0]);
            }
            if (mWilds != 0 && mWilds < 3) {
                SlotBlink_Start(pad_4690, sSlotWildBlinkX[mWilds - 1][0],
                                sSlotWildBlinkY[mWilds - 1][0]);
            }
            if (mPayout != 0) {
                mStep = 5;
                mTimer = 0x64;
                SlotLamp_Start(pad_469c);
                func_02012790(0x26);
            } else {
                mStep = 7;
                mTimer = 0x3c;
                func_02012790(0xe);
            }
            mPromptEnabled = 0;
        }
        break;
    case 5:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            SlotReel_SetSpeed(&mBetIcon, mPayout);
            mTimer = 0x3c;
            mStep = 6;
        }
        break;
    case 6: {
        int done = (mBetIcon.unk_020 == 0) ? 1 : 0;
        if (done != 0) {
            Mg_SubmitScore(mCredits);
            if (DecIfAbove0_Byte(&mTimer) == 0) {
                StartRound(4);
            }
        }
        break;
    }
    case 7:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            if (mCredits > 0) {
                StartRound(5);
            } else {
                int j;
                char *p = (char *)this;
                for (j = 0; j < 3; j++) {
                    mReelScroll[j] += 0x40000;
                    mReelStop[j]++;
                    mReelStop[j] %= 21;
                    *(u8 *)(p + mReelStop[j] + 0x46c0) = 0xd;
                    p += 0x15;
                }
                mStep = 9;
            }
        }
        break;
    case 9:
    case 11: {
        int idx = (mStep - 9) >> 1;
        mReelScroll[idx] -= 0x2000;
        if ((int)mReelScroll[idx] < 0) {
            mReelScroll[idx] = 0;
            mTimer = 0xa;
            mStep++;
            func_02012718(0x164, sSlot1ReelPos[idx][0] << 12);
            if (idx == 0) {
                Mg_PlayJingle((void *) 2);
            }
        }
        break;
    }
    case 13:
        mReelScroll[2] -= 0x2000;
        if ((int)mReelScroll[2] < 0) {
            mReelScroll[2] = 0;
            mTimer = 0x3c;
            mStep = 8;
            func_02012718(0x164, sSlot1ReelPos[2][0] << 12);
        }
        break;
    case 8:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            dScene_c::SetFaders(&data_0209f61c);
            data_0209f61c.func_0202ec9c(2);
            dScene_c::StartSceneFade(5, 0, 0);
        }
        break;
    case 10:
    case 12:
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            mStep++;
        }
        break;
    }
    mBetIcon.Behavior();
    SlotBlink_Tick(pad_4684);
    SlotBlink_Tick(pad_4690);
    SlotLamp_Tick(pad_469c);
    return 1;
}

// @symbol _ZN12dScMgSlot1_c13InitResourcesEv
/* The ov004 scroll words (sMgScoreDigitsX..sMgMainBannerX) name
 * no struct; they are stored raw. The reel-table fill stays a raw char*
 * cursor over 0x46c0..0x46ff (see leftovers). */
s32 dScMgSlot1_c::InitResources()
{
    char *c = (char *)this;
    volatile unsigned short fill;
    int i, j;

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1210;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1310;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_UP_NCG_BIN, (void *)func_02054d88());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN, _ZN2G213GetBG2CharPtrEv() + 0x7800);
    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_UP_NCL_BIN);
        _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_SCORE_UP_NSC_BIN, _ZN2G212GetBG2ScrPtrEv());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_UP_NSC_BIN, _ZN2G212GetBG3ScrPtrEv());

    data_0209d45c = 0x1c;

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x1410;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & ~3) | 1;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0x4010;
    *(volatile u16 *)0x4001008 = *(volatile u16 *)0x4001008 & ~0x40;
    SetSubBg1Offset(0x100, 0);

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1310;
    *(volatile u16 *)0x400100e = *(volatile u16 *)0x400100e & ~0x40;

    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_DOWN_NCG_BIN, _ZN3G2S13GetBG2CharPtrEv());
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_GRADE_NCG_BIN, _ZN3G2S13GetBG0CharPtrEv() + 0x7800);
    {
        int h = LoadFile(ASSET_D_2D_MG_BG_SLOT_DOWN_NCL_BIN);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }
    LoadCompressedFileAt(ASSET_D_2D_GRADE_NSC_BIN, _ZN3G2S12GetBG0ScrPtrEv());
    LoadCompressedFileAt(ASSET_D_2D_BADWIZER_NSC_BIN, _ZN3G2S12GetBG1ScrPtrEv() + 0x800);
    LoadCompressedFileAt(ASSET_D_2D_MG_BG_SLOT_DOWN_NSC_BIN, _ZN3G2S12GetBG2ScrPtrEv());

    {
        char *d3 = _ZN3G2S12GetBG3ScrPtrEv();
        fill = 0x43e4;
        MultiStore16(fill, d3, 0x800);
    }

    LoadCompressedFileAt(sSlotNcgByLanguage[GetGameLanguage()], (void *)0x6400000);
    LoadCompressedFileAt(sSlotNcgByLanguage[GetGameLanguage()], (void *)0x6600000);

    {
        int h = LoadFile(ASSET_D_2D_MG_SLOT_NCL_BIN);
        _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0x40);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0x40);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x1d;
    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 1, 0x1c, 7, 9);

    sMgWinBannerX = 0x80;
    sMgWinBannerY = 0x40;
    sMgLoseBannerX = 0x80;
    sMgLoseBannerY = 0x40;
    sMgMainBannerX = 0x80;
    sMgMainBannerY = 0x40;

    if ((int)param1 & 0xff) {
        unsigned char *tbl = sSlotReelSymbolsAlt;
        char *p = c;
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 0x15; i++) {
                *(u8 *)(p + 0x46c0 + i) = tbl[i];
            }
            mReelStop[j] = 0;
            mReelScroll[j] = 0x40000;
            mReelSpinning[j] = 1;
            tbl += 0x15;
            p += 0x15;
        }
    } else {
        int k;
        unsigned char *tbl = sSlotReelSymbols;
        char *p = c;
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 0x15; i++) {
                *(u8 *)(p + 0x46c0 + i) = tbl[i];
            }
            mReelStop[k] = 0;
            mReelScroll[k] = 0x40000;
            mReelSpinning[k] = 1;
            tbl += 0x15;
            p += 0x15;
        }
    }

    mMatchSymbol = 5;
    mStartCredits = 0xa;
    Mg_SubmitScore(mStartCredits);
    SlotBet_Place((char *)&mBetIcon);
    StartRound(3);
    Mg_SetTopScreenYOffset(0x20);
    data_0208ee44 = 1;
    return 1;
}

// @symbol dScMgSlot1_c_classInit
/* The name is reconstructed; the ROM proves the class, not the spelling.
 * The allocation, the base C2 call and the three vptr stores are written out
 * by hand: the ROM has no dScMgSlot1_c C1, so `new dScMgSlot1_c` cannot
 * link. This TU emits _ZTV12dScMgSlot1_c and the betIcon_c vtable itself, so
 * their stores take the object-relative +8 the Slot3 factory above uses;
 * dThIcon_c's table is ov001's and is stored by its configured address. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void *dScMgSlot1_c_classInit(void)
{
    dScMgSlot1_c *scene =
        (dScMgSlot1_c *)_ZN7fBase_cnwEj(sizeof(dScMgSlot1_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int *)scene = (int)&_ZTV12dScMgSlot1_c[2];
        dScMgSlot1_c::betIcon_c *icon = &scene->mBetIcon;
        *(int *)icon = (int)_ZTV9dThIcon_c;
        *(int *)icon = (int)&_ZTVN12dScMgSlot1_c9betIcon_cE[2];
        SlotBlink_Clear((char *)scene->pad_4684);
        SlotBlink_Clear((char *)scene->pad_4690);
        SlotLamp_Clear((char *)scene->pad_469c);
    }
    return scene;
}
}

/* Static-init globals (was the handwritten __sinit_ov006_02132894 shard).
 * The Slot3 state table plain-copied from the ROM PMF constants, in
 * retail copy order. */
SlotState sSlot3StateTable[8] = {
    pmf_Slot3_StateIntro,    pmf_Slot3_StateSpin, pmf_Slot3_StateJudge,    pmf_Slot3_StatePayout,
    pmf_Slot3_StatePayout_2, pmf_Slot3_StateWait, pmf_Slot3_StateGameOver, pmf_Slot3_StateDone,
};
