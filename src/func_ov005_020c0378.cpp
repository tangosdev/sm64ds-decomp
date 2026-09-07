//cpp
// @symbol func_ov005_020c0378
/* recovered: real types against include/dScMiniGm_c.h.

   func_ov005_020c0378 -- ov005 0x020c0378, 0x354 bytes. The minigame select
   screen's tap handler: when the touch slot data_020a0e40 is pressed and held,
   find which icon cell it lands in (a 2x3 grid on the main page, a single row of
   three on the other), and if that game is unlocked copy its 0x34-byte entry into
   data_0209b308, fade the scene out and start it.

   Rewritten from the disassembly against the register assignment the cartridge
   shows, which pins three things about the source:
     * the touch slot index is not held in a local: data_020a0de8[data_020a0e40]
       is spelled out at every use, and the compiler keeps the one load in r5. A
       named `idx` local colours scratch (r2) and pushes the page base into r5.
     * the page base data_0208a170 is read inside the loops, hoisted by the compiler
       into lr / r3; naming it moves it to a callee-saved register.
     * each loop has its own counter (`int col` per loop). Reusing the 2x3 grid's
       column counter for the single-row loop colours it scratch (r2) instead of
       r8, and the whole row of inductions shifts down by one.
   The hit flag is an int, not a bool: as a bool the compiler folds it into the
   branch structure and the `mov r2, #0 / movne r2, #1` materialisation is lost. */
#include "dScMiniGm_c.h"

struct TouchRec { u8 pressed; u8 held; u8 x; u8 y; };
struct MgMid { s32 v[5]; };
struct MgEntry {
    u16 id; u16 pad;
    s32 unk4, unk8, unkc, unk10;
    MgMid mid;
    s32 unk28, unk2c, unk30;
};
struct TouchOwner { virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int IsActive(); };

extern "C" {
extern TouchOwner *data_0209f5bc;
extern u8 data_020a0e40;
extern TouchRec data_020a0de8[];
extern u8 data_0209b304;
extern s32 data_0208a170;
extern s32 data_0208a174;
extern u8 data_0209b2fc;
extern u8 data_0209f61c;
extern MgEntry data_0209b308;
extern MgMid data_0209b31c;
extern const MgEntry data_ov005_020c24d8[];
int func_ov005_020c00b4(void *self, int n);
void func_ov005_020c1688(void *self, int n);
void func_02012790(int n);
void _ZN8dScene_c9SetFadersEP15FaderBrightness(void *p);
void func_0202ec9c(void *p, int v);
int _ZN8dScene_c14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 n);

void func_ov005_020c0378(dScMiniGm_c *self)
{
    int sel;
    int ok;
    int row;

    if (data_0209f5bc->IsActive() == 0) return;
    if (self->mExiting != 0) return;
    if (self->mPageFlipped == 1) return;
    if (self->mPrevPageTimer > 0) return;
    if (self->mNextPageTimer > 0) return;
    if (self->mExitTimer > 0) return;

    sel = -1;
    ok = 0;
    if (data_020a0de8[data_020a0e40].pressed != 0) {
        if (data_020a0de8[data_020a0e40].held != 0) ok = 1;
    }
    if (ok != 0) {
        if (data_0209b304 == 0) {
            for (row = 0; row < 2; row++) {
                for (int col = 0; col < 3; col++) {
                    if (data_020a0de8[data_020a0e40].x >= 0x40 + col * 0x30 && data_020a0de8[data_020a0e40].x < 0x70 + col * 0x30 &&
                        data_020a0de8[data_020a0e40].y >= 0x18 + row * 0x48 && data_020a0de8[data_020a0e40].y < 0x60 + row * 0x48)
                        sel = data_0208a170 + (row * 0xc + col * 4);
                }
            }
        } else {
            for (int col = 0; col < 3; col++) {
                if (data_020a0de8[data_020a0e40].x >= 0x60 + col * 0x30 && data_020a0de8[data_020a0e40].x < 0x90 + col * 0x30 &&
                    data_020a0de8[data_020a0e40].y >= 0x38 && data_020a0de8[data_020a0e40].y < 0x80)
                    sel = data_0208a170 + (0x18 + col * 4);
            }
        }
    }

    if (sel < 0) return;
    if (func_ov005_020c00b4(self, sel) == 0) return;

    {
        const MgEntry *src = &data_ov005_020c24d8[sel];
        data_0208a174 = sel;
        data_0209b308.id = src->id;
        data_0209b308.unk4 = src->unk4;
        data_0209b308.unk8 = src->unk8;
        data_0209b308.unkc = src->unkc;
        data_0209b308.unk10 = src->unk10;
        data_0209b31c = src->mid;
        data_0209b308.unk28 = src->unk28;
        data_0209b308.unk2c = src->unk2c;
        data_0209b308.unk30 = src->unk30;
    }
    func_ov005_020c1688(self, sel);
    func_02012790(0x1e);
    _ZN8dScene_c9SetFadersEP15FaderBrightness(&data_0209f61c);
    func_0202ec9c(&data_0209f61c, 0);
    data_0209b308.unk30 = self->unk_05c;
    _ZN8dScene_c14StartSceneFadeEjjt(data_ov005_020c24d8[data_0208a174].id, data_ov005_020c24d8[data_0208a174].unk4, 0);
    _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
    self->mExiting = 1;
    data_0209b2fc = 1;
}
}
