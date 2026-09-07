//cpp
// @symbol _ZN11dScMgBase_c9Virtual84Ev
/* dScMgBase_c::Virtual84 - slot 33, and the only slot in this campaign with
   NOTHING to correct.  There was no `recovered name:` line on this file or on
   any of its overrides: the recovery pass never guessed a name here, so unlike
   slots 26, 29, 30, 31 and 32 there is no borrowed label to retire.  The ROM
   names nothing either -- dScMgBase_c is a SCENE (fBase_c -> dBase_c ->
   dScene_c -> dScMgBase_c) and dActor_c, whose names slots 18-30 borrowed by
   index, has no slot 33 at all.  Virtual84 is the repo's own no-name spelling,
   after the +0x84 vtable offset, the same convention fBase_c uses for
   Virtual34 and Virtual38.  See the slot-33 block in include/dScMgBase_c.h.

   ENGINE BRING-UP.  Graphics modes for both engines (GX mode 1/0/0, GXS mode
   0), VRAM banks assigned to BG and OBJ on both screens, and BOTH BG-enable
   shadows -- data_0209d45c for the main engine, data_0209d454 for the sub --
   initialised to 0x10, which is the value slots 30 and 31 later save, clear
   bits out of and restore.  A language-indexed character file (indexed by
   GetGameLanguage into data_ov004_020bbfe4) is decompressed into BOTH engines'
   BG char VRAM at 0x06404000 and 0x06604000, OBJ palette file 0xc3 is loaded
   into both, and the scene object is published into the global registry --
   data_ov004_020beb74[1] = this, then data_0209d4a8 points at that registry.
   The object fields it touches are its own: +0x68 cleared and +0x6c set to -1.

   WHEN IT RUNS.  First out of dScMgBase_c::BeforeInitResources
   (ov004:0x020b0930): the +0x84 dispatch at 0x020b09d0 is near the top and
   slot 31's +0x7c dispatch at 0x020b0a0c is the last thing before the
   function returns 1.  Slot 32 runs out of AfterInitResources.  So the
   sequence is: bring the engines up (33), dress the sub screen (31), then
   dress the main screen (32).

   arity: no explicit parameters, MEASURED.  Scanning arm9 and all 103 overlays
   for the dispatch pair -- `ldr rD,[rN,#0x84]` with Rn != pc, followed within
   three instructions by `blx rD`/`bx rD` -- finds exactly two sites image-wide,
   of which one is in ov004 or ov006: the 0x020b09d0 call above.  It reads
   `mov r0,r4; ldr r1,[r0]; ldr r1,[r1,#0x84]; blx r1`, so r1 is the loaded
   pointer and cannot also be a second argument.  The scanner was validated by
   re-running it at +0x80 and reproducing slot 32's known call site.
   return type: void, which is what all three bodies do -- none assigns a
   result and the one caller ignores whatever falls out. */
#include "types.h"
#include "dScMgBase_c.h"
extern "C" {
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern void func_ov004_020b2980(void);
extern void func_ov004_020b290c(void);
extern void _ZN2GX12SetBankForBGEt(u16 a);
extern void _ZN2GX13SetBankForOBJEt(u16 a);
extern void _ZN2GX15SetBankForSubBGEt(u16 a);
extern void _ZN2GX16SetBankForSubOBJEt(u16 a);
extern s32 GetGameLanguage(void);
extern void *func_ov004_020adc68(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void Ov004_Deallocate(void *p);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 len);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_ov004_020b0d30(void);
extern void FreeGfxSlotsById(int arg);
}

extern "C" {
extern int data_ov004_020beb6c;
extern u8 data_0209d45c;
extern int data_ov004_020bbfe4[];
extern int data_ov004_020beb74[];
extern u8 data_0209d454;
extern void **data_0209d4a8;
extern int data_0208ee44;
}

void dScMgBase_c::Virtual84()
{
    char *obj = (char *)this;

    void *p;

    data_ov004_020beb6c = 0;
    obj[0x68] = 0;
    *(int *)(obj + 0x6c) = -1;
    *(vu32 *)0x4001000u |= 0x10000u;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);
    *(vu32 *)0x4001000u &= 0xffcfffefu;
    *(vu16 *)0x4000304u |= 0x8000u;
    func_ov004_020b2980();
    func_ov004_020b290c();
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    _ZN2GX12SetBankForBGEt(3);
    _ZN2GX13SetBankForOBJEt(0x10);
    data_0209d45c = 0x10;
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);
    p = func_ov004_020adc68(data_ov004_020bbfe4[GetGameLanguage()]);
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    Ov004_Deallocate(p);
    p = func_ov004_020adc68(0xc3);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(p, 0x100u);
    _ZN2GX11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    _ZN3GXS11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    Ov004_Deallocate(p);
    data_0209d454 = 0x10;
    data_ov004_020beb74[1] = (int)obj;
    data_0209d4a8 = (void **)data_ov004_020beb74;
    func_ov004_020b0d30();
    FreeGfxSlotsById(0x1d);
    data_0208ee44 = 1;
}
