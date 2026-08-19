// @symbol _ZN16dScMgSmartball_c13InitResourcesEv
// NONMATCHING: compiler-build floor at the entry, cascades through the body.
/* dScMgSmartball_c::InitResources() -- vtable slot 0 (fBase_c::InitResources).
 *
 * Plain C carries the literal mangled name with no mangling needed -- the class
 * declares `s32 InitResources()` in include/dScMgSmartball_c.h, the same shape
 * src/_ZN12dScStarSel_c13InitResourcesEv.c uses. This is the resource loader the
 * Slots Shot / Bingo Ball scene (376) crashes without: it builds both screens'
 * BG layers, loads the minigame tiles/palettes/OBJ, zeroes the eleven object
 * slot arrays, constructs the spring singleton, sets the two blend registers,
 * fixes the OAM priority of twelve cell-animation templates, then hands off to
 * func_ov006_02115b0c and returns 1.
 *
 * WHY NONMATCHING (verified against ROM 0x02118b70, size 0x8dc):
 *   The prologue reads `*(int*)(this+8)` once and masks its low byte TWICE --
 *   `ands r0,r0,#0xff` (the `if` test) then a redundant `and r0,r0,#0xff` before
 *   storing it to this+0xbc. mwccarm 2004/b56 folds the second mask away as a
 *   common subexpression in EVERY C spelling tried (implicit `if (x & 0xff)`,
 *   `(u8)x`, a masked temp, a split assignment); the ROM's own compiler
 *   (CodeWarrior for NITRO 0.6.1) kept it. That single extra instruction shifts
 *   the whole function by one word, so nothing downstream lines up. This is a
 *   compiler-build floor, not a logic gap -- the permuter cannot cross it either,
 *   because it rewrites C and no C form makes 2004/b56 emit the redundant mask.
 *
 * LOGIC IS VERIFIED INSTRUCTION BY INSTRUCTION. Every asset-load block, the
 * (this+8 & 0xff) asset-set branch, the seven slot-array zeroing loops, the
 * spring placement-new (_Znwj(0x48) -> func_ov006_0211157c), the two
 * SetBlendAlpha calls and all twelve cell-priority fixups were read off the
 * capstone disassembly and cross-checked against the fan-out near-miss in
 * nearmiss/db.jsonl; all three agree. Callee names are the resolvable symbols
 * (func_ov004_020adc74 / Ov004_Deallocate, matching dScMgSnowball_c). */

typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;

extern void *func_ov004_020adc74(void *sym);        /* load compressed file by name */
extern void  Ov004_Deallocate(void *p);             /* free the temp buffer */
extern void  DecompressLZ16(void *src, void *dst);
extern void  MultiStore16(u16 val, void *dst, s32 nbytes);
extern void  _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);

extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern void *func_02054d88(void);
extern void *_ZN3G2S13GetBG3CharPtrEv(void);

extern void _ZN2GX10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 size);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 size);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *src, u32 offset, u32 size);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *src, u32 offset, u32 size);

extern void *_Znwj(u32 size);
extern s32   func_ov006_0211157c(void *self, void *scene, s32 idx, s32 *pos);
extern void  _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16 *addr, u16 a, u16 b, u16 c, u16 d);
extern void  func_ov006_02115b0c(void *scene);

extern s32 data_0208ee44;
extern u8  data_0209d45c;
extern u8  data_0209d454;

extern char data_ov006_0213f260;
extern char data_ov006_0213f280;
extern char data_ov006_0213f2a0;
extern char data_ov006_0213f2bc;
extern char data_ov006_0213f2ec;
extern char data_ov006_0213f31c;
extern char data_ov006_0213f348;
extern char data_ov006_0213f374;
extern char data_ov006_0213f3a4;
extern char data_ov006_0213f3d4;
extern char data_ov006_0213f400;
extern char data_ov006_0213f42c;
extern char data_ov006_0213f458;
extern char data_ov006_0213f484;
extern char data_ov006_0213f4b0;
extern char data_ov006_0213f4dc;

extern char data_ov006_021382e0;
extern char data_ov006_021382f8;
extern char data_ov006_02137a9c;
extern char data_ov006_02138a08;
extern char data_ov006_0213765c;
extern char data_ov006_02137674;
extern char data_ov006_02138a20;
extern char data_ov006_02138a38;
extern char data_ov006_02137630;
extern char data_ov006_02137684;
extern char data_ov006_02137994;

/* Force priority bits 10..11 to 0b01 in the cell's control word, then re-write
 * the palette nibble of its second halfword back onto itself. The palette write
 * is the ROM's own redundant read-modify-write and is a no-op, kept for fidelity.
 * Expanded inline (the ROM inlines all twelve of these), so a macro not a call. */
#define FIX_CELL_PRIO(cellExpr)                                                 \
    do {                                                                        \
        char *cell__ = (char *)(cellExpr);                                      \
        u16 pal__ = *(u16 *)(cell__ + 4);                                       \
        *(u32 *)cell__ = (*(u32 *)cell__ & ~0xc00) | 0x400;                     \
        *(u16 *)(cell__ + 4) = (u16)((*(u16 *)(cell__ + 4) & ~0xf000) |         \
            ((((s32)(pal__ & 0xf000)) >> 12) << 12));                           \
    } while (0)

s32 _ZN16dScMgSmartball_c13InitResourcesEv(void *arg0)
{
    char *c = (char *)arg0;
    void *scr;
    volatile u16 h;
    void *buf;
    void *o9;
    u16 pal;
    s32 i;

    /* Clamp the layout selector (this+8, low byte) into this+0xbc, max 0x270e. */
    if (*(s32 *)(c + 8) & 0xff) {
        *(s32 *)(c + 0xbc) = *(s32 *)(c + 8) & 0xff;
        if ((u32)*(s32 *)(c + 0xbc) > 0x270eU)
            *(s32 *)(c + 0xbc) = 0x270e;
    }

    data_0208ee44 = 1;
    *(volatile u16 *)0x04001008 = (u16)((*(volatile u16 *)0x04001008 & 0x43) | 0x520c);
    *(volatile u16 *)0x04001008 &= ~0x40;
    *(volatile s32 *)0x04001010 = 0x100;
    *(volatile u16 *)0x04001008 &= ~3;

    buf = func_ov004_020adc74(&data_ov006_0213f260);
    DecompressLZ16(buf, _ZN3G2S13GetBG0CharPtrEv());
    Ov004_Deallocate(buf);

    scr = _ZN3G2S12GetBG0ScrPtrEv();
    h = 0;
    MultiStore16(h, scr, 0x1000);

    buf = func_ov004_020adc74(&data_ov006_0213f280);
    DecompressLZ16(buf, _ZN3G2S12GetBG0ScrPtrEv());
    Ov004_Deallocate(buf);

    buf = func_ov004_020adc74(&data_ov006_0213f2a0);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)buf, 0x20);
    _ZN3GXS10LoadBGPlttEPKvjj((char *)buf + 2, 2, 0x1e);
    Ov004_Deallocate(buf);

    *(volatile u16 *)0x0400000c = (u16)((*(volatile u16 *)0x0400000c & 0x43) | 0x1410);
    *(volatile u16 *)0x0400000c &= ~0x40;
    *(volatile s32 *)0x04000018 = 0;
    *(volatile u16 *)0x0400000c = (u16)((*(volatile u16 *)0x0400000c & ~3) | 1);

    *(volatile u16 *)0x0400100c = (u16)((*(volatile u16 *)0x0400100c & 0x43) | 0x1410);
    *(volatile u16 *)0x0400100c &= ~0x40;
    *(volatile s32 *)0x04001018 = 0;
    *(volatile u16 *)0x0400100c = (u16)((*(volatile u16 *)0x0400100c & ~3) | 1);

    *(volatile u16 *)0x0400000e = (u16)((*(volatile u16 *)0x0400000e & 0x43) | 0x1618);
    *(volatile u16 *)0x0400000e &= ~0x40;
    *(volatile s32 *)0x0400001c = 0;
    *(volatile u16 *)0x0400000e = (u16)((*(volatile u16 *)0x0400000e & ~3) | 2);

    *(volatile u16 *)0x0400100e = (u16)((*(volatile u16 *)0x0400100e & 0x43) | 0x1618);
    *(volatile u16 *)0x0400100e &= ~0x40;
    *(volatile s32 *)0x0400101c = 0;
    *(volatile u16 *)0x0400100e = (u16)((*(volatile u16 *)0x0400100e & ~3) | 2);

    /* Layout 0 loads the "_lay0" tile set, any other layout loads its own. */
    if ((u8)*(s32 *)(c + 8) == 0) {
        buf = func_ov004_020adc74(&data_ov006_0213f2bc);
        DecompressLZ16(buf, _ZN2G213GetBG2CharPtrEv());
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f2ec);
        DecompressLZ16(buf, _ZN3G2S13GetBG2CharPtrEv());
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f31c);
        DecompressLZ16(buf, func_02054d88());
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f348);
        DecompressLZ16(buf, _ZN3G2S13GetBG3CharPtrEv());
        Ov004_Deallocate(buf);
    } else {
        buf = func_ov004_020adc74(&data_ov006_0213f374);
        DecompressLZ16(buf, _ZN2G213GetBG2CharPtrEv());
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f3a4);
        DecompressLZ16(buf, _ZN3G2S13GetBG2CharPtrEv());
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f3d4);
        DecompressLZ16(buf, func_02054d88());
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f400);
        DecompressLZ16(buf, _ZN3G2S13GetBG3CharPtrEv());
        Ov004_Deallocate(buf);
    }

    if ((u8)*(s32 *)(c + 8) == 0) {
        buf = func_ov004_020adc74(&data_ov006_0213f42c);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)buf, 0x1a0);
        _ZN2GX10LoadBGPlttEPKvjj(buf, 0x60, 0x1a0);
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f458);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)buf, 0x1a0);
        _ZN3GXS10LoadBGPlttEPKvjj(buf, 0x60, 0x1a0);
        Ov004_Deallocate(buf);
    } else {
        buf = func_ov004_020adc74(&data_ov006_0213f484);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)buf, 0x1a0);
        _ZN2GX10LoadBGPlttEPKvjj(buf, 0x60, 0x1a0);
        Ov004_Deallocate(buf);

        buf = func_ov004_020adc74(&data_ov006_0213f4b0);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)buf, 0x1a0);
        _ZN3GXS10LoadBGPlttEPKvjj(buf, 0x60, 0x1a0);
        Ov004_Deallocate(buf);
    }

    data_0209d45c &= ~1;
    data_0209d454 |= 0xd;
    data_0209d45c |= 0xc;

    buf = func_ov004_020adc74(&data_ov006_0213f4dc);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)buf, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj(buf, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj(buf, 0, 0x100);
    Ov004_Deallocate(buf);

    /* Clear the eleven object-slot arrays (balls, mushrooms, holes, propellers,
     * pipes, piranhas, push switches) -- the counterparts AfterCleanupResources
     * walks and frees. */
    for (i = 0; i < 0xd; i++)
        ((s32 *)(c + 0x4688))[i] = 0;
    for (i = 0; i < 0x19; i++)
        ((s32 *)(c + 0x46bc))[i] = 0;
    for (i = 0; i < 8; i++)
        ((s32 *)(c + 0x4720))[i] = 0;
    for (i = 0; i < 3; i++)
        ((s32 *)(c + 0x4740))[i] = 0;
    for (i = 0; i < 6; i++)
        ((s32 *)(c + 0x474c))[i] = 0;
    for (i = 0; i < 3; i++)
        ((s32 *)(c + 0x4764))[i] = 0;
    for (i = 0; i < 2; i++)
        ((s32 *)(c + 0x4770))[i] = 0;

    /* Spring singleton at this+0x4684: placement-new a 0x48-byte object at a fixed
     * table position (0xec000, 0xa0000). */
    {
        s32 pos[2];
        void *spring;

        pos[0] = 0xec000;
        pos[1] = 0xa0000;
        spring = _Znwj(0x48);
        if (spring != 0) {
            s32 arg[2];
            arg[0] = pos[0];
            arg[1] = pos[1];
            spring = (void *)func_ov006_0211157c(spring, arg0, 0, arg);
        }
        *(void **)(c + 0x4684) = spring;
    }
    *(s32 *)(c + 0x4778) = 0;
    *(s32 *)(c + 0x477c) = 0;
    *(s32 *)(c + 0x4780) = 0;

    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16 *)0x04000050, 0, 0x18, 4, 0xa);
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16 *)0x04001050, 0, 0x18, 4, 0xa);

    /* Fix the OAM priority of the cell-animation templates. Each global holds the
     * template pointer at a fixed offset. The 0x02137630 template carries two
     * sub-cells (offset +8 gives the first; its second sub-cell at +8/+0xc copies
     * the palette nibble from the first). */
    FIX_CELL_PRIO(*(void **)(&data_ov006_021382e0 + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_021382f8 + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_02137a9c + 8));
    FIX_CELL_PRIO(*(void **)(&data_ov006_02138a08 + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_0213765c + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_02137674 + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_02138a20 + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_02138a38 + 4));

    o9 = *(void **)(&data_ov006_02137630 + 8);
    FIX_CELL_PRIO(o9);
    pal = *(u16 *)((char *)o9 + 4);
    *(u32 *)((char *)o9 + 8) = (*(u32 *)((char *)o9 + 8) & ~0xc00) | 0x400;
    *(u16 *)((char *)o9 + 0xc) =
        (u16)((*(u16 *)((char *)o9 + 0xc) & ~0xf000) | ((((s32)(pal & 0xf000)) >> 12) << 12));

    FIX_CELL_PRIO(*(void **)(&data_ov006_02137684 + 4));
    FIX_CELL_PRIO(*(void **)(&data_ov006_02137994 + 4));

    func_ov006_02115b0c(arg0);
    return 1;
}
