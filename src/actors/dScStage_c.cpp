//cpp
/* dScStage_c -- the playable level scene: pause screen, level-clear and VS
 * exit groups, arm9.
 *
 * .text 0x02023688..0x02029408. The destructor pair plus the VE_ (VS exit),
 * LC_ (level clear) and PS_ (pause screen) groups, in ROM order. The rest of
 * the class -- InitResources, Behavior, Render, UpdateMessage, the graph
 * callbacks, the factory dScStage_c_classInit -- sits in later ranges
 * interleaved with the message and camera systems; folding those needs
 * multi-range claims.
 *
 * _ZTS10dScStage_c 0x02092170. _ZTI10dScStage_c 0x02092158, base word
 * _ZTI8dScene_c. _ZTV10dScStage_c 0x020921c0. The out-of-line destructor is
 * the key function. Under `#pragma defer_codegen off` it emits D1
 * (0x02023688), D0 (0x020236f0), then a base-object D2 the cartridge does
 * not carry.
 *
 * Leftover: OAM::Render, OAM::RenderSub, Sound::PlaySub and the dScene_c
 * fade stay mangled (Fix12 by value). RenderNumber keeps a local extern
 * spelled over the member's mask-emitting parameter conversions.
 * CheckCameraInput keeps `#pragma opt_strength_reduction off` and a local
 * CamInput overlay of the camera input array; PS_Update keeps
 * `#pragma opt_common_subs off`, the stale-tmpv levers in its case 0xa and
 * the address-rematerialization pokes in its case-1 star scroll. The touch
 * and controller globals (gTouchHeld..gTouchY, gActivePlayerSlot, data_0209f498)
 * have no header; data_ov001_ and data_ov002_ handles keep linker names.
 */
#include "types.h"
#include "dScStage_c.h"
#include "Timer.h"
#include "decl_common.h"
#include "Sound.h"
#include "PlayerInput.h"

struct OamAttr;
struct Matrix2x2;

/* TU-local overlay of the camera input records at data_0209f498, one per
 * controller: the pause screen polls the camera-state byte at +0x16. */
struct CamInput {
    u8 pad0[4];
    u16 held;      /* 0x4 */
    u16 pressed;   /* 0x6 */
    u8 pad8[0xc];
    u8 f14;        /* 0x14 */
    u8 f15;
    u8 f16;        /* 0x16 */
    u8 f17;
};

/* TU-local overlay of the touch records the ROM keeps at gTouchHeld:
   held/edge are the gTouchHeld/gTouchEdge lanes, x/y the gTouchX/gTouchY lanes. */
struct TouchInfo {
    u8 held;
    u8 edge;
    u8 x;
    u8 y;
};

extern "C" {
int IsButtonInputValid(void);
int SublevelToLevel(int lv);
int GetOwnerLanguage(void);
void GiveLives(int n);
void Deallocate(void *ptr);
void SetBg2Offset(int x, int y);
void SetBg3Offset(int x, int y);
void SetBg1Offset(int x, int y);
void SetSubBg0Offset(int x, int y);
void SetSubBg1Offset(int x, int y);
unsigned int func_02012790(unsigned int snd);
int func_02029408(void);
int LoadFile(int handle);
void SetControllerMode(unsigned char m);
unsigned int LoadCompressedFileAt(unsigned short id, void *dst);
int GetSoundMode(void);
void SetSoundMode(int m);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
int IsStarCollected(int lv, int star);

void _ZN7Message17DisplayVsExitTextEt(unsigned short n);
void _ZN7Message21DisplayLevelClearTextEta(int m, unsigned char lvl);
void _ZN7Message21DisplaySaveStatusTextEt(int m);
void _ZN7Message7DisplayEj(int m);
void _ZN7Message11DisplayTextEt(int m);
void _ZN7Message16DisplayPauseTextEth(int m, unsigned char a);
void _ZN7Message18DisplayPauseTextVSEt(int m);
void _ZN7Message22DisplayOptionsMenuTextEt(int m);
void _ZN7Message19DisplaySaveMenuTextEt(int m);
void _ZN7Message19DisplayDontSaveTextEt(int m);
void _ZN7Message25DisplayControllerModeTextEt(int m);
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fix, int e);
unsigned int _ZN5Sound12PlayBank3_2DEj(unsigned int a);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int a);
int _ZN8SaveData15SaveCurrentFileEv(void);
unsigned char _ZN8SaveData13GetCoinRecordEj(unsigned int lv);
void _ZN8SaveData21SetCoinRecordIfHigherEah(int a, int b);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p, int val, short amt);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
void _ZN3OAM14BOUNCING_ARROWE(void);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, void *attr, int x, int y, int a, int cc, int fx, int fy, int rot, int mode);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool sub, OamAttr *attr, int x, int y, int a, int cc, Matrix2x2 *mtx);
void _ZN3OAM9RenderSubEP7OamAttrii(void *attr, int x, int y);
int __aeabi_idiv(int a, int b);

extern int data_0208ee44;
extern u8 data_0208ee3c;
extern u8 data_0208ee40;
extern s8 data_02092124;
extern s16 data_02092144[];
extern unsigned int data_0209b454;
extern unsigned int data_0209b464;
extern u8 data_020755b8[];
extern volatile int data_02075610[];
extern u16 data_020755cc[];
extern u16 data_020755c0[];
extern u16 data_020755c4[];
extern s16 data_020756d0[];
extern u8 data_0209d660;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern Timer data_0209d4c8;
extern int data_0209fc68;
extern u8 data_0209f1ec;
extern u8 data_0209f1fc;
extern u8 data_0209f210;
extern u8 data_0209f218;
extern u8 data_0209f20c;
extern u8 data_0209f21c;
extern u8 data_0209f22c;
extern u8 data_0209f234;
extern u8 data_0209f238;
extern u8 data_0209f23c;
extern u8 data_0209f240;
extern u8 data_0209f244;
extern u8 data_0209f248;
extern u8 data_0209f250;
extern u8 data_0209f260;
extern u8 data_0209f280;
extern u8 data_0209f284;
extern u8 data_0209f290;
extern u8 data_0209f294;
extern u8 data_0209f29c;
extern u8 data_0209f2a0[];
extern u8 data_0209f2a4;
extern u8 data_0209f2a8;
extern u8 data_0209f2ac;
extern u8 data_0209f2b0;
extern u8 data_0209f2b4;
extern u8 data_0209f2b8;
extern u8 data_0209f2c4;
extern u8 data_0209f2c8;
extern u8 data_0209f2cc;
extern u8 data_0209f2d4;
extern u8 data_0209f2d8;
extern u8 data_0209f2dc;
extern u8 data_0209f2e0;
extern u8 data_0209f2e4;
extern u8 data_0209f2ec;
extern u8 data_0209f2f0;
extern s8 data_0209f2f8;
extern u16 data_0209f300;
extern u16 data_0209f360[];
extern u16 data_0209f368[];
extern u8 data_0209f350[];
extern void *data_0209f318;
extern unsigned char data_0209f498[];
extern u8 data_0209f4ae[];
extern int data_020a0db0;
extern u8 data_ov002_02111150;
extern u8 data_ov002_02111178;
extern u8 data_ov002_02111180;
extern int data_ov002_0210cea8;
extern int data_ov002_0210cee8;
extern int data_ov002_0210cf28;
extern int data_ov002_0210cf78;
extern int data_ov002_0210cb14;
extern int data_ov002_0210ce90;
extern int data_ov002_0210ced0;
extern int data_ov002_0210cf08;
extern int data_ov002_0210cfa0;
extern int data_ov002_0210caf4;
extern void *data_ov001_020abd98[];
extern void *data_ov001_020ac218[];
extern void *data_ov001_020ac64c[];
extern void *data_ov001_020acb68[];
extern void *_ZN3OAM21CONTROLLER_MODE_TEXTSE[];
extern OamAttr *_ZN3OAM7NUMBERSE[];
extern char _ZN3OAM8VS_PAUSEE;
extern char _ZN3OAM19ARROW_POINTING_LEFTE;
extern char _ZN3OAM20ARROW_POINTING_RIGHTE;
extern char _ZN3OAM13MM_SMALL_STARE;
extern char _ZN3OAM5PAUSEE;
extern char _ZN3OAM9TINY_STARE;
extern char _ZN3OAM25SMALL_ARROW_POINTING_LEFTE;
extern char _ZN3OAM26SMALL_ARROW_POINTING_RIGHTE;
extern char _ZN3OAM16SMALL_STAR_EMPTYE;
extern OamAttr _ZN3OAM4COINE;
extern OamAttr _ZN3OAM5TIMESE;
}

// Outside extern "C" so the namespace mangles to _ZN3G2S12GetBG1ScrPtrEv.
namespace G2S { u16 *GetBG1ScrPtr(); }

#define REG16(a) (*(volatile u16 *)(a))
#define REG32(a) (*(volatile u32 *)(a))
#define TOUCHP(off) ((u8 *)(gTouchHeld + (off)))

#pragma defer_codegen off

// @symbol _ZN10dScStage_cD1Ev, _ZN10dScStage_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Destroy through dScStage_c's own three fields -- dBgW_Kc at 0x91c, Model at
 * 0x86c, Particle::SysTracker at 0x50 -- in reverse declaration order, then
 * dScene_c (now inline-defined in dScene_c.h, so its vptr store and dBase_c's
 * inline in turn), then fBase_c's subobject destructor, called rather than
 * inlined because fBase_c's is not. The deleting variant destroys the same
 * chain and then hands the object back through dScene_c's inline operator
 * delete -- declared there rather than on dBase_c, because mwcc only inlines
 * it when found on the class itself or its IMMEDIATE base, which for
 * dScStage_c is dScene_c. One definition emits both; the per-function shards
 * carried the pair as two empty definitions. */
dScStage_c::~dScStage_c()
{
}

// @symbol _ZN10dScStage_c9VE_UpdateEv
void dScStage_c::VE_Update()
{
    u8 t = data_0209f244;
    u8 t2;
    u8 s;
    if (t != 0) {
        data_0209f244 = t - data_0208ee44;
        if (data_0209f244 == 0) {
            dScStage_c::UpdateMenuButtons(0);
        }
    }
    t2 = data_0209f22c;
    if (t2 != 0) {
        data_0209f22c = t2 - data_0208ee44;
        return;
    }
    s = data_0209f290;
    switch (s) {
    case 0: {
        u8 idx = gActivePlayerSlot;
        int ok = 0;
        int off = idx * 4;
        u8 idx2;
        int off2;
        u8 a;
        if (gTouchHeld[idx * 4]) {
            if (gTouchEdge[off]) ok = 1;
        }
        if (ok == 0) {
            if (IsButtonInputValid() == 0) return;
        }
        idx2 = gActivePlayerSlot;
        a = gTouchX[idx2 * 4];
        off2 = idx2 * 4;
        if (((u8)(a - 8) < 0xf0 && (u8)(gTouchY[off2] - 0x38) < 0x20)
            || IsButtonInputValid() != 0) {
            if (data_0209f2e0 == 0) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 0;
            data_0209f22c = data_0208ee44 << 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9a);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0) return;
            return;
        } else {
            u8 idx3 = gActivePlayerSlot;
            int off3 = idx3 * 4;
            if ((u8)(gTouchX[idx3 * 4] - 8) >= 0xf0) return;
            if ((u8)(gTouchY[off3] - 0x68) >= 0x20) return;
            if (data_0209f2e0 == 1) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9b);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0) return;
            return;
        }
    }
    case 1: {
        if (data_0209f2e0 == 0) {
            dScene_c::StartSceneFade(6, 1, 0);
        } else {
            dScene_c::StartSceneFade(1, 0, 0);
        }
        data_0209f290 = 2;
        data_0209d45c &= ~4;
        data_0209d454 &= ~3;
        *(u16 *)0x04000050 = 0;
        *(u16 *)0x04001050 = 0;
        break;
    }
    }
}
// @symbol _ZN10dScStage_c7VE_InitEv
void dScStage_c::VE_Init()
{
    unsigned int a;
    unsigned int b;

    a = data_0209d45c ^ 0x10;
    b = data_0209d454 ^ 0x10;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x1100;
    *(volatile unsigned short *)0x400000c = (*(volatile unsigned short *)0x400000c & 0x43) | 0x1010;
    _ZN7Message17DisplayVsExitTextEt(0x1f);
    data_0209f360[0] = 0xe0;
    data_0209f360[1] = 0x1a0;
    data_0209f2b4 = 2;
    data_0209f2e0 = 0;
    dScStage_c::UpdateMenuButtons(true);
    SetBg2Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    data_0209d45c |= 4;
    data_0209d454 |= 3;
    data_0209b454 |= 0x40000000;
    data_0209b464 = data_0209b454;
    data_0209f294 = 1;
    data_0209f290 = 0;
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4000050, a | 0x20, -7);
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4001050, b | 0x20, -7);
}
// @symbol _ZN10dScStage_c20RenderBouncingArrowsEv
void dScStage_c::RenderBouncingArrows() {
    int r4;
    unsigned char A;
    if (data_0208ee44 == 1) {
        r4 = (data_020a0db0 & 0x10) ? 0xae : 0xb0;
    } else {
        r4 = (data_020a0db0 & 8) ? 0xae : 0xb0;
    }
    A = data_0209f2c4;
    if (A == 0 && data_0209f284 != 0) {
        int t = (data_0209f2d8 == 1);
        if (t == 0) goto draw1;
    }
    if (A == 0) goto draw2;
    {
        unsigned char d = data_0209f248;
        if ((unsigned char)(d + 0xf7) > 2u) goto draw2;
    }
draw1:
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)_ZN3OAM14BOUNCING_ARROWE, 0x40, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)_ZN3OAM14BOUNCING_ARROWE, 0x80, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)_ZN3OAM14BOUNCING_ARROWE, 0xc0, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    return;
draw2:
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)_ZN3OAM14BOUNCING_ARROWE, 0xc, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)_ZN3OAM14BOUNCING_ARROWE, 0xf4, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    return;
}
// @symbol _ZN10dScStage_c9LC_RenderEv
void dScStage_c::LC_Render() {
    data_0209f2a8 = data_0209f2a8 + data_0208ee44;
    if (data_0209f2a8 >= 0xc)
        data_0209f2a8 = 0;

    if (data_0209f20c == 0)
        return;

    {
        void* p = (void*)LoadFile(0x25a);
        _ZN2GX11LoadOBJPlttEPKvjj((void*)((char*)p + (((unsigned int)data_0209f2a8 >> 2) << 5)), 0x1c0, 0x10);
        Deallocate(p);
    }

    if (SublevelToLevel(data_02092124) >= 0xf && SublevelToLevel(data_02092124) < 0x15) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cea8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cee8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf28, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf78, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cb14, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
    }

    if (data_0209f2b0 != 0) {
        if (data_0209f20c != 0 && SublevelToLevel(data_02092124) >= 0xf) {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210caf4, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210caf4, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (data_0209f2d4 == 3)
        RenderBouncingArrows();
}
// @symbol _ZN10dScStage_c9LC_UpdateEv
void dScStage_c::LC_Update()
{
    if (data_0209f244 != 0) {
        data_0209f244 = data_0209f244 - data_0208ee44;
        if (data_0209f244 == 0)
            dScStage_c::UpdateMenuButtons(0);
    }
    if (data_0209f22c != 0) {
        data_0209f22c = data_0209f22c - data_0208ee44;
        return;
    }

    switch (data_0209f2d4) {
    case 0:
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        _ZN7Message21DisplayLevelClearTextEta(0x27a, SublevelToLevel(data_02092124));
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2b0 = 0;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetBg3Offset(0, 0);
        data_0209d45c |= 8;
        data_0209f260 = 0;
        data_0209f2d4 = 1;
        data_0209f280 = 0;
        if (data_0209f358[0] != 0)
            return;
        if (data_0209f2ac != 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        data_0209f22c = 0x1e;
        return;
    case 1: {
        int level = SublevelToLevel(data_02092124);
        if (data_0209f358[0] > data_0209f260) {
            data_0209f260 += 1;
            if (data_0209f260 % 50 == 0) {
                GiveLives(1);
                _ZN5Sound12PlayBank3_2DEj(0x6e);
            }
            if (data_0209f358[0] == data_0209f260) {
                if (data_0209f2ac != 0) {
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
                    data_0209f280 = 1;
                }
                data_0209f22c = 0x1e;
                if (level < 0xf) {
                    u8 coins = data_0209f260;
                    if (_ZN8SaveData13GetCoinRecordEj(level) >= coins)
                        return;
                    _ZN8SaveData21SetCoinRecordIfHigherEah(level, coins);
                    data_0209f2b0 = 1;
                    func_02012790(0x22);
                    return;
                }
                if (level < 0xf)
                    return;
                if (level >= 0x15)
                    return;
                func_02012790(0x22);
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
                data_0209f280 = 1;
                return;
            }
            func_02012790(0x15);
            return;
        }
        if (data_0209f260 == 0 && level >= 0xf && level < 0x15) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        data_0209f22c = 0x3c;
        data_0209f358[0] = 0;
        data_0209f2d4 = 2;
        return;
    }
    case 2: {
        int t6;
        int t5;
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1210;
        } else {
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        }
        data_0209f358[0] = 0;
        t6 = data_0209d45c ^ 0x18;
        t5 = data_0209d454 ^ 0x10;
        SetBg2Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d454 |= 3;
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4000050, t6 | 0x20, -7);
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4001050, t5 | 0x20, -7);
        data_0209f2d4 = 3;
        return;
    }
    case 3: {
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x28) < 0x20)
                goto sel3_0;
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel3_0;
            goto chk3_1;
        sel3_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            func_02012790(0x57);
            data_0209f2d4 = 4;
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_1:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x50) < 0x20)
                goto sel3_1;
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel3_1;
            goto chk3_2;
        sel3_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            func_02012790(0x58);
            data_0209f2d4 = 4;
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_2:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x78) < 0x20)
                goto sel3_2;
            if (data_0209f2e0 != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel3_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            func_02012790(0x59);
            data_0209f2d4 = 6;
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 4:
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        data_0209f2d4 = 5;
        data_0209d45c &= ~4;
        _ZN7Message21DisplaySaveStatusTextEt(0x295);
        data_0209f2b8 = 0x78;
        return;
    case 5:
        if (data_0209f2b8 == 0)
            return;
        if (data_0209f2b8 == 0x78) {
            _ZN8SaveData15SaveCurrentFileEv();
        } else if (data_0209f2b8 == 0x3c) {
            _ZN7Message21DisplaySaveStatusTextEt(0x296);
        }
        data_0209f2b8 = data_0209f2b8 - data_0208ee44;
        if (data_0209f2b8 == 0)
            data_0209f2d4 = 6;
        return;
    case 6:
        if (data_0209f280 != 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x7f, 0, 0x15666, 0);
            data_0209f280 = 0;
        }
        data_0209d45c &= ~0xc;
        data_0209d454 &= ~3;
        REG16(0x4000050) = 0;
        if (data_0209f2e0 != 1) {
            data_ov002_02111178 = 5;
        } else {
            _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
            data_ov002_02111178 = 0;
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
            {
                int t = data_0209b454 | 0x40000000;
                data_0209b454 = t;
                data_0209b464 = t;
            }
        }
        data_0209f2e0 = 0;
        data_0209f2d4 = 0;
        data_0209f2b0 = 0;
        data_0209f20c = 0;
        return;
    default:
        return;
    }
}
// @symbol _ZN10dScStage_c16CheckCameraInputEv
#pragma opt_strength_reduction off
void dScStage_c::CheckCameraInput()
{
    CamInput* cam = (CamInput *)data_0209f498;
    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && data_0209d660 == 0) {
        int i = 0;
        int n1 = data_0209f21c;
        if (n1 > 0) {
        u8* st = data_0209f350;
        do {
            cam->pressed = 0;
            cam->held = 0;
            if (((TouchInfo *)gTouchHeld)[i].held != 0) {
                if (*(int *)((u8 *)data_0209f318 + 0x154) & 0x1000) {
                    u8 state = *((u8*)data_0209f498 + i * 0x18 + 0x16);
                    if ((state == 0 && ((((TouchInfo *)gTouchHeld)[i].x <= 0x58 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8a) || (((TouchInfo *)gTouchHeld)[i].x >= 0xa7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8a)))
                        || (state == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xa7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x9a)) {
                        if (((((TouchInfo *)gTouchHeld)[i].held && ((TouchInfo *)gTouchHeld)[i].edge) ? 1 : 0) || *st == 1) {
                            u16 mask;
                            if ((state == 0 && ((TouchInfo *)gTouchHeld)[i].x < 0x2b) || (((TouchInfo *)gTouchHeld)[i].x >= 0xa7 && ((TouchInfo *)gTouchHeld)[i].x < 0xd1))
                                mask = 0x200;
                            else if ((state == 0 && ((TouchInfo *)gTouchHeld)[i].x >= 0x30 && ((TouchInfo *)gTouchHeld)[i].x <= 0x58) || ((TouchInfo *)gTouchHeld)[i].x >= 0xd6)
                                mask = 0x100;
                            else {
                                mask = data_0209f368[i];
                                if (mask == 0) mask = 0x200;
                            }
                            if ((((TouchInfo *)gTouchHeld)[i].held && ((TouchInfo *)gTouchHeld)[i].edge) ? 1 : 0)
                                *(u16*)(((int)cam + 6)) |= mask;
                            else
                                *(u16*)(((int)cam + 6)) |= mask & (mask ^ data_0209f368[i]);
                            data_0209f368[i] = mask;
                            *(u16*)(((int)cam + 4)) |= mask;
                            *st = 1;
                        }
                    } else if (*st == 1) {
                        *st = 0;
                        data_0209f368[i] = 0;
                    }
                }
            } else {
                if (*st == 1) *st = 0;
                data_0209f368[i] = 0;
            }
            {
                u8 state2 = *((u8*)data_0209f498 + i * 0x18 + 0x16);
                if (state2 != 0) {
                    if (((*((u8*)gTouchHeld + i * 4) && ((TouchInfo *)gTouchHeld)[i].edge) ? 1 : 0)
                        && ((state2 == 1 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8d)
                            || (state2 == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x73 && ((TouchInfo *)gTouchHeld)[i].y < 0x97))) {
                        data_ov002_02111180 = 0x10;
                        *(u16*)(((int)cam + 6)) |= 0x8000;
                        *st = 2;
                        cam->f14 = 0;
                    } else if (*st == 2) {
                        if ((state2 == 1 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd5 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8d)
                            || (state2 == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd5 && ((TouchInfo *)gTouchHeld)[i].y >= 0x73 && ((TouchInfo *)gTouchHeld)[i].y < 0x97)) {
                            cam->f14 = 0;
                        } else {
                            *st = 0;
                        }
                    }
                }
            }
            i++; cam++; st++;
        } while (i < data_0209f21c);
        }
    } else {
        int i = 0;
        u8* st;
        int n = data_0209f21c;
        if (n > 0) {
        st = data_0209f350;
        do {
            cam->pressed = 0;
            cam->held = 0;
            *st = 0;
            i++; cam++; st++;
        } while (i < n);
        }
    }
}
#pragma opt_strength_reduction on
// @symbol _ZN10dScStage_c12RenderNumberEhiibi
void dScStage_c::RenderNumber(unsigned char num, int x, int y, bool b, int p5)
{
    int leading = 0;
    Matrix2x2 *mtx = 0;
    int i;

    for (i = 0; i < 3; i++)
    {
        int digit = __aeabi_idiv(num, data_020755b8[i]);
        if (digit != 0 || leading != 0 || i == 2)
        {
            if (b == 0)
            {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(true, _ZN3OAM7NUMBERSE[digit], x, y, p5, -1, mtx);
            }
            else
            {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(false, _ZN3OAM7NUMBERSE[digit], x, y, p5, -1, 0);
            }
            leading = 1;
            x += 9;
        }
        num = num % data_020755b8[i];
    }
}
// @symbol _ZN10dScStage_c9PS_RenderEv
void dScStage_c::PS_Render()
{
    if (func_02029408() != 0 && data_02092144[data_0209f250] != 0 &&
        data_0209f248 != 0xb && data_0209f248 != 0xe && data_0209f248 != 0xf &&
        ((unsigned int)data_0209f248 < 7U || (unsigned int)data_0209f248 > 8U)) {
        dScStage_c::RenderBouncingArrows();
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
                        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, var_sb, 0x60, -1, -1, 0);
                        var_r7 = (var_r7 + 1) & 0xff;
                    } else {
                        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (OamAttr *)&_ZN3OAM16SMALL_STAR_EMPTYE, var_sb, 0x60, -1, -1, 0);
                        sp18 = 1;
                    }
                    var_sb += 0x10;
                    var_sl += 1;
                } while (var_r7 != var_r8);
            }
            if (sp18 == 0 && var_r8 != 7) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (OamAttr *)&_ZN3OAM16SMALL_STAR_EMPTYE, var_sb, 0x60, -1, -1, 0);
            }
            unsigned char r5v = _ZN8SaveData13GetCoinRecordEj(data_0209f2c8);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM4COINE, 0x70, 0x70, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x80, 0x78, -1, -1, 0);
            RenderNumber(r5v, 0x88, 0x70, 1, 8);
            if (IsStarCollected(data_0209f2c8, 0) != 0) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, 0xb0, 0x78, -1, -1, 0);
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, 0x70, 0x60, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x80, 0x60, -1, -1, 0);
            RenderNumber(CountStarsCollectedInLevelToDisplay(0x1d), 0x88, 0x58, 1, 8);
        }
    } else if (data_0209d45c & 8) {
        if ((unsigned int)data_0209f2c8 < 0xfU) {
            unsigned char r4v = CountStarsCollectedInLevelToDisplay(data_0209f2c8);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, 0x58, 0x8c, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x68, 0x8c, -1, -1, 0);
            RenderNumber(r4v, 0x70, 0x84, 1, 1);
            unsigned char r4v2 = _ZN8SaveData13GetCoinRecordEj(data_0209f2c8);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM4COINE, 0x88, 0x84, -1, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, 0x98, 0x8c, -1, -1, 0);
            RenderNumber(r4v2, 0xa0, 0x84, 1, 1);
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
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x46, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                } else if (GetOwnerLanguage() == 3) {
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x4e, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                } else {
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x56, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                }
            } else {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0xce, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x26, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
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
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x46, 0xa8, -1, -1, 0);
            } else if (GetOwnerLanguage() == 3) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x4e, 0xa8, -1, -1, 0);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x56, 0xa8, -1, -1, 0);
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0xce, 0xa8, -1, -1, 0);
        }
    } else {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM9TINY_STARE, 0xe, ((data_0209f2e0 * 6) + 7) * 8, -1, -1, 0);
    }

    if (data_0209f238 == 2) {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM25SMALL_ARROW_POINTING_LEFTE, 0x62, 0x38, -1, -1, 0);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM25SMALL_ARROW_POINTING_LEFTE, 0x64, 0x38, -1, -1, 0);
    }

    if (data_0209f238 == 1) {
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM26SMALL_ARROW_POINTING_RIGHTE, 0xf6, 0x38, -1, -1, 0);
        return;
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, (OamAttr *)&_ZN3OAM26SMALL_ARROW_POINTING_RIGHTE, 0xf4, 0x38, -1, -1, 0);
}
// @symbol _ZN10dScStage_c25PS_UpdateOkAndBackButtonsEb
/* Lever (notes 6au): TWO coupled changes that only work together.
 *   1. the array object is declared `extern volatile int data_02075610[];`
 *   2. there is NO named pointer local -- the array is indexed INLINE in the loop bound.
 * A named pointer local puts the base in mwcc's address-constant class, which is colored
 * after every normal web and lands it in r7; that alone was the whole 9-word miss, and it
 * dominates even if the pointer is itself volatile. A plain inline access rematerializes
 * the base in normal birth order but into a scratch register and caches the bound, which
 * drops the tail reload and comes out three words SHORT. The volatile forces the bound to
 * be re-read every iteration (the ROM reloads at +0xc0 and +0x114), which keeps the base
 * live across the inner loop so it must take a callee-saved register -- but via the
 * normal-birth path, not the address-constant class. Result ptr r5 / 0x1000 r6 / idx r7,
 * exactly the ROM. Do NOT reintroduce a named pointer and do NOT drop the volatile;
 * either one alone regresses. Link-verified: VERIFIED, 0 diffs, 0 blind.
 */
void dScStage_c::PS_UpdateOkAndBackButtons(bool b)
{
    int sl;
    int i;
    int base;
    int data_0x1000;
    int idx;
    int data_0x2000;
    int j;

    if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 3)
        base = 2;
    else
        base = 0;

    for (i = 0; i < 2; i++) {
        data_0x2000 = 0x2000;
        data_0x1000 = 0x1000;

        if (b) {
            sl = data_0x1000;
        } else {
            int v;
            if (data_0209f2b4 == data_0209f2e0 && data_0209f29c == i && data_0209f244 == 0)
                v = data_0x2000;
            else
                v = data_0x1000;
            sl = (u16)v;
        }

        idx = i + base;
        u16 *scr = G2S::GetBG1ScrPtr() + data_020755cc[idx];
        for (j = 0; j < data_02075610[idx]; j++) {
            scr[0] = (scr[0] & 0x3ff) | sl;
            scr[0x20] = (scr[0x20] & 0x3ff) | sl;
            scr[0x40] = (scr[0x40] & 0x3ff) | sl;
            scr[0x60] = (scr[0x60] & 0x3ff) | sl;
            scr++;
        }
    }
}
// @symbol _ZN10dScStage_c17PS_UpdateSaveMenuEb
void dScStage_c::PS_UpdateSaveMenu(bool b)
{
  u16 v;
  int i;
  for (i = 0; i < 2; i++)
  {
    int j;
    u16 *p;
    if (b != 0)
    {
      v = 0x1000;
    }
    else
    {
      unsigned int t;
      if (data_0209f2e0 == i && data_0209f244 == 0) t = 0x2000;
      else t = 0x1000;
      v = (u16) t;
    }
    p = G2S::GetBG1ScrPtr() + data_020755c8[i];
    for (j = 0; j < 10; j++)
    {
      p[0] = (p[0] & 0x3ff) | v;
      p[0x20] = (p[0x20] & 0x3ff) | v;
      p[0x40] = (p[0x40] & 0x3ff) | v;
      p[0x60] = (p[0x60] & 0x3ff) | v;
      p++;
    }
  }
}
// @symbol _ZN10dScStage_c20PS_UpdateOptionsMenuEv
void dScStage_c::PS_UpdateOptionsMenu()
{
    int i;
    int j;
    int counter;
    u16 sel;
    u16 *scr;
    u16 v;

    sel = (data_0209f2e4 != 0) ? 0x5000 : 0x6000;
    scr = G2S::GetBG1ScrPtr() + 0xae;
    for (j = 0; j < 0xf; j++) {
        scr[0]    = (scr[0]    & 0x3ff) | sel;
        scr[0x20] = (scr[0x20] & 0x3ff) | sel;
        scr[0x40] = (scr[0x40] & 0x3ff) | sel;
        scr[0x60] = (scr[0x60] & 0x3ff) | sel;
        scr++;
    }

    for (j = 0; j < 2; j++) {
        scr = G2S::GetBG1ScrPtr() + data_020755c0[j];
        v = (data_0209f2e0 == j) ? 0x4000 : 0x3000;
        counter = 0;
        for (; counter < 0xa; counter++) {
            scr[0]    = (scr[0]    & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr++;
        }
    }

    for (i = 0; i < 2; i++) {
        scr = G2S::GetBG1ScrPtr() + data_020755c4[i];
        v = (data_0209f2ec == i && data_0209f2cc == 0) ? 0x6000 : 0x5000;
        counter = 0;
        for (; counter < 7; counter++) {
            scr[0]    = (scr[0]    & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr[0x40] = (scr[0x40] & 0x3ff) | v;
            scr[0x60] = (scr[0x60] & 0x3ff) | v;
            scr++;
        }
    }
}
// @symbol _ZN10dScStage_c17UpdateMenuButtonsEb
    void dScStage_c::UpdateMenuButtons(bool b)
    {
      int i;
      for (i = 0; i < data_0209f2b4; i++)
      {
        unsigned int v;
        int j;
        u16 *p;
        p = G2S::GetBG1ScrPtr() + data_0209f360[i];
        if (b == 0)
        {
          if (data_0209f2e0 == i && data_0209f244 == 0)
          {
            v = 0x2000;
          }
          else
          {
            v = 0x1000;
          }
          v = (u16) v;
        }
        else
        {
          v = 0x1000;
        }
        if (data_0209f2c4 != 0)
        {
          u8 t = (u8) (data_0209f248 + 0xf9);
          if (t <= 1)
          {
            v = (u16) (v + 0x4000);
          }
        }
        for (j = 0; j < 0x20; j++)
        {
          p[0] = (p[0] & 0x3ff) | v;
          p[0x20] = (p[0x20] & 0x3ff) | v;
          p[0x40] = (p[0x40] & 0x3ff) | v;
          p[0x60] = (p[0x60] & 0x3ff) | v;
          p++;
        }

      }

    }
// @symbol _ZN10dScStage_c10PS_CleanupEv
/* Static, like the rest of the PS_ group: restarts the timer, unpauses or
 * stops the music and clears the blend registers, all through globals. */
void dScStage_c::PS_Cleanup(){
  if(data_0209f2a0[0]){
    data_0209d4c8.StartTimer();
    data_0209f2a0[0]=0;
  }
  data_0209d45c &= ~0xe;
  data_0209d454 &= ~3;
  if(data_0209f280==0) Sound::UnpauseMusic();
  else Sound::StopLoadedMusic_Layer1(0x3c);
  data_0209f300=0xf;
  *(short*)0x4000050=0;
  *(short*)0x4001050=0;
  data_0209f280=0;
  data_0209f2c4=0;
}
// @symbol _ZN10dScStage_c9PS_UpdateEv
// Cases 0/2/3/8/0xa..0x13 BYTE-EXACT.
/*
 * dScStage_c::PS_Update @ 0x0202635c size 0x30ac, mwccarm 1.2/sp2p3 -O4,p -lang c++
 *
 * 2026-07-10 refine (69 -> 2):
 *   1) sel3_0 stored data_0209f1ec = 2 but ROM stores data_0209f2c4 = 2 (pool
 *      slot 0x9a8). Semantic fix, not regperm: match.py wildcards reloc slots,
 *      so only the ldr displacement field betrayed it (+0xc48).
 *   2) backlight_on guard reads the STALE tmpv (= tx - 0x6e, left from the
 *      snd_next chain guard), NOT t6. Writing "tmpv = tx - 0x6e;
 *      if ((u8)tmpv < 0x7c)" at the chain guard and "(u8)tmpv < 0x3c" at
 *      backlight_on extends tmpv's live range r0-shaped and flipped the whole
 *      case-0xa tx/ty cascade (tx=r3, ty reloads=r2) in one move: 68 -> 3.
 *   3) opt_okback first gTouchY check reads gTouchY[gActivePlayerSlot * 4]
 *      DIRECTLY (fresh gActivePlayerSlot load indexes the ldrb, +0x26e4); the old
 *      "u8 s4 = gActivePlayerSlot; (void)s4" kept slot/r6 as index. With (2) in place the
 *      direct read no longer drops slot's r6 coloring.
 *
 * Final match: moving the f238 store into the following unconditional scope,
 *   after the displayed-level normalization, makes MWCC schedule the case-1
 *   literal loads in ROM order without changing behavior.
 */
#pragma opt_common_subs off
void dScStage_c::PS_Update()
{
    register u8 var_sl;
    register u8 var_fp;
    s32 sp0;
    s32 sp4;
    s32 sp8;
    s32 spC;
    s32 sp10;
    s32 var_r0;

    if (data_0209f210 != 0) {
        data_0209f210 = data_0209f210 - data_0208ee44;
        if (data_0209f210 == 0)
            data_0209f238 = 0;
    }
    if (data_0209f2e4 != 0) {
        data_0209f2e4 = data_0209f2e4 - data_0208ee44;
        if (data_0209f2e4 == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f244 != 0) {
        data_0209f244 = data_0209f244 - data_0208ee44;
        if (data_0209f244 == 0) {
            u8 st = data_0209f248;
            if (st == 8 || st == 0xa) {
                if (data_0209f240 != 0) {
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                } else if (st == 8) {
                    dScStage_c::UpdateMenuButtons(0);
                }
            } else if ((u8)(st + 0xef) <= 1) {
                dScStage_c::PS_UpdateSaveMenu(0);
            } else {
                dScStage_c::UpdateMenuButtons(0);
            }
        }
    }
    if (data_0209f2cc != 0) {
        data_0209f2cc = data_0209f2cc - data_0208ee44;
        if (data_0209f2cc == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f23c != 0) {
        data_0209f23c = data_0209f23c - data_0208ee44;
        if (data_0209f23c == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f22c != 0) {
        data_0209f22c = data_0209f22c - data_0208ee44;
        return;
    }

    {
        u8 nxt = data_0209f1ec;
        u8 cur = data_0209f248;
        if (cur != nxt)
            data_0209f248 = nxt;
    }

    switch (data_0209f248) {
    case 0: {
        u8 i;
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000a) = (REG16(0x400000a) & 0x43) | 0x1210;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        i = 0;
        do {
            if (CountStarsCollectedInLevelToDisplay(i) != 0)
                break;
            i = (u8)(i + 1);
        } while (i < 0xf);
        if (i == 0xf) {
            data_0209f2c8 = i;
            data_0209f2f0 = 1;
        } else {
            if (SublevelToLevel(data_02092124) >= 0xf) {
                data_0209f2c8 = 0xf;
            } else {
                data_0209f2c8 = SublevelToLevel(data_02092124);
            }
            data_0209f2f0 = 0;
        }
        _ZN7Message7DisplayEj(data_020756d0[data_0209f2c8]);
        data_0209f360[0] = 0x80;
        data_0209f360[1] = 0x120;
        data_0209f360[2] = 0x1c0;
        data_0209f360[3] = 0x260;
        data_0209f2b4 = 4;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        SetBg1Offset(0, 0);
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d45c |= 0xe;
        data_0209d454 |= 3;
        data_0209f1ec = 1;
        return;
    }
    case 1: {
        if (data_0209f300 != 0)
            return;
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x20) < 0x20)
                goto sel1_0;
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel1_0;
            goto chk1_1;
        sel1_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            data_0209f22c = data_0208ee44 << 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f2c4 = 2;
            func_02012790(3);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                return;
            return;
        }
    chk1_1:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x48) < 0x20)
                goto sel1_1;
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel1_1;
            goto chk1_2;
        sel1_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 7;
            func_02012790(0x53);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk1_2:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x70) < 0x20)
                goto sel1_2;
            if (data_0209f2e0 == 2 && IsButtonInputValid() != 0)
                goto sel1_2;
            goto chk1_3;
        sel1_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 9;
            func_02012790(0x54);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk1_3:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x98) < 0x20)
                goto sel1_3;
            if (data_0209f2e0 == 3 && IsButtonInputValid() != 0)
                goto sel1_3;
            goto chk1_arrows;
        sel1_3:
            if (data_0209f2e0 == 3)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 3;
            data_0209f22c = data_0208ee44 << 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f1ec = 0xc;
            func_02012790(0x55);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                return;
            return;
        }
    chk1_arrows:
        if (data_0209f2f0 != 0)
            return;
        {
            register u8 *de8_sb;
            var_sl = 0;
            var_fp = var_sl;
            (void)(u32)&data_0209f238;
            (void)(u32)&gTouchHeld;
            (void)(u32)&data_0209f2c8;
            sp4 = var_sl;
            sp8 = var_sl;
            sp0 = var_sl;
            sp10 = 0x52;
            spC = 0x51;
            do {
                s32 de_off;
                u8 sl2;
                u8 a;
                u8 vx;
                {
                    volatile u8 *pSlot = &gActivePlayerSlot;
                    volatile s32 *psp = &sp0;
                    var_r0 = *psp;
                    sl2 = *pSlot;
                    {
                        register u8 aa = gTouchHeld[sl2 * 4];
                        a = aa;
                    }
                    de_off = sl2 * 4;
                }
                if ((a != 0) && (TOUCHP(de_off)[1] != 0)) {
                    var_r0 = 1;
                }
                if ((var_r0 != 0) && ((u32)(vx = TOUCHP(sl2 * 4)[2]) < 0x38U) && ((u32)TOUCHP(sl2 * 4)[3] < 0x20U)) {
                    u8 t;
                    data_0209f2c8 = (u8)(data_0209f2c8 - 1);
                    var_fp = 1;
                    t = data_0209f2c8;
                    if (t == 0xFF)
                        data_0209f2c8 = 0xF;
                    {
                        u8 *p0 = &data_0209f210;
                        u8 *p1 = &data_0209f210;
                        u8 *p = ((u8)(vx & 0xff) < 0x38) ? p0 : p1;
                        data_0209f238 = 1;
                        *p = (u8)(data_0208ee44 * 3);
                    }
                } else {
                    s32 var_r0_2;
                    if ((a != 0) && (TOUCHP(sl2 * 4)[1] != 0)) {
                        var_r0_2 = 1;
                    } else {
                        var_r0_2 = sp4;
                    }
                    if (var_r0_2 != 0) {
                        if (((u32)(u8)(TOUCHP(sl2 * 4)[2] - 0xC8) < 0x38U) && ((u32)TOUCHP(sl2 * 4)[3] < 0x20U)) {
                            data_0209f2c8 = (u8)(data_0209f2c8 + 1);
                            data_0209f238 = 2;
                            var_fp = 1;
                            if (data_0209f2c8 == 0x10) {
                                data_0209f2c8 = (u8)sp8;
                            }
                            data_0209f210 = (u8)(data_0208ee44 * 3);
                        }
                    }
                }
                if ((data_0209f2c8 == 0xF) || (CountStarsCollectedInLevelToDisplay(data_0209f2c8) != 0)) {
                    var_sl = 1;
                    if (data_0209f238 == 1) {
                        func_02012790(spC);
                    } else if (data_0209f238 == 2) {
                        func_02012790(sp10);
                    }
                }
            } while (var_sl == 0);
        }
        if (var_fp == 0)
            return;
        _ZN7Message11DisplayTextEt(data_020756d0[data_0209f2c8]);
        return;
    }
    case 2: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        data_0209f1ec = 3;
        data_0209f2c8 = SublevelToLevel(data_0209f2f8);
        _ZN7Message16DisplayPauseTextEth(0x277, data_0209f2c8);
        data_0209f360[0] = 0x60;
        data_0209f360[1] = 0x100;
        data_0209f360[2] = 0x1a0;
        data_0209f360[3] = 0x240;
        data_0209f2b4 = 4;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetBg3Offset(0, 0);
        data_0209d45c |= 8;
        if (func_02029408() != 0 && data_02092144[data_0209f250] != 0) {
            SetBg2Offset(0, 0);
            SetSubBg0Offset(0, 0);
            SetSubBg1Offset(0, 0);
            data_0209d45c |= 4;
            data_0209d454 |= 3;
            return;
        }
        data_0209f1ec = 0xb;
        return;
    }
    case 3: {
        if (data_0209f300 != 0)
            return;
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x18) < 0x20)
                goto sel3_0;
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel3_0;
            goto chk3_1;
        sel3_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f2c4 = 2;
            func_02012790(3);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_1:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x40) < 0x20)
                goto sel3_1;
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel3_1;
            goto chk3_2;
        sel3_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 7;
            func_02012790(0x53);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_2:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x68) < 0x20)
                goto sel3_2;
            if (data_0209f2e0 == 2 && IsButtonInputValid() != 0)
                goto sel3_2;
            goto chk3_3;
        sel3_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 9;
            func_02012790(0x54);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk3_3:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x90) < 0x20)
                goto sel3_3;
            if (data_0209f2e0 != 3)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel3_3:
            if (data_0209f2e0 == 3)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x13;
            func_02012790(0x56);
            return;
        }
    }
    case 4: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        data_0209f1ec = 5;
        _ZN7Message18DisplayPauseTextVSEt(0);
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        if (func_02029408() == 0) {
            data_0209f1ec = 0xb;
            return;
        }
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d45c &= ~8;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d454 |= 3;
        return;
    }
    case 5: {
        if (data_0209f300 != 0)
            return;
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x28) < 0x20)
                goto sel5_0;
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto sel5_0;
            goto chk5_1;
        sel5_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f2c4 = 2;
            func_02012790(3);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chk5_1:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x50) < 0x20)
                goto sel5_1;
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto sel5_1;
            goto chk5_2;
        sel5_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x13;
            data_ov002_02111150 = 1;
            func_02012790(0x56);
            return;
        }
    chk5_2:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x78) < 0x20)
                goto sel5_2;
            if (data_0209f2e0 != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        sel5_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 6;
            data_ov002_02111150 = 1;
            func_02012790(0x55);
            return;
        }
    }
    case 6: {
        _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        return;
    }
    case 7: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xf00;
        if (data_0209f218 != 0) {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb001, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac01, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa801, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa401, G2S::GetBG1ScrPtr());
            } else {
                LoadCompressedFileAt(0xa001, G2S::GetBG1ScrPtr());
            }
        } else {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb009, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac09, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa809, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa409, G2S::GetBG1ScrPtr());
            } else {
                LoadCompressedFileAt(0xa009, G2S::GetBG1ScrPtr());
            }
        }
        data_0209f360[0] = 0x80;
        data_0209f360[1] = 0x120;
        data_0209f360[2] = 0x1c0;
        data_0209f2b4 = 3;
        data_0209f1ec = 8;
        {
            u8 cur = data_0209f4ae[gActivePlayerSlot * 0x18];
            data_0209f2dc = cur;
            data_0209f2e0 = cur;
        }
        dScStage_c::UpdateMenuButtons(0);
        _ZN7Message25DisplayControllerModeTextEt(0x280);
        REG16(0x400000c) = (((data_0209f2dc + 0xd) << 8) | (REG16(0x400000c) & 0x43)) | 0x10;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        REG32(0x4001010) = 0;
        REG32(0x4001014) = 0;
        data_0209f29c = 0;
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        SetBg2Offset(0, 0);
        data_0209d45c &= ~2;
        data_0209d45c |= 4;
        data_0209d45c &= ~8;
        data_0209d454 |= 3;
        return;
    }
    case 8: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        int idxa;
        if (data_0209f300 != 0)
            return;
        if (gTouchHeld[gActivePlayerSlot * 4] == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        if (GetOwnerLanguage() == 5) {
            slot = gActivePlayerSlot;
            firstw = 0x64;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x44);
        } else if (GetOwnerLanguage() == 3) {
            slot = gActivePlayerSlot;
            firstw = 0x74;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x3c);
        } else {
            slot = gActivePlayerSlot;
            firstw = 0x54;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x4c);
        }
        idxa = slot * 4;
        ty = gTouchY[slot * 4];
        if ((u8)(ty - 0x1e) < 0x24) {
            if ((u8)(ty - 0x20) >= 0x20)
                return;
            if (data_0209f2a4 == 1) {
                int t = 0;
                if (gTouchHeld[idxa] != 0) {
                    if (gTouchEdge[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 1;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xd10;
            data_0209f244 = data_0208ee44 << 2;
            {
                int t = 1;
                u8 a = gTouchHeld[slot * 4];
                if (a == 0)
                    goto mode0_change;
                if (!(a != 0 && gTouchEdge[slot * 4] != 0))
                    t = 0;
                if (t == 0)
                    goto mode0_keep;
                if (data_0209f2dc != 0)
                    goto mode0_keep;
            mode0_change:
                data_0209f2dc = 0;
                SetControllerMode(0);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x116);
                data_0209f2c4 = 2;
                goto mode0_done;
            mode0_keep:
                data_0209f2dc = 0;
                data_0209f240 = 0;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                func_02012790(0x5b);
            mode0_done:
                data_0209f2e0 = 0;
                dScStage_c::UpdateMenuButtons(0);
                return;
            }
        } else if ((u8)(ty - 0x46) < 0x24) {
            if ((u8)(ty - 0x48) >= 0x20)
                return;
            if (data_0209f2a4 == 2) {
                int t = 0;
                if (gTouchHeld[idxa] != 0) {
                    if (gTouchEdge[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 2;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xe10;
            data_0209f244 = data_0208ee44 << 2;
            {
                int t;
                u8 a = gTouchHeld[slot * 4];
                if (a == 0)
                    goto mode1_change;
                t = (a != 0 && gTouchEdge[slot * 4] != 0);
                if (t == 0)
                    goto mode1_keep;
                if (data_0209f2dc != 1)
                    goto mode1_keep;
            mode1_change:
                data_0209f2dc = 1;
                SetControllerMode(1);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x116);
                data_0209f2c4 = 2;
                goto mode1_done;
            mode1_keep:
                data_0209f2dc = 1;
                data_0209f240 = 0;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                func_02012790(0x5b);
            mode1_done:
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                return;
            }
        } else if ((u8)(ty - 0x6e) < 0x24) {
            if ((u8)(ty - 0x70) >= 0x20)
                return;
            if (data_0209f2a4 == 3) {
                int t = 0;
                if (gTouchHeld[idxa] != 0) {
                    if (gTouchEdge[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 3;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xf10;
            data_0209f244 = data_0208ee44 << 2;
            {
                int t;
                u8 a = gTouchHeld[slot * 4];
                if (a == 0)
                    goto mode2_change;
                t = (a != 0 && gTouchEdge[slot * 4] != 0);
                if (t == 0)
                    goto mode2_keep;
                if (data_0209f2dc != 2)
                    goto mode2_keep;
            mode2_change:
                data_0209f2dc = 2;
                SetControllerMode(2);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x116);
                data_0209f2c4 = 2;
                goto mode2_done;
            mode2_keep:
                data_0209f2dc = 2;
                data_0209f240 = 0;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                func_02012790(0x5b);
            mode2_done:
                data_0209f2e0 = 2;
                dScStage_c::UpdateMenuButtons(0);
                return;
            }
        } else {
            int t = 0;
            u8 a0 = gTouchHeld[idxa];
            if (a0 != 0) {
                if (gTouchEdge[idxa] != 0)
                    t = 1;
            }
            if (t == 0)
                goto chk218;
            if (relx >= firstw)
                goto chk218;
            if ((u8)(ty - 0x98) < 0x20)
                goto okback;
        chk218:
            if (data_0209f218 == 0) {
                int t2;
                t2 = (a0 != 0 && gTouchEdge[slot * 4] != 0);
                if (t2 == 0)
                    goto biv8;
                if ((u8)(tx - 0xd8) >= 0x20)
                    goto biv8;
                if ((u8)(ty - 0x98) < 0x20)
                    goto okback;
            }
        biv8:
            if (IsButtonInputValid() == 0)
                goto fail8;
        okback:
            if (relx < firstw) {
                u8 slot2 = gActivePlayerSlot;
                if ((u8)(gTouchY[slot2 * 4] - 0x98) < 0x20 &&
                    IsButtonInputValid() != 0) {
                    data_0209f29c = 0;
                    goto okback_go;
                }
            }
            {
                u8 slot2 = gActivePlayerSlot;
                if ((u8)(gTouchX[slot2 * 4] - 0xd8) < 0x20 &&
                    (u8)(gTouchY[slot2 * 4] - 0x98) < 0x20) {
                    data_0209f29c = 1;
                }
            }
        okback_go:
            data_0209f2e0 = 3;
            SetControllerMode(data_0209f2dc);
            data_0209f240 = 1;
            data_0209f244 = data_0208ee44 << 2;
            dScStage_c::PS_UpdateOkAndBackButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            if (data_0209f29c == 0) {
                data_0209f2c4 = 2;
                func_02012790(0x116);
                return;
            }
            func_02012790(0x5c);
            if (SublevelToLevel(data_0209f2f8) == 0x1d) {
                data_0209f1ec = 0;
            } else {
                data_0209f1ec = 2;
            }
            return;
        fail8:
            data_0209f2a4 = 0;
            return;
        }
    }
    case 9: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1000;
        data_0209f1ec = 0xa;
        data_0209f234 = GetSoundMode();
        data_0209f2ec = data_0208ee3c ^ 1;
        data_0209f1fc = data_0208ee40;
        data_0209f238 = 0;
        _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
        data_0209f2b4 = 2;
        data_0209f2e0 = 0;
        dScStage_c::PS_UpdateOptionsMenu();
        data_0209f29c = 0;
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            data_0209d45c &= ~2;
        } else {
            data_0209d45c &= ~4;
        }
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        return;
    }
    case 0xa: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        if (gTouchHeld[gActivePlayerSlot * 4] == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        if (GetOwnerLanguage() == 5) {
            slot = gActivePlayerSlot;
            firstw = 0x70;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x40);
        } else if (GetOwnerLanguage() == 3) {
            slot = gActivePlayerSlot;
            firstw = 0x60;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x48);
        } else {
            slot = gActivePlayerSlot;
            firstw = 0x50;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x50);
        }
        if ((u8)(tx - 0x5a) < 0x14) {
            ty = gTouchY[slot * 4];
            if ((u8)(ty - 0x2e) < 0x14) {
                if ((u8)(tx - 0x5c) >= 0x10)
                    return;
                if ((u8)(ty - 0x30) >= 0x10)
                    return;
                if (data_0209f2a4 == 1) {
                    int t = 0;
                    if (gTouchHeld[slot * 4] != 0) {
                        if (gTouchEdge[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                data_0209f2a4 = 1;
                data_0209f234 = data_0209f234 - 1;
                if (data_0209f234 == 0xff)
                    data_0209f234 = 2;
                data_0209f238 = 2;
                data_0209f210 = data_0208ee44 * 3;
                data_0209f2e0 = 0;
                _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
                data_0209f2e4 = 8;
                dScStage_c::PS_UpdateOptionsMenu();
                SetSoundMode(data_0209f234);
                func_02012790(0x64);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x5c) < 0x10)
                    return;
                return;
            }
        }
        {
            u8 ty2;
            int t6;
            int tmpv;
            if ((u8)(tx - 0xea) < 0x14) {
                ty2 = gTouchY[slot * 4];
                if ((u8)(ty2 - 0x2e) < 0x14)
                    goto snd_next;
            }
            tmpv = tx - 0x6e;
            if ((u8)tmpv < 0x7c) {
                ty2 = gTouchY[slot * 4];
                if ((u8)(ty2 - 0x26) < 0x24)
                    goto snd_next;
            }
            t6 = tx - 6;
            if ((u8)t6 < 0x3c) {
                ty2 = gTouchY[slot * 4];
                if ((u8)(ty2 - 0x26) < 0x24)
                    goto snd_next;
            }
            goto backlight_on;
        snd_next:
            if ((u8)(tx - 0xec) < 0x10) {
                if ((u8)(ty2 - 0x30) < 0x10)
                    goto snd_next_go;
            }
            if ((u8)(tx - 0x70) < 0x78) {
                if ((u8)(ty2 - 0x28) < 0x20)
                    goto snd_next_go;
            }
            if ((u8)(tx - 8) >= 0x38)
                return;
            if ((u8)(ty2 - 0x28) >= 0x20)
                return;
        snd_next_go:
            if (data_0209f2a4 == 2) {
                int t = 0;
                if (gTouchHeld[slot * 4] != 0) {
                    if (gTouchEdge[slot * 4] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 2;
            data_0209f234 = data_0209f234 + 1;
            if (data_0209f234 == 3)
                data_0209f234 = 0;
            data_0209f238 = 1;
            data_0209f210 = data_0208ee44 * 3;
            data_0209f2e0 = 0;
            SetSoundMode(data_0209f234);
            func_02012790(0x64);
            data_0209f2e4 = 8;
            dScStage_c::PS_UpdateOptionsMenu();
            _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
            {
                u8 s3 = gActivePlayerSlot;
                u8 x3 = gTouchX[s3 * 4];
                if ((u8)(x3 - 0xec) < 0x10) {
                    if ((u8)(gTouchY[s3 * 4] - 0x28) < 0x10)
                        return;
                }
                if ((u8)(x3 - 0x70) < 0x78) {
                    if ((u8)(gTouchY[s3 * 4] - 0x20) < 0x20)
                        return;
                }
                if ((u8)(x3 - 8) < 0x38)
                    return;
                return;
            }
        backlight_on:
            if ((u8)tmpv < 0x3c) {
                u8 ty2b = gTouchY[slot * 4];
                if ((u8)(ty2b - 0x56) < 0x24) {
                tmpv = tx - 0x70;
                if ((u8)tmpv >= 0x38)
                    return;
                tmpv = ty2b - 0x58;
                if ((u8)tmpv >= 0x20)
                    return;
                if (data_0209f2a4 == 3) {
                    int t = 0;
                    if (gTouchHeld[slot * 4] != 0) {
                        if (gTouchEdge[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                data_0209f2a4 = 3;
                if (data_0209f2ec != 0) {
                    func_02012790(0x66);
                    TurnBacklightOn();
                } else {
                    data_0209f2cc = data_0208ee44 << 2;
                    func_02012790(0x67);
                }
                data_0209f2ec = 0;
                data_0209f2e0 = 1;
                dScStage_c::PS_UpdateOptionsMenu();
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x70) < 0x38)
                    return;
                return;
                }
            }
        chk_bl_off:
            if ((u8)(tx - 0xae) < 0x3c) {
                u8 ty3 = gTouchY[slot * 4];
                if ((u8)(ty3 - 0x56) < 0x24) {
                    tmpv = tx - 0xb0;
                    if ((u8)tmpv >= 0x38)
                        return;
                    tmpv = ty3 - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (data_0209f2a4 == 4) {
                        int t = 0;
                        if (gTouchHeld[slot * 4] != 0) {
                            if (gTouchEdge[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 4;
                    if (data_0209f2ec == 0) {
                        func_02012790(0x66);
                        TurnBacklightOff();
                    } else {
                        data_0209f2cc = data_0208ee44 << 2;
                        func_02012790(0x67);
                    }
                    data_0209f2ec = 1;
                    data_0209f2e0 = 1;
                    dScStage_c::PS_UpdateOptionsMenu();
                    if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0xb0) < 0x38)
                        return;
                    return;
                }
            }
        chk_bl_off2:
            if ((u8)t6 < 0x4c) {
                u8 ty4 = gTouchY[slot * 4];
                if ((u8)(ty4 - 0x56) < 0x24) {
                    tmpv = tx - 8;
                    if ((u8)tmpv >= 0x48)
                        return;
                    tmpv = ty4 - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (data_0209f2a4 == 5) {
                        int t = 0;
                        if (gTouchHeld[slot * 4] != 0) {
                            if (gTouchEdge[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 5;
                    if (data_0209f2e0 != 1) {
                        data_0209f2e0 = 1;
                        dScStage_c::PS_UpdateOptionsMenu();
                    }
                    data_0209f2ec = data_0209f2ec ^ 1;
                    if (data_0209f2ec == 0) {
                        TurnBacklightOn();
                    } else {
                        TurnBacklightOff();
                    }
                    data_0209f2cc = data_0208ee44 << 2;
                    dScStage_c::PS_UpdateOptionsMenu();
                    func_02012790(0x66);
                    return;
                }
            }
            {
                int t = 0;
                if (gTouchHeld[slot * 4] != 0) {
                    if (gTouchEdge[slot * 4] != 0)
                        t = 1;
                }
                if (t == 0)
                    goto biv_a;
                if (relx >= firstw)
                    goto chk_dx;
                if ((u8)(gTouchY[slot * 4] - 0x98) < 0x20)
                    goto opt_okback;
            chk_dx:
                if ((u8)(tx - 0xd8) < 0x20) {
                    if ((u8)(gTouchY[slot * 4] - 0x98) < 0x20)
                        goto opt_okback;
                }
            biv_a:
                if (IsButtonInputValid() == 0)
                    return;
            opt_okback:
                if (relx < firstw) {
                    if ((u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20)
                        goto set29c;
                }
                if (IsButtonInputValid() != 0) {
                set29c:
                    data_0209f29c = 0;
                    goto opt_go;
                }
                {
                    u8 s4 = gActivePlayerSlot;
                    if ((u8)(gTouchX[s4 * 4] - 0xd8) < 0x20 &&
                        (u8)(gTouchY[s4 * 4] - 0x98) < 0x20) {
                        data_0209f29c = 1;
                    }
                }
            opt_go:
                data_0209f2e0 = 2;
                data_0209f22c = data_0208ee44 << 3;
                dScStage_c::PS_UpdateOptionsMenu();
                data_0209f240 = 1;
                data_0209f244 = data_0208ee44 << 2;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                if (data_0209f29c == 0) {
                    SetSoundMode(data_0209f234);
                    if (data_0209f1fc != 0) {
                        data_0208ee40 = 1;
                    } else {
                        data_0208ee40 = 0;
                    }
                    data_0209f2c4 = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5d);
                if (SublevelToLevel(data_0209f2f8) == 0x1d) {
                    data_0209f1ec = 0;
                } else {
                    data_0209f1ec = 2;
                }
                return;
            }
        }
    }
    case 0xb: {
        if (IsButtonInputValid() == 0)
            return;
        data_0209f244 = data_0208ee44 << 2;
        data_0209f2c4 = 2;
        func_02012790(3);
        return;
    }
    case 0xc: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        _ZN7Message19DisplaySaveMenuTextEt(0x290);
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209f1ec = 0xd;
        return;
    }
    case 0xd: {
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x28) < 0x20)
                goto seld_0;
            if (data_0209f2e0 == 0 && IsButtonInputValid() != 0)
                goto seld_0;
            goto chkd_1;
        seld_0:
            if (data_0209f2e0 == 0)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 0;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f2c4 = 2;
            func_02012790(3);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chkd_1:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x50) < 0x20)
                goto seld_1;
            if (data_0209f2e0 == 1 && IsButtonInputValid() != 0)
                goto seld_1;
            goto chkd_2;
        seld_1:
            if (data_0209f2e0 == 1)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0xe;
            func_02012790(0x5e);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    chkd_2:
        {
            u8 slot = gActivePlayerSlot;
            if ((u8)(gTouchX[slot * 4] - 8) < 0xf0 &&
                (u8)(gTouchY[slot * 4] - 0x78) < 0x20)
                goto seld_2;
            if (data_0209f2e0 != 2)
                return;
            if (IsButtonInputValid() == 0)
                return;
        seld_2:
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x10;
            func_02012790(0x5f);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 0xe: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        data_0209f1ec = 0xf;
        _ZN7Message21DisplaySaveStatusTextEt(0x295);
        data_0209d45c &= ~2;
        data_0209f2b8 = 0x78;
        return;
    }
    case 0xf: {
        if (data_0209f2b8 == 0)
            return;
        if (data_0209f2b8 == 0x78)
            _ZN8SaveData15SaveCurrentFileEv();
        if (data_0209f2b8 == 0x3c)
            _ZN7Message21DisplaySaveStatusTextEt(0x296);
        data_0209f2b8 = data_0209f2b8 - data_0208ee44;
        if (data_0209f2b8 != 0)
            return;
        if (data_0209f2e0 != 1) {
            data_0209f2c4 = 2;
            return;
        }
        _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
        return;
    }
    case 0x10: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1100;
        data_0209f1ec = 0x11;
        _ZN7Message19DisplayDontSaveTextEt(0x297);
        data_0209f2e0 = 1;
        dScStage_c::PS_UpdateSaveMenu(1);
        return;
    }
    case 0x11: {
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            register u8 slot;
            register u8 tx;
            int rel;
            int rely;
            slot = gActivePlayerSlot;
            tx = gTouchX[slot * 4];
            rel = tx - 0x28;
            if ((u8)rel < 0x50) {
                rely = gTouchY[slot * 4] - 0x98;
                if ((u8)rely < 0x20) {
                    if (data_0209f2e0 == 0)
                        data_0209f244 = data_0208ee44 << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        data_0209f22c = data_0208ee44 << 3;
                        data_0209f1ec = 0x12;
                        func_02012790(0x60);
                    } else {
                        func_02012790(0x51);
                    }
                    data_0209f2e0 = 0;
                    dScStage_c::PS_UpdateSaveMenu(0);
                    return;
                }
            }
            {
                rel = tx - 0x88;
                if ((u8)rel >= 0x50)
                    return;
                {
                    rely = gTouchY[slot * 4] - 0x98;
                    if ((u8)rely >= 0x20)
                        return;
                    if (data_0209f2e0 == 1)
                        data_0209f244 = data_0208ee44 << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        data_0209f22c = data_0208ee44 << 3;
                        data_0209f1ec = 0x12;
                        func_02012790(3);
                    } else {
                        func_02012790(0x52);
                    }
                    data_0209f2e0 = 1;
                    dScStage_c::PS_UpdateSaveMenu(0);
                    return;
                }
            }
        }
    }
    case 0x12: {
        if (data_0209f2e0 != 0) {
            data_0209f2c4 = 2;
            return;
        }
        _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        return;
    }
    case 0x13: {
        int t = (data_0209f2d8 == 1);
        if (t != false && data_0209fc68 == 0) {
            _ZN8dScene_c14StartSceneFadeEjjt(6, 1, 0);
            data_0209f280 = 1;
            dScStage_c::PS_Cleanup();
            data_0209b454 |= 0x40000000;
            data_0209b464 = data_0209b454;
            return;
        }
        LoadLevelNoReturn(2, 0, -1, 0);
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        data_0209f280 = 1;
        data_0209f2c4 = 2;
        return;
    }
    default:
        return;
    }
}
#pragma opt_common_subs on
