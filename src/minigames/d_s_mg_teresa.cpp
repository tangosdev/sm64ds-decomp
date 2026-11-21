//cpp
/* Hide and Boo Seek. The player watches the Boo cross the room, the lights
 * go out, and they rub the Touch Screen to uncover every hiding Boo. The
 * first 15 rounds give three seconds; after that the clock is two. This
 * file is all 82 functions of dScMgTeresa_c's ov006 unit (.text
 * 0x0211cbd0..0x021207dc): the big Boo's flight, the countdown and the
 * reveal sprites, the 16 hiding Boos (spawn, flight, wall bounce, fade and
 * the touch test against the rubbed-out BG0 tiles), the scene states, and
 * the class's own overrides, and last the factory dScMgTeresa_c_classInit.
 * Below it is dScMgSound_c.
 *
 * _ZTS13dScMgTeresa_c 0x0213f9c0. _ZTI13dScMgTeresa_c 0x0213f9b4.
 * _ZTV13dScMgTeresa_c 0x0213fa0c.
 *
 * Nothing in the ROM names the helpers, so the members keep their
 * func_ov006_ names. InitResources, Behavior, Render and StartRound were
 * named from their vtable slots (see dScMgBase_c.h).
 *
 * Functions run in ROM order, lowest address first, because of
 * `#pragma defer_codegen off` below. Do not reorder, and do not drop that
 * pragma. It makes the out-of-line destructor come out D1, D0 as in the
 * ROM (the unused D2 trails it and is deadstripped), and it is what lets
 * the two push and pop brackets bind. Each bracket was measured: without
 * it, its function stops matching. The destructor is out of line, so it is
 * the key function and this file emits the vtable and RTTI for the whole
 * base chain.
 *
 * The four PMF state tables dispatch on this class: the big Boo's flight
 * at sTeresaBigBooStateTable (9 states by BigBoo::state), the reveal pair at
 * sTeresaRevealStateTable (5 states by mReveal[0].state), the 16-Boo rows at
 * sTeresaBooStateTable (8 states), and the scene table sTeresaSceneStateTable
 * (5 no-argument states by mSceneState). The tables are initialized from
 * &dScMgTeresa_c:: member pointers: mwcc emits each one as its own 8-byte
 * {fn, 0} .data constant and __sinit_d_s_mg_teresa.cpp copies it into the
 * table, as the cartridge does from 0x0213f8d4..0x0213f9b4.
 */

// leftovers:
// - Reveal_Start clears mReveal[0].unk_1b (byte 0x1b): nothing in
//   this TU reads it, so it has no name.
// - Boo_StateFadeAway and CountRubbedTiles flip a Boo's heading as
//   `mHeading = mHeading + 0x8000`; `+=` is a word longer in FadeAway
//   (measured 2026-11-07).
// - PaintSquare keeps opt_loop_invariants off and a hand-rotated loop; its
//   comment has the measurements.

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "OamAttr.h"
#include "dScMgTeresa_c.h"
#include "SpawnInfo.h"
#include "decl_common.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"
#include "PlayerInput.h"
#include "nitro/gx/gx_bgcnt.h"
#include "cstd.h"
#include "nitro/gx/gx_load.h"

/* Everything this file calls or reads that decl_common.h does not declare. */
extern "C" {

extern void RenderOamMainScreen(void *, int, int, int, int);
extern void *LoadFile(int handle);
extern void DecompressLZ16(const void *src, void *dst);
extern void FreeGfxSlotsById(int arg);
extern int GetGameLanguage(void);
struct Matrix2x2;
extern void DrawOamSprite(OamAttr *attr, int x, int y, Matrix2x2 *mtx);
extern void Hud_RenderSprite(OamAttr *fn, int a, int b, int c, int d);
extern void Mg_DrawNumberRotated(int, int, int, int, int, int, u16);
extern int Mg_IsNewRecord(void);
#include "OAM.h"
#include "nitro/mi/memory.h"

extern unsigned char gSubScreenLayers;
extern unsigned char gMainScreenLayers;
OamAttr data_ov006_02135268[40] = {
    {0x00f2, 0x41f3, 0x50d4, 0x0000}, {0x80f2, 0x0003, 0x50d6, 0x0000},
    {0x00e9, 0x4003, 0x4089, 0x0000}, {0x40f8, 0x01ea, 0x408b, 0x0000},
    {0x00ca, 0x81f2, 0x5005, 0x0000}, {0x40bd, 0x81f6, 0x4000, 0x0000},
    {0x80bd, 0x0016, 0x4004, 0x0000}, {0x00cd, 0x4006, 0x4042, 0x0000},
    {0x80cd, 0x0016, 0x4044, 0x0000}, {0x00da, 0x0012, 0x5049, 0x0000},
    {0x00e0, 0x81f0, 0x4080, 0x0000}, {0x40f9, 0x0003, 0x40c9, 0x0000},
    {0x40e3, 0x81d0, 0x5050, 0x0000}, {0x40f3, 0x41d0, 0x5090, 0x0000},
    {0x00e5, 0x41ed, 0x4040, 0x0000}, {0x00d7, 0x41ae, 0x60d7, 0x0000},
    {0x80d7, 0x01be, 0x60d9, 0x0000}, {0x00d7, 0x41c6, 0x60ba, 0x0000},
    {0x40e7, 0x01c6, 0x60fa, 0x0000}, {0x40e7, 0x01b6, 0x60bc, 0x0000},
    {0x44c3, 0x4184, 0x6017, 0x0000}, {0x44c3, 0x01a4, 0x601b, 0x0000},
    {0x04c3, 0x01b4, 0x601d, 0x0000}, {0x44cb, 0x0184, 0x601e, 0x0000},
    {0x44cb, 0x4194, 0x6037, 0x0000}, {0x44cb, 0x01b4, 0x603b, 0x0000},
    {0x04cb, 0x01c4, 0x603d, 0x0000}, {0x44d3, 0x0184, 0x603e, 0x0000},
    {0x44d3, 0x4194, 0x6057, 0x0000}, {0x44d3, 0x41b4, 0x605b, 0x0000},
    {0x04db, 0x0184, 0x605f, 0x0000}, {0x44db, 0x418c, 0x6077, 0x0000},
    {0x04db, 0x01ac, 0x607b, 0x0000}, {0x44db, 0x01c4, 0x607c, 0x0000},
    {0x44e3, 0x018c, 0x607e, 0x0000}, {0x44e3, 0x419c, 0x6097, 0x0000},
    {0x04e3, 0x01cc, 0x609b, 0x0000}, {0x44eb, 0x419c, 0x609c, 0x0000},
    {0x44eb, 0x01bc, 0x60b7, 0x0000}, {0x04eb, 0x01cc, 0x60b9, 0xffff},
};
OamAttr data_ov006_021354f8[42] = {
    {0x00f2, 0x41fb, 0x5011, 0x0000}, {0x80f2, 0x000b, 0x5013, 0x0000},
    {0x00ea, 0x4001, 0x40c4, 0x0000}, {0x80ea, 0x0011, 0x40c6, 0x0000},
    {0x40f2, 0x81e3, 0x40cd, 0x0000}, {0x00cd, 0x81f2, 0x5005, 0x0000},
    {0x40c0, 0x81f6, 0x4000, 0x0000}, {0x80c0, 0x0016, 0x4004, 0x0000},
    {0x00d0, 0x4006, 0x4042, 0x0000}, {0x80d0, 0x0016, 0x4044, 0x0000},
    {0x00dc, 0x0012, 0x5049, 0x0000}, {0x00e2, 0x81f0, 0x4080, 0x0000},
    {0x40e3, 0x81d3, 0x5050, 0x0000}, {0x40f3, 0x41d3, 0x5090, 0x0000},
    {0x00e5, 0x41f0, 0x4040, 0x0000}, {0x40fa, 0x01d3, 0x50eb, 0x0000},
    {0x40fa, 0x000c, 0x50cb, 0x0000}, {0x00d7, 0x41b1, 0x60d7, 0x0000},
    {0x80d7, 0x01c1, 0x60d9, 0x0000}, {0x00d7, 0x41c9, 0x60ba, 0x0000},
    {0x40e7, 0x01c9, 0x60fa, 0x0000}, {0x40e7, 0x01b9, 0x60bc, 0x0000},
    {0x44c3, 0x4187, 0x6017, 0x0000}, {0x44c3, 0x01a7, 0x601b, 0x0000},
    {0x04c3, 0x01b7, 0x601d, 0x0000}, {0x44cb, 0x0187, 0x601e, 0x0000},
    {0x44cb, 0x4197, 0x6037, 0x0000}, {0x44cb, 0x01b7, 0x603b, 0x0000},
    {0x04cb, 0x01c7, 0x603d, 0x0000}, {0x44d3, 0x0187, 0x603e, 0x0000},
    {0x44d3, 0x4197, 0x6057, 0x0000}, {0x44d3, 0x41b7, 0x605b, 0x0000},
    {0x04db, 0x0187, 0x605f, 0x0000}, {0x44db, 0x418f, 0x6077, 0x0000},
    {0x04db, 0x01af, 0x607b, 0x0000}, {0x44db, 0x01c7, 0x607c, 0x0000},
    {0x44e3, 0x018f, 0x607e, 0x0000}, {0x44e3, 0x419f, 0x6097, 0x0000},
    {0x04e3, 0x01cf, 0x609b, 0x0000}, {0x44eb, 0x419f, 0x609c, 0x0000},
    {0x44eb, 0x01bf, 0x60b7, 0x0000}, {0x04eb, 0x01cf, 0x60b9, 0xffff},
};
OamAttr data_ov006_02135798[42] = {
    {0x00e4, 0x41f1, 0x5014, 0x0000}, {0x40f4, 0x01f1, 0x5054, 0x0000},
    {0x00f4, 0x0001, 0x5056, 0x0000}, {0x40e3, 0x81d0, 0x5050, 0x0000},
    {0x80e4, 0x0001, 0x5016, 0x0000}, {0x40f2, 0x0003, 0x40c7, 0x0000},
    {0x00e2, 0x4003, 0x4087, 0x0000}, {0x00ca, 0x81f2, 0x5005, 0x0000},
    {0x40f8, 0x41e6, 0x40e7, 0x0000}, {0x40bd, 0x81f6, 0x4000, 0x0000},
    {0x80bd, 0x0016, 0x4004, 0x0000}, {0x00cd, 0x4006, 0x4042, 0x0000},
    {0x80cd, 0x0016, 0x4044, 0x0000}, {0x00da, 0x0012, 0x5049, 0x0000},
    {0x00e0, 0x81f0, 0x4080, 0x0000}, {0x40f3, 0x41d0, 0x5090, 0x0000},
    {0x00e5, 0x41ed, 0x4040, 0x0000}, {0x00d7, 0x41ae, 0x60d7, 0x0000},
    {0x80d7, 0x01be, 0x60d9, 0x0000}, {0x00d7, 0x41c6, 0x60ba, 0x0000},
    {0x40e7, 0x01c6, 0x60fa, 0x0000}, {0x40e7, 0x01b6, 0x60bc, 0x0000},
    {0x44c3, 0x4184, 0x6017, 0x0000}, {0x44c3, 0x01a4, 0x601b, 0x0000},
    {0x04c3, 0x01b4, 0x601d, 0x0000}, {0x44cb, 0x0184, 0x601e, 0x0000},
    {0x44cb, 0x4194, 0x6037, 0x0000}, {0x44cb, 0x01b4, 0x603b, 0x0000},
    {0x04cb, 0x01c4, 0x603d, 0x0000}, {0x44d3, 0x0184, 0x603e, 0x0000},
    {0x44d3, 0x4194, 0x6057, 0x0000}, {0x44d3, 0x41b4, 0x605b, 0x0000},
    {0x04db, 0x0184, 0x605f, 0x0000}, {0x44db, 0x418c, 0x6077, 0x0000},
    {0x04db, 0x01ac, 0x607b, 0x0000}, {0x44db, 0x01c4, 0x607c, 0x0000},
    {0x44e3, 0x018c, 0x607e, 0x0000}, {0x44e3, 0x419c, 0x6097, 0x0000},
    {0x04e3, 0x01cc, 0x609b, 0x0000}, {0x44eb, 0x419c, 0x609c, 0x0000},
    {0x44eb, 0x01bc, 0x60b7, 0x0000}, {0x04eb, 0x01cc, 0x60b9, 0xffff},
};
OamAttr data_ov006_02135ba8[44] = {
    {0x00dc, 0x41e5, 0x5014, 0x0000}, {0x80dc, 0x01f5, 0x5016, 0x0000},
    {0x40ec, 0x01e5, 0x5054, 0x0000}, {0x00ec, 0x01f5, 0x5056, 0x0000},
    {0x00e9, 0x41f2, 0x4084, 0x0000}, {0x80e9, 0x0002, 0x4086, 0x0000},
    {0x00cd, 0x81f2, 0x5005, 0x0000}, {0x40c0, 0x81f6, 0x4000, 0x0000},
    {0x80c0, 0x0016, 0x4004, 0x0000}, {0x00d0, 0x4006, 0x4042, 0x0000},
    {0x80d0, 0x0016, 0x4044, 0x0000}, {0x00dc, 0x0012, 0x5049, 0x0000},
    {0x00e2, 0x81f0, 0x4080, 0x0000}, {0x00e6, 0x41ed, 0x4040, 0x0000},
    {0x80f3, 0x01f3, 0x40d3, 0x0000}, {0x40fa, 0x000a, 0x50ab, 0x0000},
    {0x40e1, 0x81d2, 0x5050, 0x0000}, {0x40f1, 0x41d2, 0x5090, 0x0000},
    {0x00f3, 0x41e3, 0x40d1, 0x0000}, {0x00d5, 0x41b0, 0x60d7, 0x0000},
    {0x80d5, 0x01c0, 0x60d9, 0x0000}, {0x00d5, 0x41c8, 0x60ba, 0x0000},
    {0x40e5, 0x01c8, 0x60fa, 0x0000}, {0x40e5, 0x01b8, 0x60bc, 0x0000},
    {0x44c1, 0x4186, 0x6017, 0x0000}, {0x44c1, 0x01a6, 0x601b, 0x0000},
    {0x04c1, 0x01b6, 0x601d, 0x0000}, {0x44c9, 0x0186, 0x601e, 0x0000},
    {0x44c9, 0x4196, 0x6037, 0x0000}, {0x44c9, 0x01b6, 0x603b, 0x0000},
    {0x04c9, 0x01c6, 0x603d, 0x0000}, {0x44d1, 0x0186, 0x603e, 0x0000},
    {0x44d1, 0x4196, 0x6057, 0x0000}, {0x44d1, 0x41b6, 0x605b, 0x0000},
    {0x04d9, 0x0186, 0x605f, 0x0000}, {0x44d9, 0x418e, 0x6077, 0x0000},
    {0x04d9, 0x01ae, 0x607b, 0x0000}, {0x44d9, 0x01c6, 0x607c, 0x0000},
    {0x44e1, 0x018e, 0x607e, 0x0000}, {0x44e1, 0x419e, 0x6097, 0x0000},
    {0x04e1, 0x01ce, 0x609b, 0x0000}, {0x44e9, 0x419e, 0x609c, 0x0000},
    {0x44e9, 0x01be, 0x60b7, 0x0000}, {0x04e9, 0x01ce, 0x60b9, 0xffff},
};
OamAttr data_ov006_02135128[40] = {
    {0x00f2, 0x51fd, 0x50d4, 0x0000}, {0x80f2, 0x11f5, 0x50d6, 0x0000},
    {0x00e9, 0x51ed, 0x4089, 0x0000}, {0x40f8, 0x1006, 0x408b, 0x0000},
    {0x00ca, 0x91ee, 0x5005, 0x0000}, {0x40bd, 0x91ea, 0x4000, 0x0000},
    {0x80bd, 0x11e2, 0x4004, 0x0000}, {0x00cd, 0x51ea, 0x4042, 0x0000},
    {0x80cd, 0x11e2, 0x4044, 0x0000}, {0x00da, 0x11e6, 0x5049, 0x0000},
    {0x00e0, 0x91f0, 0x4080, 0x0000}, {0x40f9, 0x11ed, 0x40c9, 0x0000},
    {0x40e3, 0x9010, 0x5050, 0x0000}, {0x40f3, 0x5010, 0x5090, 0x0000},
    {0x00e5, 0x5003, 0x4040, 0x0000}, {0x00d7, 0x5042, 0x60d7, 0x0000},
    {0x80d7, 0x103a, 0x60d9, 0x0000}, {0x00d7, 0x502a, 0x60ba, 0x0000},
    {0x40e7, 0x102a, 0x60fa, 0x0000}, {0x40e7, 0x103a, 0x60bc, 0x0000},
    {0x44c3, 0x505c, 0x6017, 0x0000}, {0x44c3, 0x104c, 0x601b, 0x0000},
    {0x04c3, 0x1044, 0x601d, 0x0000}, {0x44cb, 0x106c, 0x601e, 0x0000},
    {0x44cb, 0x504c, 0x6037, 0x0000}, {0x44cb, 0x103c, 0x603b, 0x0000},
    {0x04cb, 0x1034, 0x603d, 0x0000}, {0x44d3, 0x106c, 0x603e, 0x0000},
    {0x44d3, 0x504c, 0x6057, 0x0000}, {0x44d3, 0x502c, 0x605b, 0x0000},
    {0x04db, 0x1074, 0x605f, 0x0000}, {0x44db, 0x5054, 0x6077, 0x0000},
    {0x04db, 0x104c, 0x607b, 0x0000}, {0x44db, 0x102c, 0x607c, 0x0000},
    {0x44e3, 0x1064, 0x607e, 0x0000}, {0x44e3, 0x5044, 0x6097, 0x0000},
    {0x04e3, 0x102c, 0x609b, 0x0000}, {0x44eb, 0x5044, 0x609c, 0x0000},
    {0x44eb, 0x1034, 0x60b7, 0x0000}, {0x04eb, 0x102c, 0x60b9, 0xffff},
};
OamAttr data_ov006_02135648[42] = {
    {0x00f2, 0x51f5, 0x5011, 0x0000}, {0x80f2, 0x11ed, 0x5013, 0x0000},
    {0x00ea, 0x51ef, 0x40c4, 0x0000}, {0x80ea, 0x11e7, 0x40c6, 0x0000},
    {0x40f2, 0x91fd, 0x40cd, 0x0000}, {0x00cd, 0x91ee, 0x5005, 0x0000},
    {0x40c0, 0x91ea, 0x4000, 0x0000}, {0x80c0, 0x11e2, 0x4004, 0x0000},
    {0x00d0, 0x51ea, 0x4042, 0x0000}, {0x80d0, 0x11e2, 0x4044, 0x0000},
    {0x00dc, 0x11e6, 0x5049, 0x0000}, {0x00e2, 0x91f0, 0x4080, 0x0000},
    {0x40e3, 0x900d, 0x5050, 0x0000}, {0x40f3, 0x500d, 0x5090, 0x0000},
    {0x00e5, 0x5000, 0x4040, 0x0000}, {0x40fa, 0x101d, 0x50eb, 0x0000},
    {0x40fa, 0x11e4, 0x50cb, 0x0000}, {0x00d7, 0x503f, 0x60d7, 0x0000},
    {0x80d7, 0x1037, 0x60d9, 0x0000}, {0x00d7, 0x5027, 0x60ba, 0x0000},
    {0x40e7, 0x1027, 0x60fa, 0x0000}, {0x40e7, 0x1037, 0x60bc, 0x0000},
    {0x44c3, 0x5059, 0x6017, 0x0000}, {0x44c3, 0x1049, 0x601b, 0x0000},
    {0x04c3, 0x1041, 0x601d, 0x0000}, {0x44cb, 0x1069, 0x601e, 0x0000},
    {0x44cb, 0x5049, 0x6037, 0x0000}, {0x44cb, 0x1039, 0x603b, 0x0000},
    {0x04cb, 0x1031, 0x603d, 0x0000}, {0x44d3, 0x1069, 0x603e, 0x0000},
    {0x44d3, 0x5049, 0x6057, 0x0000}, {0x44d3, 0x5029, 0x605b, 0x0000},
    {0x04db, 0x1071, 0x605f, 0x0000}, {0x44db, 0x5051, 0x6077, 0x0000},
    {0x04db, 0x1049, 0x607b, 0x0000}, {0x44db, 0x1029, 0x607c, 0x0000},
    {0x44e3, 0x1061, 0x607e, 0x0000}, {0x44e3, 0x5041, 0x6097, 0x0000},
    {0x04e3, 0x1029, 0x609b, 0x0000}, {0x44eb, 0x5041, 0x609c, 0x0000},
    {0x44eb, 0x1031, 0x60b7, 0x0000}, {0x04eb, 0x1029, 0x60b9, 0xffff},
};
OamAttr data_ov006_021353a8[42] = {
    {0x00e4, 0x51ff, 0x5014, 0x0000}, {0x40f4, 0x11ff, 0x5054, 0x0000},
    {0x00f4, 0x11f7, 0x5056, 0x0000}, {0x40e3, 0x9010, 0x5050, 0x0000},
    {0x80e4, 0x11f7, 0x5016, 0x0000}, {0x40f2, 0x11ed, 0x40c7, 0x0000},
    {0x00e2, 0x51ed, 0x4087, 0x0000}, {0x00ca, 0x91ee, 0x5005, 0x0000},
    {0x40f8, 0x51fa, 0x40e7, 0x0000}, {0x40bd, 0x91ea, 0x4000, 0x0000},
    {0x80bd, 0x11e2, 0x4004, 0x0000}, {0x00cd, 0x51ea, 0x4042, 0x0000},
    {0x80cd, 0x11e2, 0x4044, 0x0000}, {0x00da, 0x11e6, 0x5049, 0x0000},
    {0x00e0, 0x91f0, 0x4080, 0x0000}, {0x40f3, 0x5010, 0x5090, 0x0000},
    {0x00e5, 0x5003, 0x4040, 0x0000}, {0x00d7, 0x5042, 0x60d7, 0x0000},
    {0x80d7, 0x103a, 0x60d9, 0x0000}, {0x00d7, 0x502a, 0x60ba, 0x0000},
    {0x40e7, 0x102a, 0x60fa, 0x0000}, {0x40e7, 0x103a, 0x60bc, 0x0000},
    {0x44c3, 0x505c, 0x6017, 0x0000}, {0x44c3, 0x104c, 0x601b, 0x0000},
    {0x04c3, 0x1044, 0x601d, 0x0000}, {0x44cb, 0x106c, 0x601e, 0x0000},
    {0x44cb, 0x504c, 0x6037, 0x0000}, {0x44cb, 0x103c, 0x603b, 0x0000},
    {0x04cb, 0x1034, 0x603d, 0x0000}, {0x44d3, 0x106c, 0x603e, 0x0000},
    {0x44d3, 0x504c, 0x6057, 0x0000}, {0x44d3, 0x502c, 0x605b, 0x0000},
    {0x04db, 0x1074, 0x605f, 0x0000}, {0x44db, 0x5054, 0x6077, 0x0000},
    {0x04db, 0x104c, 0x607b, 0x0000}, {0x44db, 0x102c, 0x607c, 0x0000},
    {0x44e3, 0x1064, 0x607e, 0x0000}, {0x44e3, 0x5044, 0x6097, 0x0000},
    {0x04e3, 0x102c, 0x609b, 0x0000}, {0x44eb, 0x5044, 0x609c, 0x0000},
    {0x44eb, 0x1034, 0x60b7, 0x0000}, {0x04eb, 0x102c, 0x60b9, 0xffff},
};
OamAttr data_ov006_02135a48[44] = {
    {0x00dc, 0x500a, 0x5014, 0x0000}, {0x80dc, 0x1002, 0x5016, 0x0000},
    {0x40ec, 0x100a, 0x5054, 0x0000}, {0x00ec, 0x1002, 0x5056, 0x0000},
    {0x00e9, 0x51fd, 0x4084, 0x0000}, {0x80e9, 0x11f5, 0x4086, 0x0000},
    {0x00cd, 0x91ed, 0x5005, 0x0000}, {0x40c0, 0x91e9, 0x4000, 0x0000},
    {0x80c0, 0x11e1, 0x4004, 0x0000}, {0x00d0, 0x51e9, 0x4042, 0x0000},
    {0x80d0, 0x11e1, 0x4044, 0x0000}, {0x00dc, 0x11e5, 0x5049, 0x0000},
    {0x00e2, 0x91ef, 0x4080, 0x0000}, {0x00e6, 0x5002, 0x4040, 0x0000},
    {0x80f3, 0x1004, 0x40d3, 0x0000}, {0x40fa, 0x11e5, 0x50ab, 0x0000},
    {0x40e1, 0x900d, 0x5050, 0x0000}, {0x40f1, 0x500d, 0x5090, 0x0000},
    {0x00f3, 0x500c, 0x40d1, 0x0000}, {0x00d5, 0x503f, 0x60d7, 0x0000},
    {0x80d5, 0x1037, 0x60d9, 0x0000}, {0x00d5, 0x5027, 0x60ba, 0x0000},
    {0x40e5, 0x1027, 0x60fa, 0x0000}, {0x40e5, 0x1037, 0x60bc, 0x0000},
    {0x44c1, 0x5059, 0x6017, 0x0000}, {0x44c1, 0x1049, 0x601b, 0x0000},
    {0x04c1, 0x1041, 0x601d, 0x0000}, {0x44c9, 0x1069, 0x601e, 0x0000},
    {0x44c9, 0x5049, 0x6037, 0x0000}, {0x44c9, 0x1039, 0x603b, 0x0000},
    {0x04c9, 0x1031, 0x603d, 0x0000}, {0x44d1, 0x1069, 0x603e, 0x0000},
    {0x44d1, 0x5049, 0x6057, 0x0000}, {0x44d1, 0x5029, 0x605b, 0x0000},
    {0x04d9, 0x1071, 0x605f, 0x0000}, {0x44d9, 0x5051, 0x6077, 0x0000},
    {0x04d9, 0x1049, 0x607b, 0x0000}, {0x44d9, 0x1029, 0x607c, 0x0000},
    {0x44e1, 0x1061, 0x607e, 0x0000}, {0x44e1, 0x5041, 0x6097, 0x0000},
    {0x04e1, 0x1029, 0x609b, 0x0000}, {0x44e9, 0x5041, 0x609c, 0x0000},
    {0x44e9, 0x1031, 0x60b7, 0x0000}, {0x04e9, 0x1029, 0x60b9, 0xffff},
};
OamAttr data_ov006_02135d08[44] = {
    {0x00e5, 0x01e3, 0x606f, 0x0000}, {0x00e5, 0x11eb, 0x606f, 0x0000},
    {0x00ed, 0x31eb, 0x606f, 0x0000}, {0x00ed, 0x21e3, 0x606f, 0x0000},
    {0x04d5, 0x41db, 0x60be, 0x0000}, {0x44e5, 0x01db, 0x60fe, 0x0000},
    {0x84dd, 0x01d3, 0x60dd, 0x0000}, {0x04d5, 0x51eb, 0x60be, 0x0000},
    {0x44e5, 0x11eb, 0x60fe, 0x0000}, {0x84dd, 0x11fb, 0x60dd, 0x0000},
    {0x04f5, 0x61db, 0x60be, 0x0000}, {0x44ed, 0x21db, 0x60fe, 0x0000},
    {0x84ed, 0x21d3, 0x60dd, 0x0000}, {0x04f5, 0x71eb, 0x60be, 0x0000},
    {0x44ed, 0x31eb, 0x60fe, 0x0000}, {0x84ed, 0x31fb, 0x60dd, 0x0000},
    {0x40ed, 0x0012, 0x70ce, 0x0000}, {0x00ed, 0x0022, 0x70d0, 0x0000},
    {0x00f5, 0x0022, 0x70f0, 0x0000}, {0x00fd, 0x0012, 0x70b0, 0x0000},
    {0x40f5, 0x0012, 0x7069, 0x0000}, {0x00e0, 0x41e3, 0x700f, 0x0000},
    {0x00f0, 0x01e3, 0x704f, 0x0000}, {0x00f0, 0x11eb, 0x704f, 0x0000},
    {0x00be, 0x41f0, 0x408d, 0x0000}, {0x80be, 0x0000, 0x408f, 0x0000},
    {0x00ce, 0x01f0, 0x40cd, 0x0000}, {0x80be, 0x1008, 0x408d, 0x0000},
    {0x00ce, 0x1008, 0x40cd, 0x0000}, {0x00c5, 0x81f0, 0x700b, 0x0000},
    {0x00e5, 0x400b, 0x4009, 0x0000}, {0x00e1, 0x41ef, 0x4074, 0x0000},
    {0x80e1, 0x01ff, 0x4076, 0x0000}, {0x40f1, 0x01ef, 0x40b4, 0x0000},
    {0x00f1, 0x11ff, 0x40b5, 0x0000}, {0x00f1, 0x0007, 0x40b6, 0x0000},
    {0x80e1, 0x0007, 0x40dc, 0x0000}, {0x00c8, 0x0000, 0x5032, 0x0000},
    {0x00c8, 0x11f8, 0x5032, 0x0000}, {0x00c0, 0x31f8, 0x5032, 0x0000},
    {0x00c0, 0x2000, 0x5032, 0x0000}, {0x40f9, 0x01ef, 0x40b1, 0x0000},
    {0x00f9, 0x0007, 0x40b3, 0x0000}, {0x00f9, 0x11ff, 0x40b2, 0xffff},
};
OamAttr data_ov006_021358e8[44] = {
    {0x00e4, 0x01e3, 0x606f, 0x0000}, {0x00e4, 0x11eb, 0x606f, 0x0000},
    {0x00ec, 0x31eb, 0x606f, 0x0000}, {0x00ec, 0x21e3, 0x606f, 0x0000},
    {0x04d4, 0x41db, 0x60be, 0x0000}, {0x44e4, 0x01db, 0x60fe, 0x0000},
    {0x84dc, 0x01d3, 0x60dd, 0x0000}, {0x04d4, 0x51eb, 0x60be, 0x0000},
    {0x44e4, 0x11eb, 0x60fe, 0x0000}, {0x84dc, 0x11fb, 0x60dd, 0x0000},
    {0x04f4, 0x61db, 0x60be, 0x0000}, {0x44ec, 0x21db, 0x60fe, 0x0000},
    {0x84ec, 0x21d3, 0x60dd, 0x0000}, {0x04f4, 0x71eb, 0x60be, 0x0000},
    {0x44ec, 0x31eb, 0x60fe, 0x0000}, {0x84ec, 0x31fb, 0x60dd, 0x0000},
    {0x40ec, 0x0012, 0x70ce, 0x0000}, {0x00ec, 0x0022, 0x70d0, 0x0000},
    {0x00f4, 0x0022, 0x70f0, 0x0000}, {0x00fc, 0x0012, 0x70b0, 0x0000},
    {0x40f4, 0x0012, 0x7069, 0x0000}, {0x00df, 0x41e3, 0x700f, 0x0000},
    {0x00ef, 0x01e3, 0x704f, 0x0000}, {0x00ef, 0x11eb, 0x704f, 0x0000},
    {0x00bc, 0x41f0, 0x408d, 0x0000}, {0x80bc, 0x0000, 0x408f, 0x0000},
    {0x00cc, 0x01f0, 0x40cd, 0x0000}, {0x80bc, 0x1008, 0x408d, 0x0000},
    {0x00cc, 0x1008, 0x40cd, 0x0000}, {0x00c3, 0x81f0, 0x700b, 0x0000},
    {0x00e4, 0x400b, 0x4009, 0x0000}, {0x00e0, 0x41ef, 0x4074, 0x0000},
    {0x80e0, 0x01ff, 0x4076, 0x0000}, {0x40f0, 0x01ef, 0x40b4, 0x0000},
    {0x00f0, 0x11ff, 0x40b5, 0x0000}, {0x00f0, 0x0007, 0x40b6, 0x0000},
    {0x80e0, 0x0007, 0x40dc, 0x0000}, {0x00c6, 0x0000, 0x5032, 0x0000},
    {0x00c6, 0x11f8, 0x5032, 0x0000}, {0x00be, 0x31f8, 0x5032, 0x0000},
    {0x00be, 0x2000, 0x5032, 0x0000}, {0x40f8, 0x01ef, 0x40b1, 0x0000},
    {0x00f8, 0x0007, 0x40b3, 0x0000}, {0x00f8, 0x11ff, 0x40b2, 0xffff},
};
OamAttr data_ov006_02135e68[44] = {
    {0x00e4, 0x01e3, 0x606f, 0x0000}, {0x00e4, 0x11eb, 0x606f, 0x0000},
    {0x00ec, 0x31eb, 0x606f, 0x0000}, {0x00ec, 0x21e3, 0x606f, 0x0000},
    {0x04d4, 0x41db, 0x60be, 0x0000}, {0x44e4, 0x01db, 0x60fe, 0x0000},
    {0x84dc, 0x01d3, 0x60dd, 0x0000}, {0x04d4, 0x51eb, 0x60be, 0x0000},
    {0x44e4, 0x11eb, 0x60fe, 0x0000}, {0x84dc, 0x11fb, 0x60dd, 0x0000},
    {0x04f4, 0x61db, 0x60be, 0x0000}, {0x44ec, 0x21db, 0x60fe, 0x0000},
    {0x84ec, 0x21d3, 0x60dd, 0x0000}, {0x04f4, 0x71eb, 0x60be, 0x0000},
    {0x44ec, 0x31eb, 0x60fe, 0x0000}, {0x84ec, 0x31fb, 0x60dd, 0x0000},
    {0x40ec, 0x0012, 0x70ce, 0x0000}, {0x00ec, 0x0022, 0x70d0, 0x0000},
    {0x00f4, 0x0022, 0x70f0, 0x0000}, {0x00fc, 0x0012, 0x70b0, 0x0000},
    {0x40f4, 0x0012, 0x7069, 0x0000}, {0x00df, 0x41e3, 0x700f, 0x0000},
    {0x00ef, 0x01e3, 0x704f, 0x0000}, {0x00ef, 0x11eb, 0x704f, 0x0000},
    {0x00bb, 0x41f0, 0x408d, 0x0000}, {0x80bb, 0x0000, 0x408f, 0x0000},
    {0x00cb, 0x01f0, 0x40cd, 0x0000}, {0x80bb, 0x1008, 0x408d, 0x0000},
    {0x00cb, 0x1008, 0x40cd, 0x0000}, {0x00c2, 0x81f0, 0x700b, 0x0000},
    {0x00e4, 0x400b, 0x4009, 0x0000}, {0x00e0, 0x41ef, 0x4074, 0x0000},
    {0x80e0, 0x01ff, 0x4076, 0x0000}, {0x40f0, 0x01ef, 0x40b4, 0x0000},
    {0x00f0, 0x11ff, 0x40b5, 0x0000}, {0x00f0, 0x0007, 0x40b6, 0x0000},
    {0x80e0, 0x0007, 0x40dc, 0x0000}, {0x00c5, 0x0000, 0x5032, 0x0000},
    {0x00c5, 0x11f8, 0x5032, 0x0000}, {0x00bd, 0x31f8, 0x5032, 0x0000},
    {0x00bd, 0x2000, 0x5032, 0x0000}, {0x40f8, 0x01ef, 0x40b1, 0x0000},
    {0x00f8, 0x0007, 0x40b3, 0x0000}, {0x00f8, 0x11ff, 0x40b2, 0xffff},
};
OamAttr *sTeresaBigBooSprites[] = {
    data_ov006_02135268, data_ov006_021354f8, data_ov006_02135798, data_ov006_02135ba8,
    data_ov006_02135128, data_ov006_02135648, data_ov006_021353a8, data_ov006_02135a48,
    data_ov006_02135d08, data_ov006_021358e8, data_ov006_02135e68,
};
extern unsigned char sTeresaBigBooHoverPicks[];
extern unsigned short sTeresaBigBooFrameTimes[];
extern unsigned char sTeresaRevealSlideFrames[];
extern unsigned char sTeresaRevealAnimFrames[];
extern unsigned short sTeresaBg3FlashTimes[];
OamAttr data_ov006_02133a60[2] = {
    {0x80f4, 0x01fc, 0x01ad, 0x0000},
    {0x0004, 0x01fc, 0x01ed, 0xffff},
};
OamAttr *sTeresaExclaimSprites = data_ov006_02133a60;
OamAttr data_ov006_021341f8[1] = {
    {0x00f8, 0x41f8, 0x11c9, 0xffff},
};
OamAttr data_ov006_0213424c[5] = {
    {0x00ee, 0x41f8, 0x110b, 0x0000}, {0x00fb, 0x51f0, 0x110b, 0x0000},
    {0x00fb, 0x61fe, 0x110b, 0x0000}, {0x00f6, 0x01f5, 0x11ee, 0x0000},
    {0x00f7, 0x0001, 0x11ee, 0xffff},
};
OamAttr *sTeresaVanishPuffSprites[] = {data_ov006_021341f8, data_ov006_0213424c};
OamAttr data_ov006_0213635c[57] = {
    {0x84d5, 0x01e4, 0x60d7, 0x0000}, {0x84d5, 0x11ec, 0x60d7, 0x0000},
    {0x04e5, 0x01e4, 0x60f7, 0x0000}, {0x04e5, 0x11ec, 0x60f7, 0x0000},
    {0x8405, 0x21e4, 0x60d7, 0x0000}, {0x8405, 0x31ec, 0x60d7, 0x0000},
    {0x04fd, 0x21e4, 0x60f7, 0x0000}, {0x04fd, 0x31ec, 0x60f7, 0x0000},
    {0x44ed, 0x01fc, 0x60f8, 0x0000}, {0x44f5, 0x21fc, 0x60f8, 0x0000},
    {0x04ed, 0x01f4, 0x60f8, 0x0000}, {0x04f5, 0x21f4, 0x60f8, 0x0000},
    {0x44ed, 0x11cc, 0x60f8, 0x0000}, {0x44f5, 0x31cc, 0x60f8, 0x0000},
    {0x04ed, 0x11dc, 0x60f8, 0x0000}, {0x04f5, 0x31dc, 0x60f8, 0x0000},
    {0x04dd, 0x41d4, 0x60bd, 0x0000}, {0x84dd, 0x01e4, 0x60bf, 0x0000},
    {0x44ed, 0x01d4, 0x60fd, 0x0000}, {0x04ed, 0x01e4, 0x60ff, 0x0000},
    {0x04dd, 0x51f4, 0x60bd, 0x0000}, {0x84dd, 0x11ec, 0x60bf, 0x0000},
    {0x44ed, 0x11f4, 0x60fd, 0x0000}, {0x04ed, 0x11ec, 0x60ff, 0x0000},
    {0x04fd, 0x61d4, 0x60bd, 0x0000}, {0x84fd, 0x21e4, 0x60bf, 0x0000},
    {0x44f5, 0x21d4, 0x60fd, 0x0000}, {0x04f5, 0x21e4, 0x60ff, 0x0000},
    {0x04fd, 0x71f4, 0x60bd, 0x0000}, {0x84fd, 0x31ec, 0x60bf, 0x0000},
    {0x44f5, 0x31f4, 0x60fd, 0x0000}, {0x04f5, 0x31ec, 0x60ff, 0x0000},
    {0x00ed, 0x01e4, 0x60dc, 0x0000}, {0x00ed, 0x11ec, 0x60dc, 0x0000},
    {0x00f5, 0x21e4, 0x60dc, 0x0000}, {0x00f5, 0x31ec, 0x60dc, 0x0000},
    {0x00e8, 0x41e4, 0x70cb, 0x0000}, {0x80e8, 0x01f4, 0x70cd, 0x0000},
    {0x00f8, 0x01e4, 0x704c, 0x0000}, {0x00f8, 0x11ec, 0x704c, 0x0000},
    {0x00d1, 0x4016, 0x700a, 0x0000}, {0x80d1, 0x0026, 0x700c, 0x0000},
    {0x40e1, 0x0016, 0x704a, 0x0000}, {0x40e9, 0x81f1, 0x40b2, 0x0000},
    {0x80e9, 0x0011, 0x40b6, 0x0000}, {0x40f9, 0x41f1, 0x40f2, 0x0000},
    {0x00f9, 0x0011, 0x40f6, 0x0000}, {0x40e1, 0x41f1, 0x4092, 0x0000},
    {0x00d9, 0x0001, 0x4074, 0x0000}, {0x00ba, 0x41f0, 0x4014, 0x0000},
    {0x80ba, 0x0000, 0x4016, 0x0000}, {0x00ca, 0x01f0, 0x4054, 0x0000},
    {0x80ba, 0x1008, 0x4014, 0x0000}, {0x00ca, 0x1008, 0x4054, 0x0000},
    {0x00c3, 0x81f0, 0x7087, 0x0000}, {0x00db, 0x4009, 0x4055, 0x0000},
    {0x40bc, 0x01fb, 0x7085, 0xffff},
};
OamAttr data_ov006_02136524[65] = {
    {0x84cc, 0x01e4, 0x60d7, 0x0000}, {0x84cc, 0x11ec, 0x60d7, 0x0000},
    {0x04dc, 0x01e4, 0x60f7, 0x0000}, {0x04dc, 0x11ec, 0x60f7, 0x0000},
    {0x840c, 0x21e4, 0x60d7, 0x0000}, {0x840c, 0x31ec, 0x60d7, 0x0000},
    {0x0404, 0x21e4, 0x60f7, 0x0000}, {0x0404, 0x31ec, 0x60f7, 0x0000},
    {0x44ec, 0x0004, 0x60f8, 0x0000}, {0x44f4, 0x2004, 0x60f8, 0x0000},
    {0x04ec, 0x01fc, 0x60f8, 0x0000}, {0x04f4, 0x21fc, 0x60f8, 0x0000},
    {0x44ec, 0x11c4, 0x60f8, 0x0000}, {0x44f4, 0x31c4, 0x60f8, 0x0000},
    {0x04ec, 0x11d4, 0x60f8, 0x0000}, {0x04f4, 0x31d4, 0x60f8, 0x0000},
    {0x04e4, 0x01e4, 0x60f7, 0x0000}, {0x04e4, 0x11ec, 0x60f7, 0x0000},
    {0x04ec, 0x11dc, 0x60f8, 0x0000}, {0x04f4, 0x31dc, 0x60f8, 0x0000},
    {0x04ec, 0x01f4, 0x60f8, 0x0000}, {0x04f4, 0x21f4, 0x60f8, 0x0000},
    {0x04fc, 0x21e4, 0x60f7, 0x0000}, {0x04fc, 0x31ec, 0x60f7, 0x0000},
    {0x04dc, 0x41d4, 0x60bd, 0x0000}, {0x84dc, 0x01e4, 0x60bf, 0x0000},
    {0x44ec, 0x01d4, 0x60fd, 0x0000}, {0x04ec, 0x01e4, 0x60ff, 0x0000},
    {0x04dc, 0x51f4, 0x60bd, 0x0000}, {0x84dc, 0x11ec, 0x60bf, 0x0000},
    {0x44ec, 0x11f4, 0x60fd, 0x0000}, {0x04ec, 0x11ec, 0x60ff, 0x0000},
    {0x04fc, 0x61d4, 0x60bd, 0x0000}, {0x84fc, 0x21e4, 0x60bf, 0x0000},
    {0x44f4, 0x21d4, 0x60fd, 0x0000}, {0x04f4, 0x21e4, 0x60ff, 0x0000},
    {0x04fc, 0x71f4, 0x60bd, 0x0000}, {0x84fc, 0x31ec, 0x60bf, 0x0000},
    {0x44f4, 0x31f4, 0x60fd, 0x0000}, {0x04f4, 0x31ec, 0x60ff, 0x0000},
    {0x00ec, 0x01e4, 0x60dc, 0x0000}, {0x00ec, 0x11ec, 0x60dc, 0x0000},
    {0x00f4, 0x21e4, 0x60dc, 0x0000}, {0x00f4, 0x31ec, 0x60dc, 0x0000},
    {0x00e7, 0x41e4, 0x70cb, 0x0000}, {0x80e7, 0x01f4, 0x70cd, 0x0000},
    {0x00f7, 0x01e4, 0x704c, 0x0000}, {0x00f7, 0x11ec, 0x704c, 0x0000},
    {0x00d0, 0x4016, 0x700a, 0x0000}, {0x80d0, 0x0026, 0x700c, 0x0000},
    {0x40e0, 0x0016, 0x704a, 0x0000}, {0x40e8, 0x81f1, 0x40b2, 0x0000},
    {0x80e8, 0x0011, 0x40b6, 0x0000}, {0x40f8, 0x41f1, 0x40f2, 0x0000},
    {0x00f8, 0x0011, 0x40f6, 0x0000}, {0x40e0, 0x41f1, 0x4092, 0x0000},
    {0x00d8, 0x0001, 0x4074, 0x0000}, {0x00b9, 0x41f0, 0x4014, 0x0000},
    {0x80b9, 0x0000, 0x4016, 0x0000}, {0x00c9, 0x01f0, 0x4054, 0x0000},
    {0x80b9, 0x1008, 0x4014, 0x0000}, {0x00c9, 0x1008, 0x4054, 0x0000},
    {0x00c2, 0x81f0, 0x7087, 0x0000}, {0x00da, 0x4009, 0x4055, 0x0000},
    {0x40bb, 0x01fb, 0x7085, 0xffff},
};
OamAttr data_ov006_0213672c[73] = {
    {0x84c4, 0x01e4, 0x60d7, 0x0000}, {0x84c4, 0x11ec, 0x60d7, 0x0000},
    {0x04d4, 0x01e4, 0x60f7, 0x0000}, {0x04d4, 0x11ec, 0x60f7, 0x0000},
    {0x8414, 0x21e4, 0x60d7, 0x0000}, {0x8414, 0x31ec, 0x60d7, 0x0000},
    {0x040c, 0x21e4, 0x60f7, 0x0000}, {0x040c, 0x31ec, 0x60f7, 0x0000},
    {0x44ec, 0x000c, 0x60f8, 0x0000}, {0x44f4, 0x200c, 0x60f8, 0x0000},
    {0x04ec, 0x0004, 0x60f8, 0x0000}, {0x04f4, 0x2004, 0x60f8, 0x0000},
    {0x44ec, 0x11bc, 0x60f8, 0x0000}, {0x44f4, 0x31bc, 0x60f8, 0x0000},
    {0x04ec, 0x11cc, 0x60f8, 0x0000}, {0x04f4, 0x31cc, 0x60f8, 0x0000},
    {0x04dc, 0x01e4, 0x60f7, 0x0000}, {0x04dc, 0x11ec, 0x60f7, 0x0000},
    {0x04ec, 0x11d4, 0x60f8, 0x0000}, {0x04f4, 0x31d4, 0x60f8, 0x0000},
    {0x04ec, 0x01fc, 0x60f8, 0x0000}, {0x04f4, 0x21fc, 0x60f8, 0x0000},
    {0x0404, 0x21e4, 0x60f7, 0x0000}, {0x0404, 0x31ec, 0x60f7, 0x0000},
    {0x04e4, 0x01e4, 0x60f7, 0x0000}, {0x04e4, 0x11ec, 0x60f7, 0x0000},
    {0x04ec, 0x11dc, 0x60f8, 0x0000}, {0x04f4, 0x31dc, 0x60f8, 0x0000},
    {0x04ec, 0x01f4, 0x60f8, 0x0000}, {0x04f4, 0x21f4, 0x60f8, 0x0000},
    {0x04fc, 0x21e4, 0x60f7, 0x0000}, {0x04fc, 0x31ec, 0x60f7, 0x0000},
    {0x04dc, 0x41d4, 0x60bd, 0x0000}, {0x84dc, 0x01e4, 0x60bf, 0x0000},
    {0x44ec, 0x01d4, 0x60fd, 0x0000}, {0x04ec, 0x01e4, 0x60ff, 0x0000},
    {0x04dc, 0x51f4, 0x60bd, 0x0000}, {0x84dc, 0x11ec, 0x60bf, 0x0000},
    {0x44ec, 0x11f4, 0x60fd, 0x0000}, {0x04ec, 0x11ec, 0x60ff, 0x0000},
    {0x04fc, 0x61d4, 0x60bd, 0x0000}, {0x84fc, 0x21e4, 0x60bf, 0x0000},
    {0x44f4, 0x21d4, 0x60fd, 0x0000}, {0x04f4, 0x21e4, 0x60ff, 0x0000},
    {0x04fc, 0x71f4, 0x60bd, 0x0000}, {0x84fc, 0x31ec, 0x60bf, 0x0000},
    {0x44f4, 0x31f4, 0x60fd, 0x0000}, {0x04f4, 0x31ec, 0x60ff, 0x0000},
    {0x00ec, 0x01e4, 0x60dc, 0x0000}, {0x00ec, 0x11ec, 0x60dc, 0x0000},
    {0x00f4, 0x21e4, 0x60dc, 0x0000}, {0x00f4, 0x31ec, 0x60dc, 0x0000},
    {0x00cc, 0x4014, 0x700a, 0x0000}, {0x80cc, 0x0024, 0x700c, 0x0000},
    {0x40dc, 0x0014, 0x704a, 0x0000}, {0x00e7, 0x41e4, 0x70cb, 0x0000},
    {0x80e7, 0x01f4, 0x70cd, 0x0000}, {0x00f7, 0x01e4, 0x704c, 0x0000},
    {0x00f7, 0x11ec, 0x704c, 0x0000}, {0x40e8, 0x81f1, 0x40b2, 0x0000},
    {0x80e8, 0x0011, 0x40b6, 0x0000}, {0x40f8, 0x41f1, 0x40f2, 0x0000},
    {0x00f8, 0x0011, 0x40f6, 0x0000}, {0x40e0, 0x41f1, 0x4092, 0x0000},
    {0x00d8, 0x0001, 0x4074, 0x0000}, {0x00b8, 0x41f0, 0x4014, 0x0000},
    {0x80b8, 0x0000, 0x4016, 0x0000}, {0x00c8, 0x01f0, 0x4054, 0x0000},
    {0x80b8, 0x1008, 0x4014, 0x0000}, {0x00c8, 0x1008, 0x4054, 0x0000},
    {0x00c1, 0x81f0, 0x7087, 0x0000}, {0x40ba, 0x01fb, 0x7085, 0x0000},
    {0x00d7, 0x4008, 0x4052, 0xffff},
};
OamAttr data_ov006_0213620c[42] = {
    {0x04dc, 0x41d4, 0x60bd, 0x0000}, {0x84dc, 0x01e4, 0x60bf, 0x0000},
    {0x44ec, 0x01d4, 0x60fd, 0x0000}, {0x04ec, 0x01e4, 0x60ff, 0x0000},
    {0x04dc, 0x51f4, 0x60bd, 0x0000}, {0x84dc, 0x11ec, 0x60bf, 0x0000},
    {0x44ec, 0x11f4, 0x60fd, 0x0000}, {0x04ec, 0x11ec, 0x60ff, 0x0000},
    {0x04fc, 0x61d4, 0x60bd, 0x0000}, {0x84fc, 0x21e4, 0x60bf, 0x0000},
    {0x44f4, 0x21d4, 0x60fd, 0x0000}, {0x04f4, 0x21e4, 0x60ff, 0x0000},
    {0x04fc, 0x71f4, 0x60bd, 0x0000}, {0x84fc, 0x31ec, 0x60bf, 0x0000},
    {0x44f4, 0x31f4, 0x60fd, 0x0000}, {0x04f4, 0x31ec, 0x60ff, 0x0000},
    {0x00ec, 0x01e4, 0x60dc, 0x0000}, {0x00ec, 0x11ec, 0x60dc, 0x0000},
    {0x00f4, 0x21e4, 0x60dc, 0x0000}, {0x00f4, 0x31ec, 0x60dc, 0x0000},
    {0x00c7, 0x4013, 0x706b, 0x0000}, {0x80c7, 0x0023, 0x706d, 0x0000},
    {0x40d7, 0x0013, 0x70ab, 0x0000}, {0x00d7, 0x0023, 0x70ad, 0x0000},
    {0x00e7, 0x41e4, 0x70cb, 0x0000}, {0x80e7, 0x01f4, 0x70cd, 0x0000},
    {0x00f7, 0x01e4, 0x704c, 0x0000}, {0x00f7, 0x11ec, 0x704c, 0x0000},
    {0x40e8, 0x81f1, 0x40b2, 0x0000}, {0x80e8, 0x0011, 0x40b6, 0x0000},
    {0x40f8, 0x41f1, 0x40f2, 0x0000}, {0x00f8, 0x0011, 0x40f6, 0x0000},
    {0x40e0, 0x41f1, 0x4092, 0x0000}, {0x00d8, 0x0001, 0x4074, 0x0000},
    {0x00b8, 0x41f0, 0x4014, 0x0000}, {0x80b8, 0x0000, 0x4016, 0x0000},
    {0x00c8, 0x01f0, 0x4054, 0x0000}, {0x80b8, 0x1008, 0x4014, 0x0000},
    {0x00c8, 0x1008, 0x4054, 0x0000}, {0x00c1, 0x81f0, 0x7087, 0x0000},
    {0x40ba, 0x01fb, 0x7085, 0x0000}, {0x00d6, 0x4008, 0x4052, 0xffff},
};
OamAttr data_ov006_02135fe4[23] = {
    {0x00e4, 0x41e5, 0x7008, 0x0000}, {0x40f4, 0x01e5, 0x7048, 0x0000},
    {0x00f0, 0x4000, 0x7005, 0x0000}, {0x80f0, 0x0010, 0x7007, 0x0000},
    {0x80d9, 0x0008, 0x400d, 0x0000}, {0x00e9, 0x0008, 0x404d, 0x0000},
    {0x00e9, 0x0010, 0x404e, 0x0000}, {0x00c1, 0x41f0, 0x4011, 0x0000},
    {0x80c1, 0x0000, 0x4013, 0x0000}, {0x00b9, 0x01f8, 0x4030, 0x0000},
    {0x40d1, 0x01f0, 0x4050, 0x0000}, {0x80c1, 0x1008, 0x4011, 0x0000},
    {0x40d1, 0x1000, 0x4050, 0x0000}, {0x00b9, 0x1000, 0x4030, 0x0000},
    {0x40c8, 0x01fb, 0x7085, 0x0000}, {0x40f8, 0x41ed, 0x40ee, 0x0000},
    {0x00df, 0x41e8, 0x400e, 0x0000}, {0x40d7, 0x01f6, 0x7045, 0x0000},
    {0x00d7, 0x0006, 0x7047, 0x0000}, {0x40df, 0x01f6, 0x7065, 0x0000},
    {0x00cc, 0x0008, 0x706a, 0x0000}, {0x00d8, 0x81ed, 0x406e, 0x0000},
    {0x00cc, 0x11f0, 0x706a, 0xffff},
};
OamAttr data_ov006_02136154[23] = {
    {0x00e4, 0x41e5, 0x7008, 0x0000}, {0x40f4, 0x01e5, 0x7048, 0x0000},
    {0x00f0, 0x4000, 0x7005, 0x0000}, {0x80f0, 0x0010, 0x7007, 0x0000},
    {0x80d9, 0x0008, 0x400d, 0x0000}, {0x00e9, 0x0008, 0x404d, 0x0000},
    {0x00e9, 0x0010, 0x404e, 0x0000}, {0x00c2, 0x41f0, 0x4011, 0x0000},
    {0x80c2, 0x0000, 0x4013, 0x0000}, {0x00ba, 0x01f8, 0x4030, 0x0000},
    {0x40d2, 0x01f0, 0x4050, 0x0000}, {0x80c2, 0x1008, 0x4011, 0x0000},
    {0x40d2, 0x1000, 0x4050, 0x0000}, {0x00ba, 0x1000, 0x4030, 0x0000},
    {0x40c9, 0x01fb, 0x7085, 0x0000}, {0x40f8, 0x41ed, 0x40ee, 0x0000},
    {0x00df, 0x41e8, 0x400e, 0x0000}, {0x40d8, 0x01f6, 0x7045, 0x0000},
    {0x00d8, 0x0006, 0x7047, 0x0000}, {0x40e0, 0x01f6, 0x7065, 0x0000},
    {0x00cd, 0x0008, 0x706a, 0x0000}, {0x00d8, 0x81ed, 0x406e, 0x0000},
    {0x00cd, 0x11f0, 0x706a, 0xffff},
};
OamAttr data_ov006_0213609c[23] = {
    {0x00e6, 0x41e5, 0x7008, 0x0000}, {0x40f5, 0x01e5, 0x7048, 0x0000},
    {0x00f2, 0x4000, 0x7005, 0x0000}, {0x80f2, 0x0010, 0x7007, 0x0000},
    {0x80db, 0x0008, 0x400d, 0x0000}, {0x00eb, 0x0008, 0x404d, 0x0000},
    {0x00eb, 0x0010, 0x404e, 0x0000}, {0x00c4, 0x41f0, 0x4011, 0x0000},
    {0x80c4, 0x0000, 0x4013, 0x0000}, {0x40d3, 0x01f0, 0x4050, 0x0000},
    {0x80c4, 0x1008, 0x4011, 0x0000}, {0x40d3, 0x1000, 0x4050, 0x0000},
    {0x40cb, 0x01fb, 0x7085, 0x0000}, {0x40f8, 0x41ed, 0x40ee, 0x0000},
    {0x00e1, 0x41e8, 0x400e, 0x0000}, {0x40d9, 0x01f6, 0x7045, 0x0000},
    {0x00d9, 0x0006, 0x7047, 0x0000}, {0x40e1, 0x01f6, 0x7065, 0x0000},
    {0x00ce, 0x0008, 0x706a, 0x0000}, {0x00d8, 0x81ed, 0x406e, 0x0000},
    {0x00ce, 0x11f0, 0x706a, 0x0000}, {0x00bc, 0x01f8, 0x4030, 0x0000},
    {0x00bc, 0x1000, 0x4030, 0xffff},
};
OamAttr *sTeresaBigBooHoverSprites[] = {
    data_ov006_0213635c, data_ov006_02136524, data_ov006_0213672c, data_ov006_0213620c,
    data_ov006_02135fe4, data_ov006_02136154, data_ov006_0213609c,
};
extern OamAttr *sMgDigitSprites[];
OamAttr data_ov006_0213a5f8[6] = {
    {0x8008, 0x81f8, 0x2149, 0x0000}, {0x8028, 0x81f8, 0x214b, 0x0000},
    {0x0048, 0x41f8, 0x21cb, 0x0000}, {0x80e8, 0x81f8, 0x2149, 0x0000},
    {0x80c8, 0x81f8, 0x2149, 0x0000}, {0x80a8, 0x81f8, 0x2149, 0xffff},
};
void *sTeresaMarkerSprites = data_ov006_0213a5f8;
OamAttr data_ov006_0213a744[11] = {
    {0x00f6, 0x4023, 0x3100, 0x0000}, {0x80f6, 0x0033, 0x3102, 0x0000},
    {0x4006, 0x0023, 0x3140, 0x0000}, {0x0006, 0x0033, 0x3142, 0x0000},
    {0x00da, 0xc1d8, 0x3116, 0x0000}, {0x401a, 0x81d8, 0x310d, 0x0000},
    {0x401a, 0x81f8, 0x3111, 0x0000}, {0x80fa, 0x01d2, 0x31d4, 0x0000},
    {0x80da, 0x8018, 0x311e, 0x0000}, {0x80fa, 0x8018, 0x319e, 0x0000},
    {0x001a, 0x4018, 0x3115, 0xffff},
};
OamAttr data_ov006_0213a6ec[11] = {
    {0x00f5, 0x4023, 0x3160, 0x0000}, {0x80f5, 0x0033, 0x3162, 0x0000},
    {0x4005, 0x0023, 0x31a0, 0x0000}, {0x0005, 0x0033, 0x31a2, 0x0000},
    {0x00d9, 0xc1d8, 0x3116, 0x0000}, {0x80d9, 0x8018, 0x311e, 0x0000},
    {0x80f9, 0x8018, 0x319e, 0x0000}, {0x4019, 0x81d8, 0x310d, 0x0000},
    {0x4019, 0x81f8, 0x3111, 0x0000}, {0x0019, 0x4018, 0x3115, 0x0000},
    {0x80fb, 0x01d2, 0x3167, 0xffff},
};
OamAttr data_ov006_0213a694[11] = {
    {0x00f4, 0x4023, 0x3103, 0x0000}, {0x80f4, 0x0033, 0x3105, 0x0000},
    {0x4004, 0x0023, 0x3143, 0x0000}, {0x0004, 0x0033, 0x3145, 0x0000},
    {0x00d8, 0xc1d8, 0x3116, 0x0000}, {0x80d8, 0x8018, 0x311e, 0x0000},
    {0x80f8, 0x8018, 0x319e, 0x0000}, {0x4018, 0x81d8, 0x310d, 0x0000},
    {0x4018, 0x81f8, 0x3111, 0x0000}, {0x0018, 0x4018, 0x3115, 0x0000},
    {0x80f9, 0x01d2, 0x31d5, 0xffff},
};
OamAttr data_ov006_0213a63c[11] = {
    {0x00f3, 0x4023, 0x3163, 0x0000}, {0x80f3, 0x0033, 0x3165, 0x0000},
    {0x4003, 0x0023, 0x31a3, 0x0000}, {0x0003, 0x0033, 0x31a5, 0x0000},
    {0x00d7, 0xc1d8, 0x3116, 0x0000}, {0x80d7, 0x8018, 0x311e, 0x0000},
    {0x80f7, 0x8018, 0x319e, 0x0000}, {0x4017, 0x81d8, 0x310d, 0x0000},
    {0x4017, 0x81f8, 0x3111, 0x0000}, {0x0017, 0x4018, 0x3115, 0x0000},
    {0x80f9, 0x21d2, 0x3167, 0xffff},
};
OamAttr data_ov006_0213a79c[11] = {
    {0x00f1, 0x4023, 0x3106, 0x0000}, {0x80f1, 0x0033, 0x3108, 0x0000},
    {0x4001, 0x0023, 0x3146, 0x0000}, {0x0001, 0x0033, 0x3148, 0x0000},
    {0x00d6, 0xc1d8, 0x3116, 0x0000}, {0x4016, 0x81d8, 0x310d, 0x0000},
    {0x4016, 0x81f8, 0x3111, 0x0000}, {0x80f9, 0x21d2, 0x31d4, 0x0000},
    {0x80d6, 0x8018, 0x311e, 0x0000}, {0x80f6, 0x8018, 0x319e, 0x0000},
    {0x0016, 0x4018, 0x3115, 0xffff},
};
struct OamAttr *sTeresaRevealSprites[] = {
    data_ov006_0213a744, data_ov006_0213a6ec, data_ov006_0213a694,
    data_ov006_0213a63c, data_ov006_0213a79c,
};
extern "C" OamAttr *sMgTextSpritesEnglish[];
extern "C" OamAttr *sMgTextSpritesSpanish[];
extern "C" OamAttr *sMgTextSpritesFrench[];
extern "C" OamAttr *sMgTextSpritesGerman[];
extern "C" OamAttr *sMgTextSpritesItalian[];
OamAttr **sTeresaCountdownSprites[] = {
    sMgTextSpritesEnglish, sMgTextSpritesSpanish, sMgTextSpritesFrench,
    sMgTextSpritesGerman,  sMgTextSpritesItalian,
};
OamAttr data_ov006_0213a814[3] = {
    {0x00f8, 0x41f8, 0x35b1, 0x0000},
    {0x00fc, 0x0006, 0x31d3, 0x0000},
    {0x00fb, 0x01f3, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a844[3] = {
    {0x00f8, 0x41f8, 0x35b1, 0x0000},
    {0x00fd, 0x0007, 0x31f3, 0x0000},
    {0x00fc, 0x01f3, 0x39f2, 0xffff},
};
OamAttr data_ov006_0213a82c[3] = {
    {0x00f8, 0x41f8, 0x35b1, 0x0000},
    {0x00fc, 0x2006, 0x31d3, 0x0000},
    {0x00fe, 0x21f3, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a8a4[4] = {
    {0x0001, 0x01f9, 0x31f0, 0x0000},
    {0x00f8, 0x41f8, 0x35b1, 0x0000},
    {0x00fc, 0x0006, 0x31d3, 0x0000},
    {0x00fb, 0x01f3, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a8e4[4] = {
    {0x0001, 0x01f9, 0x31f0, 0x0000},
    {0x00f8, 0x41f8, 0x35b1, 0x0000},
    {0x00fd, 0x0007, 0x31f3, 0x0000},
    {0x00fc, 0x01f3, 0x39f2, 0xffff},
};
OamAttr data_ov006_0213a904[4] = {
    {0x0001, 0x01f9, 0x31f0, 0x0000},
    {0x00f8, 0x41f8, 0x35b1, 0x0000},
    {0x00fc, 0x2006, 0x31d3, 0x0000},
    {0x00fe, 0x21f3, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a804[2] = {
    {0x00f8, 0x51fa, 0x31ae, 0x0000},
    {0x80f8, 0x11f2, 0x31b0, 0xffff},
};
OamAttr data_ov006_0213a874[3] = {
    {0x00f8, 0x51f8, 0x35b1, 0x0000},
    {0x00fd, 0x11f1, 0x31f3, 0x0000},
    {0x00fc, 0x1005, 0x39f2, 0xffff},
};
OamAttr data_ov006_0213a85c[3] = {
    {0x00f8, 0x51f8, 0x35b1, 0x0000},
    {0x00fd, 0x11f1, 0x31f3, 0x0000},
    {0x00fc, 0x1005, 0x39f2, 0xffff},
};
OamAttr data_ov006_0213a88c[3] = {
    {0x00f8, 0x51f8, 0x35b1, 0x0000},
    {0x00fc, 0x31f2, 0x31d3, 0x0000},
    {0x00fe, 0x3005, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a924[4] = {
    {0x0001, 0x11ff, 0x31f0, 0x0000},
    {0x00f8, 0x51f8, 0x35b1, 0x0000},
    {0x00fc, 0x11f2, 0x31d3, 0x0000},
    {0x00fb, 0x1005, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a944[4] = {
    {0x0001, 0x11ff, 0x31f0, 0x0000},
    {0x00f8, 0x51f8, 0x35b1, 0x0000},
    {0x00fd, 0x11f1, 0x31f3, 0x0000},
    {0x00fc, 0x1005, 0x39f2, 0xffff},
};
OamAttr data_ov006_0213a8c4[4] = {
    {0x0001, 0x11ff, 0x31f0, 0x0000},
    {0x00f8, 0x51f8, 0x35b1, 0x0000},
    {0x00fc, 0x31f2, 0x31d3, 0x0000},
    {0x00fe, 0x3005, 0x39f1, 0xffff},
};
OamAttr data_ov006_0213a7f4[2] = {
    {0x00f8, 0x41f6, 0x31ae, 0x0000},
    {0x80f8, 0x0006, 0x31b0, 0xffff},
};
void *sTeresaBooFrameSprites[] = {
    data_ov006_0213a814, data_ov006_0213a844, data_ov006_0213a82c, data_ov006_0213a8a4,
    data_ov006_0213a8e4, data_ov006_0213a904, data_ov006_0213a804, data_ov006_0213a874,
    data_ov006_0213a85c, data_ov006_0213a88c, data_ov006_0213a924, data_ov006_0213a944,
    data_ov006_0213a8c4, data_ov006_0213a7f4,
};
extern int sTeresaBooTouchOffsets[];
extern int sTeresaSpawnCounts[];
extern int sTeresaSpawnKinds[];
extern int sTeresaSpawnBurst[];
/* PaintSquare's stipple shape: eight rows of eight 4-bit columns. */
int sTeresaPaintMask[8] = {0x00111100, 0x01111110, 0x11111111, 0x11111111,
                           0x11111111, 0x11111111, 0x01111110, 0x00111100};
extern int            sMgTopScreenYOffset;

/* The four pointer-to-member state tables. Pmf handlers take the Boo/slot
   index; the scene table in sTeresaSceneStateTable takes none. */
typedef void (dScMgTeresa_c::*Pmf)(int);
typedef void (dScMgTeresa_c::*Pmf0)();
struct PmfEntry { Pmf0 pmf; };

extern Pmf sTeresaBigBooStateTable[];     /* big Boo flight states, by BigBoo::state */
extern Pmf sTeresaRevealStateTable[];     /* reveal states, by mReveal[0].state */
extern Pmf sTeresaBooStateTable[];        /* per-Boo states, one call per live row */
extern PmfEntry sTeresaSceneStateTable[]; /* scene states, by mSceneState */

} /* extern "C" */
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

namespace Sound { void PlayBank2_2D(unsigned int id); }

/* One out-of-line definition; with codegen not deferred it emits D1, then
 * D0, as in the ROM. */
dScMgTeresa_c::~dScMgTeresa_c()
{
}

void dScMgTeresa_c::BigBoo_ShowResult() {
    if (mRoundCleared) {
        mBigBoo[0].frame = 3;
    } else {
        mBigBoo[0].frame = 4;
    }
    mBigBoo[0].x = 0x80000;
    mBigBoo[0].y = 0x100000;
    mBigBoo[0].yVel = -0x1800;
    mBigBoo[0].frameTimer = 0;
    mBigBoo[0].moveRight = 0;
    mBigBoo[0].state = 8;
    mBigBoo[0].shown = 1;
    mCountdownShown = 0;
}

void dScMgTeresa_c::BigBoo_SetDone() {
    if (mBigBoo[0].active != 0) {
        mBigBoo[0].done = 1;
    }
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::BigBoo_Draw() {
    int idx, p, q;
    OamAttr *v;
    if (mBigBoo[0].shown == 0)
        return;
    idx = mBigBoo[0].frame;
    p = mBigBoo[0].x >> 12;
    q = mBigBoo[0].y >> 12;
    if (mBigBoo[0].moveRight != 0)
        idx += 4;
    if (mBigBoo[0].state == 8)
        v = sTeresaBigBooHoverSprites[idx];
    else
        v = sTeresaBigBooSprites[idx];
    RenderOamMainScreen(v, p, q, -1, -1);
}

void dScMgTeresa_c::BigBoo_StateResult(int idx) {

    if ((mBigBoo[idx].y >> 12) == 0xc0) {
        mBigBoo[idx].frameTimer++;
        if (mRoundCleared != 0) {
            if (mBigBoo[idx].frame >= 3)
                return;
            if (mBigBoo[idx].frameTimer < 4)
                return;
            mBigBoo[idx].frameTimer = 0;
            mBigBoo[idx].frame++;
        } else {
            if (mBigBoo[idx].frame >= 6)
                return;
            if (mBigBoo[idx].frameTimer < 8)
                return;
            mBigBoo[idx].frameTimer = 0;
            mBigBoo[idx].frame++;
        }
    } else {
        mBigBoo[idx].shown = 1;
        mBigBoo[idx].y += mBigBoo[idx].yVel;
        mBigBoo[idx].yVel -= 0x200;
        if ((mBigBoo[idx].y >> 12) > 0xc0)
            return;
        mBigBoo[idx].y = 0xc0000;
        mBigBoo[idx].yVel = 0;
        mBigBoo[idx].frameTimer = 0;
        if (mRoundCleared != 0)
            mBigBoo[idx].frame = 0;
    }
}

/* State 7 does nothing. It takes the index like every other state in
   sTeresaBigBooStateTable, and ignores it. */
void dScMgTeresa_c::BigBoo_StateGone(int) {}

void dScMgTeresa_c::BigBoo_StateRemove(int index) {
    void *data;
    mBigBoo[index].xVel = 0;
    mBigBoo[index].yVel = 0;
    mBigBoo[index].shown = 0;
    data = LoadFile(0x103);
    DecompressLZ16(data, G2_GetOBJCharPtr());
    mBigBoo[index].state = 7;
    Deallocate(data);
}

void dScMgTeresa_c::BigBoo_StateExit(int i) {
    unsigned short *p16 = &mBigBoo[i].frameTimer;
    int *sum;
    int *add;
    int v;
    *p16 = *p16 + 1;
    if (*p16 >= 8) {
        unsigned char *p8;
        *p16 = 0;
        p8 = &mBigBoo[i].frame;
        *p8 = *p8 + 1;
        if (*p8 >= 4)
            *p8 = 0;
    }
    add = &mBigBoo[i].xVel;
    sum = &mBigBoo[i].x;
    *sum = *sum + *add;
    v = *sum >> 12;
    if (mBigBoo[i].moveRight != 0) {
        if (v >= 0x120) {
            mBigBoo[i].state = 6;
            return;
        }
        if (*add <= 0x3000) *add = *add + 0x200;
    } else {
        if (v <= -0x20) {
            mBigBoo[i].state = 6;
            return;
        }
        if (*add >= -0x3000) *add = *add - 0x200;
    }
}

void dScMgTeresa_c::BigBoo_StateOffscreen(int idx) {
    if (mBigBoo[idx].done != 0) {
        mBigBoo[idx].state = 6;
        return;
    }
    if (mBigBoo[idx].wait != 0) {
        mBigBoo[idx].wait--;
        return;
    }
    mBigBoo[idx].shown = 1;
    if ((mBigBoo[idx].x >> 0xc) < 0) {
        mBigBoo[idx].moveRight = 1;
        mBigBoo[idx].x = -0x80000;
        mBigBoo[idx].xVel = 0x4800;
        mBigBoo[idx].state = 3;
        return;
    }
    mBigBoo[idx].moveRight = 0;
    mBigBoo[idx].x = 0x180000;
    mBigBoo[idx].xVel = -0x4800;
    mBigBoo[idx].state = 2;
}

void dScMgTeresa_c::BigBoo_StateFlyRight(int i) {
    int v;
    mBigBoo[i].frameTimer++;
    if (mBigBoo[i].frameTimer >= 4) {
        mBigBoo[i].frameTimer = 0;
        mBigBoo[i].frame++;
        if (mBigBoo[i].frame >= 4)
            mBigBoo[i].frame = 0;
    }
    mBigBoo[i].x += mBigBoo[i].xVel;
    v = mBigBoo[i].x >> 0xc;
    if (v >= 0xc0) {
        if (mBigBoo[i].xVel >= 0x800)
            mBigBoo[i].xVel -= 0x80;
    } else {
        if (mBigBoo[i].xVel <= 0x7000)
            mBigBoo[i].xVel += 0x400;
    }
    if (v >= 0x120) {
        mBigBoo[i].xVel = 0;
        mBigBoo[i].x = 0x120000;
        mBigBoo[i].state = 4;
        mBigBoo[i].wait = 8;
        mBigBoo[i].shown = 0;
        if (mBigBoo[i].done != 0)
            mBigBoo[i].state = 6;
    }
}

void dScMgTeresa_c::BigBoo_StateFlyLeft(int i) {
    int t;

    mBigBoo[i].frameTimer++;
    if (mBigBoo[i].frameTimer >= 4) {
        mBigBoo[i].frameTimer = 0;
        mBigBoo[i].frame++;
        if (mBigBoo[i].frame >= 4) {
            mBigBoo[i].frame = 0;
        }
    }

    mBigBoo[i].x = mBigBoo[i].x + mBigBoo[i].xVel;
    t = mBigBoo[i].x >> 0xc;
    if (t <= 0x40) {
        if (mBigBoo[i].xVel <= -0x800) {
            mBigBoo[i].xVel = mBigBoo[i].xVel + 0x80;
        }
    } else {
        if (mBigBoo[i].xVel >= -0x7000) {
            mBigBoo[i].xVel = mBigBoo[i].xVel - 0x400;
        }
    }
    if (t > -0x20) {
        return;
    }
    mBigBoo[i].x = -0x20000;
    mBigBoo[i].xVel = 0;
    mBigBoo[i].state = 4;
    mBigBoo[i].wait = 8;
    mBigBoo[i].shown = 0;
    if (mBigBoo[i].done != 0) {
        mBigBoo[i].state = 6;
    }
}

void dScMgTeresa_c::BigBoo_StateHover(int i) {
    unsigned char state = mBigBoo[i].frame;
    if ((unsigned char)(state - 8) < 2)
    {
        mBigBoo[i].frameTimer++;
        if (mBigBoo[i].frameTimer >= sTeresaBigBooFrameTimes[(unsigned char)(state - 8)]) {
            mBigBoo[i].frameTimer = 0;
            mBigBoo[i].frame++;
        }
    }
    if (mBigBoo[i].wait != 0) {
        mBigBoo[i].wait--;
        return;
    }
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    SetBg3Offset(0, 0x100);
    {
        /* The draw lands in its own int: through MG_RAND the pick colours
           differently (5 words). */
        u32 rnd = RandomIntInternal(&data_0209d4b8.value);
        mBigBoo[i].state = sTeresaBigBooHoverPicks[(((rnd >> 16) & 0x7fff) * 2) >> 15];
    }
    mBigBoo[i].frameTimer = 0;
    mBigBoo[i].frame = 0;
    if (mBigBoo[i].state == 2) {
        mBigBoo[i].moveRight = 0;
        mBigBoo[i].xVel = -0x4800;
        mBigBoo[i].x = 0x180000;
    } else {
        mBigBoo[i].moveRight = 1;
        mBigBoo[i].xVel = 0x4800;
        mBigBoo[i].x = -0x80000;
    }
    if (mBigBoo[i].done != 0) {
        mBigBoo[i].state = 5;
    }
}

void dScMgTeresa_c::BigBoo_StateDropIn(int i) {
    mBigBoo[i].shown = 1;
    mBigBoo[i].y += mBigBoo[i].yVel;
    mBigBoo[i].yVel -= 0x100;
    if (mBigBoo[i].y >> 0xc > 0xc0)
        return;
    mBigBoo[i].y = 0xc0000;
    mBigBoo[i].yVel = 0;
    mBigBoo[i].state = 1;
    mBigBoo[i].wait = 0x30;
    Mg_ShowBanner(0xc, 0x80, 0x40, 1, -1, 0xd);
}

void dScMgTeresa_c::BigBoo_Update()
{
    if (mBigBoo[0].active == 0)
        return;
    (this->*sTeresaBigBooStateTable[mBigBoo[0].state])(0);
}

void dScMgTeresa_c::BigBoo_Start()
{
    mBigBoo[0].active = 1;
    mBigBoo[0].x = 0x80000;
    mBigBoo[0].y = 0x100000;
    mBigBoo[0].xVel = 0;
    mBigBoo[0].yVel = -0x1800;
    mBigBoo[0].frameTimer = 0;
    mBigBoo[0].wait = 0;
    mBigBoo[0].state = 0;
    mBigBoo[0].frame = 8;
    mBigBoo[0].moveRight = 0;
    mBigBoo[0].done = 0;
    G2_SetBlendAlpha(0, 0xc, 0xc, 0x10);
}

void dScMgTeresa_c::BigBoo_Hide()
{
    mBigBoo[0].active = 0;
    mBigBoo[0].shown = 0;
}

void dScMgTeresa_c::StepBg3Scroll() {
    if (mBg3FlashStep >= 8) {
        return;
    }
    mBg3FlashTick++;
    if (mBg3FlashTick >= sTeresaBg3FlashTimes[mBg3FlashStep]) {
        mBg3FlashStep++;
        mBg3FlashTick = 0;
        if (mBg3FlashStep & 1) {
            SetBg3Offset(0, 0);
        } else {
            SetBg3Offset(0, 0x100);
        }
    }
    if (mBg3FlashStep >= 8) {
        SetBg3Offset(0, 0);
    }
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Marker_Draw()
{
  if(mMarker.visible==0) return;
  RenderOamMainScreen(sTeresaMarkerSprites, mMarker.x >> 0xc, mMarker.y >> 0xc, -1, -1);
}

/* Empty. Scene_StateReveal calls it with the object. */
void dScMgTeresa_c::Marker_Update()
{
}

void dScMgTeresa_c::Marker_Show()
{
    mMarker.x = 0x50000;
    mMarker.y = 0x40000;
    mMarker.active = 1;
    mMarker.visible = 1;
}

void dScMgTeresa_c::Marker_Hide()
{
    mMarker.active = 0;
    mMarker.visible = 0;
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Reveal_Draw()
{
    if (mReveal[0].visible == 0) return;
    OAM::Render(0, sTeresaRevealSprites[mReveal[0].sprite], mReveal[0].x >> 12, mReveal[0].y >> 12,
                -1, 0, 0x1000, 0x1000, mReveal[0].angle, -1);
}

void dScMgTeresa_c::Reveal_StateFinish(int idx) {
    if (mReveal[idx].timer != 0) {
        mReveal[idx].timer -= 1;
        return;
    }
    mReveal[idx].visible = 0;
    mReveal[idx].active = 0;
    Marker_Hide();
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gSubScreenLayers |= 1;
    Sound::PlayBank2_2D(0x1f6);
    mSceneState = 2;
    StartBoosHiding();
    BigBoo_Start();
    Sound::PlayBank2_2D(0x1f5);
}

#define AT(p, off) (((p)+(off)))
void dScMgTeresa_c::Reveal_StateDipAndRise(int i) {
    u8 flag;

    flag = mReveal[i].phase;

    if (flag == 0) {
        mReveal[i].timer += 1;
        mReveal[i].y += mReveal[i].vy;
        mMarker.y += mReveal[i].vy;
        if (mReveal[i].vy >= 0x800)
            mReveal[i].vy -= 0x20;
        if (mReveal[i].timer >= 16) {
            mReveal[i].timer = 0;
            mReveal[i].phase += 1;
            mReveal[i].vy = 0;
        }
    } else if (flag == 1) {
        mReveal[i].timer += 1;
        if (mReveal[i].timer == 16)
            mReveal[i].vy = -0x1000;
        mReveal[i].y += mReveal[i].vy;
        mMarker.y += mReveal[i].vy;
        if (mReveal[i].vy <= -0x800)
            mReveal[i].vy += 0x20;
        if (mReveal[i].timer >= 32) {
            mReveal[i].timer = 32;
            mReveal[i].phase = 0;
            mReveal[i].state = 4;
        }
    }
}
#undef AT

void dScMgTeresa_c::Reveal_StateAnimate(int i) {
    mReveal[i].frameTimer++;
    if (mReveal[i].frameTimer < 8) return;
    mReveal[i].frameTimer = 0;
    mReveal[i].frame++;
    if (mReveal[i].frame >= 8) {
        mReveal[i].frame = 0;
        mReveal[i].sprite = 0;
        mReveal[i].state = 3;
        mReveal[i].timer = 0;
        mReveal[i].vy = 0x1000;
        Sound::PlayBank2_2D(0x1f9);
    } else {
        mReveal[i].sprite = sTeresaRevealAnimFrames[mReveal[i].frame];
    }
}

void dScMgTeresa_c::Reveal_StateSlideIn(int i) {
    mReveal[i].frameTimer += 1;
    if (mReveal[i].frameTimer >= 8) {
        mReveal[i].frameTimer = 0;
        mReveal[i].frame += 1;
        if (mReveal[i].frame >= 8)
            mReveal[i].frame = 0;
        mReveal[i].sprite = sTeresaRevealSlideFrames[mReveal[i].frame];
    }
    if (mReveal[i].x >> 12 <= 0x80) {
        mReveal[i].x = 0x80000;
    } else {
        mReveal[i].x += mReveal[i].vx;
        if (mReveal[i].vx <= -0x600)
            mReveal[i].vx += 0x20;
    }
    if (mReveal[i].timer != 0) {
        mReveal[i].timer -= 1;
        return;
    }
    Sound::PlayBank2_2D(0x1f3);
    mReveal[i].state = 2;
    mReveal[i].frame = 0;
    mReveal[i].sprite = 0;
    mReveal[i].frameTimer = 0;
    mReveal[i].vx = 0;
}

void dScMgTeresa_c::Reveal_StateStart(int i) {
    mReveal[i].state = 1;
    mReveal[i].timer = 64;
    mReveal[i].vx = -0xb00;
}

void dScMgTeresa_c::Reveal_Update()
{
    if (mReveal[0].active == 0) return;
    (this->*sTeresaRevealStateTable[mReveal[0].state])(0);
}

void dScMgTeresa_c::Reveal_Start()
{
    mReveal[0].x = 0xa0000;
    mReveal[0].y = 0x60000;
    mReveal[0].active = 1;
    mReveal[0].visible = 1;
    mReveal[0].state = 0;
    mReveal[0].sprite = 0;
    mReveal[0].angle = 0;
    mReveal[0].frameTimer = 0;
    mReveal[0].timer = 0;
    /* written only here, read nowhere in this TU */
    mReveal[0].unk_1b = 0;
    mReveal[0].frame = 0;
    mReveal[0].phase = 0;
}

void dScMgTeresa_c::Reveal_Hide()
{
    mReveal[0].visible = 0;
    mReveal[0].active = 0;
}

/* decl_common.h declares this one with a `void *` parameter, and
   Mg_RenderSubSpriteMtx as `(void *, int, int, Matrix2x2 *)`. */
void dScMgTeresa_c::FoundNumber_DrawAll() {
    int i;
    for (i = 0; i < FOUND_NUMBER_NUM; i++) {
        if (mFoundNumbers[i].visible) {
            int b = mFoundNumbers[i].y >> 12;
            int a = mFoundNumbers[i].x >> 12;
            if (b <= 8) b = 8;
            if (b >= 0xb8) b = 0xb8;
            Mg_RenderSubSpriteMtx(sMgDigitSprites[mFoundNumbers[i].sprite + 1], a, b, 0);
        }
    }
}

void dScMgTeresa_c::FoundNumber_ClearAll() {
    int i;
    for (i = 0; i < FOUND_NUMBER_NUM; i++) {
        mFoundNumbers[i].active = 0;
        mFoundNumbers[i].visible = 0;
    }
}

int dScMgTeresa_c::FoundNumber_CountBusy() {
    int cnt = 0;
    int i;
    for (i = 0; i < FOUND_NUMBER_NUM; i++) {
        if (mFoundNumbers[i].active != 0 && mFoundNumbers[i].state != 2) {
            cnt++;
        }
    }
    return cnt != 0;
}

void dScMgTeresa_c::FoundNumber_UpdateAll() {
    int i;
    for (i = 0; i < FOUND_NUMBER_NUM; i++) {
        unsigned char v;
        if (mFoundNumbers[i].active == 0) {
            continue;
        }
        v = mFoundNumbers[i].state;
        if (v == 0) {
            if (mFoundNumbers[i].timer != 0) {
                mFoundNumbers[i].timer -= 1;
                if ((short)mFoundNumbers[i].timer < 0) {
                    mFoundNumbers[i].timer = 0;
                }
            } else {
                Sound::PlayBank2_2D(0x1bc);
                mFoundNumbers[i].vy = -0x400;
                mFoundNumbers[i].timer = 16;
                mFoundNumbers[i].state += 1;
                mFoundNumbers[i].visible = 1;
            }
        } else if (v == 1) {
            mFoundNumbers[i].y += mFoundNumbers[i].vy;
            mFoundNumbers[i].vy -= 0x80;
            if (mFoundNumbers[i].timer != 0) {
                mFoundNumbers[i].timer -= 1;
                if ((short)mFoundNumbers[i].timer < 0) {
                    mFoundNumbers[i].timer = 0;
                }
            } else {
                mFoundNumbers[i].timer = 24;
                mFoundNumbers[i].state += 1;
            }
        }
    }
}

void dScMgTeresa_c::FoundNumber_Spawn(int i) {
    int n;
    for (n = 0; n < FOUND_NUMBER_NUM; n++) {
        if (mFoundNumbers[n].active == 0) {
            mFoundNumbers[n].active = 1;
            mFoundNumbers[n].visible = 0;
            mFoundNumbers[n].x = mBoos[i].mX;
            mFoundNumbers[n].y = mBoos[i].mY - 0x10000;
            mFoundNumbers[n].timer = (mBoos[i].mFoundOrder * 60) + 16;
            mFoundNumbers[n].state = 0;
            mFoundNumbers[n].sprite = mBoos[i].mFoundOrder;
            return;
        }
    }
}

void dScMgTeresa_c::FoundNumber_SpawnForFound() {
    int i;
    for (i = 0; i < BOO_NUM; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState == 5)
            FoundNumber_Spawn(i);
    }
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::VanishPuff_DrawAll() {
    int i;
    for (i = 0; i < VANISH_PUFF_NUM; i++) {
        if (mVanishPuffs[i].visible) {
            Hud_RenderSprite(sTeresaVanishPuffSprites[mVanishPuffs[i].frame],
                             mVanishPuffs[i].x >> 12, mVanishPuffs[i].y >> 12, -1, -1);
        }
    }
}

void dScMgTeresa_c::VanishPuff_UpdateAll() {
    int i;
    for (i = 0; i < VANISH_PUFF_NUM; i++) {
        if (mVanishPuffs[i].active == 0)
            continue;
        mVanishPuffs[i].timer += 1;
        if (mVanishPuffs[i].timer < 4)
            continue;
        mVanishPuffs[i].timer = 0;
        mVanishPuffs[i].frame += 1;
        if (mVanishPuffs[i].frame >= 5) {
            mVanishPuffs[i].frame = 0;
            mVanishPuffs[i].visible = 0;
            mVanishPuffs[i].active = 0;
        }
    }
}

void dScMgTeresa_c::VanishPuff_Spawn(int param) {
    int i;
    for (i = 0; i < VANISH_PUFF_NUM; i++) {
        if (mVanishPuffs[i].active == 0) {
            mVanishPuffs[i].x = mBoos[param].mX;
            mVanishPuffs[i].y = mBoos[param].mY;
            mVanishPuffs[i].timer = 0;
            mVanishPuffs[i].frame = 0;
            mVanishPuffs[i].active = 1;
            mVanishPuffs[i].visible = 1;
            return;
        }
    }
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::DrawCountdown()
{
    if (mCountdownShown == 0)
        return;
    Mg_DrawNumberRotated(0x80, 0x60, mTimeLeft, 1, 0, 0x800, 0);
    DrawOamSprite(sTeresaCountdownSprites[GetGameLanguage()][3], 128, 72, 0);
}

void dScMgTeresa_c::UpdateCountdown()
{
    if (mCountdownRunning == 0)
        return;
    if (mTimeLeft == 0)
        return;
    mCountdownFrames += 1;
    if (mCountdownFrames >= 60) {
        mCountdownFrames = 0;
        mTimeLeft -= 1;
        if ((s16)mTimeLeft < 0)
            mTimeLeft = 0;
        if (mTimeLeft != 0)
            Sound::PlayBank2_2D(0xa7);
        else
            Sound::PlayBank2_2D(0xa6);
    }
    if (mTimeLeft == 0) {
        mTimeLeft = 0;
        mCountdownFrames = 0;
    }
}

void dScMgTeresa_c::StartCountdown() {
    if (mRevealDone == 0)
        return;
    if (mCountdownRunning != 0)
        return;
    if (mLevel >= 0xf)
        mTimeLeft = 2;
    else
        mTimeLeft = 3;
    mCountdownFrames = 0;
    mCountdownShown = 1;
    mCountdownRunning = 1;
    Sound::PlayBank2_2D(0xa7);
}

/* decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::Exclaim_DrawAll()
{
    int i;
    for (i = 0; i < EXCLAIM_NUM; i++) {
        if (mExclaims[i].visible != 0) {
            int v0 = mExclaims[i].x >> 12;
            int v4 = mExclaims[i].y >> 12;
            if (v4 <= 8) v4 = 8;
            if (v4 >= 0xb8) v4 = 0xb8;
            Hud_RenderSprite(sTeresaExclaimSprites, v0, v4, -1, -1);
        }
    }
}

void dScMgTeresa_c::Exclaim_UpdateAll()
{
    int i;

    for (i = 0; i < EXCLAIM_NUM; i++) {
        if (mExclaims[i].active != 0) {
            if (mExclaims[i].timer != 0) {
                u8 *p = &mExclaims[i].timer;
                int v;
                *p -= 1;
                v = mExclaims[i].timer;
                if (v < 0)
                    mExclaims[i].timer = 0;
            } else {
                mExclaims[i].active = 0;
                mExclaims[i].visible = 0;
            }
        }
    }
}

void dScMgTeresa_c::Exclaim_Spawn(int idx)
{
    int i;
    for (i = 0; i < EXCLAIM_NUM; i++) {
        if (mExclaims[i].active == 0) {
            mExclaims[i].active = 1;
            mExclaims[i].visible = 1;
            mExclaims[i].x = mBoos[idx].mX;
            mExclaims[i].y = mBoos[idx].mY - 0x18000;
            mExclaims[i].timer = 96;
            return;
        }
    }
}

void dScMgTeresa_c::CheckRevealDone()
{
    int found;
    int i;
    if (mRevealDone != 0)
        return;
    found = 0;
    for (i = 0; i < BOO_NUM; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState <= 2) {
            found++;
            break;
        }
    }
    if (found != 0)
        return;
    FreeGfxSlotsById(0xc);
    mRevealDone++;
}

void dScMgTeresa_c::CheckRoundEnd()
{
    if (mTimeLeft == 0 && mRevealDone != 0) {
        mRoundCleared = 0;
        mSceneState = 3;
        mSceneTimer = 96;
        BigBoo_SetDone();
        mRubLocked = 1;
        return;
    }
    {
        int count = 0;
        int i;
        for (i = 0; i < BOO_NUM; i++) {
            if (mBoos[i].mInPlay != 0 && mBoos[i].mFound == 0) {
                count++;
                break;
            }
        }
        if (count != 0)
            return;
        mSceneState = 3;
        mSceneTimer = 96;
        BigBoo_SetDone();
        mRoundCleared = 1;
    }
}

/* Draws the 16 Boo sprites. Each 0x24-byte slot at +0x4660 holds a position
 * (x, y in 20.12), a translucency flag, a visible flag, an OAM priority and an
 * animation/frame pair that picks the sprite's attribute block from a table of
 * seven frames per animation. Visible slots go through OAM::Render at unit
 * scale, forced semi-transparent (mode 1) when the flag is set and in the
 * attribute's own mode (-1) otherwise. The mode is declared between the frame
 * index and the y read on purpose: that is what puts the -1 where the ROM
 * keeps it. decl_common.h declares this one with a `void *` parameter. */
void dScMgTeresa_c::DrawBoos()
{
    int i;

    for (i = 0; i < BOO_NUM; i++) {
        if (mBoos[i].mVisible != 0) {
            void **frames = sTeresaBooFrameSprites;
            int translucent = mBoos[i].mFade;
            int x = mBoos[i].mX >> 12;
            int frame = mBoos[i].mFrame;
            int anim = mBoos[i].mAnim;
            int idx = anim * 7 + frame;
            int mode = -1;
            int y = mBoos[i].mY >> 12;
            int priority = mBoos[i].mPriority;

            if (translucent != 0)
                mode = 1;
            OAM::Render(1, (struct OamAttr *)frames[idx], x, y, -1, priority, 0x1000, 0x1000, 0,
                        mode);
        }
    }
}

void dScMgTeresa_c::AreBoosSettled()
{
    int found = 0;
    int i = 0;
    for (; i < BOO_NUM; i++) {
        if (mBoos[i].mInPlay != 0) {
            unsigned int t = (mBoos[i].mState + 0xfd) & 0xff;
            if (t <= 1) {
                mBoos[i].mState = 6;
                mBoos[i].mTimer = 0;
                mBoos[i].mFade = 0;
                mBoos[i].mSettled = 1;
                mBoos[i].mFrame = 3;
                mBoos[i].mPlaySettleSound = 0;
                mLastSettledBoo = i;
                mBoos[i].mDelay = 1;
                found++;
            }
        }
    }
    if (found != 0) {
        unsigned char idx = mLastSettledBoo;
        mBoos[idx].mPlaySettleSound = 1;
    } else {
        unsigned char idx = mLastSettledBoo;
        mBoos[idx].mPlaySettleSound = 0;
    }
}

#define A(a) (*(a))
void dScMgTeresa_c::TouchTestBoo(int idx)
{
    int x;
    int y;
    int count;
    int firstOk;
    int i;
    int* r;
    int word;

    if (mBoos[idx].mFound != 0)
        return;

    count = 0;
    firstOk = 0;
    for (i = 0; i < 5; i++) {
        x = (mBoos[idx].mX >> 12) - sTeresaBooTouchOffsets[i * 2];
        y = (mBoos[idx].mY >> 12) - sTeresaBooTouchOffsets[i * 2 + 1];
        r = ((int(*)[8])G2S_GetBG0CharPtr())[(x >> 3) + ((y >> 3) << 5)];
        word = r[y & 7];
        if (((word >> ((x & 7) << 2)) & 0xf) == 0) {
            count++;
            if (i == 0)
                firstOk++;
        }
    }

    if (count < 2)
        return;
    if (firstOk == 0)
        return;

    mBoos[idx].mFound = 1;
    mBoos[idx].mPriority = 0;
    mBoos[idx].mState = 5;
    mBoos[idx].mFoundOrder = mFoundCount;
    mBoos[idx].mTimer = 64;
    mFoundCount++;
    Exclaim_Spawn(idx);
    func_02012718(0x1f0, mBoos[idx].mX);
    if (mFirstFoundBoo == 0xff)
        mFirstFoundBoo = idx;
}
#undef A

void dScMgTeresa_c::CountRubbedTiles(int idx)
{
    int gx, gy;
    int cnt;
    int y;
    int x;
    int *row;

    cnt = 0;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            gx = x + ((mBoos[idx].mX >> 12) - 4);
            gy = y + ((mBoos[idx].mY >> 12) - 4);
            row = (int *)((char *)G2S_GetBG0CharPtr() + ((gx >> 3) + ((gy >> 3) << 5)) * 32);
            if (((row[gy & 7] >> ((gx & 7) << 2)) & 0xf) == 0)
                cnt++;
        }
    }
    if (cnt == 0)
        return;
    if (mBoos[idx].mKind == 0)
        return;
    mBoos[idx].mHeading = mBoos[idx].mHeading + 0x8000;
}

void dScMgTeresa_c::MoveBoo(int i)
{
    if (mBoos[i].mKind == 0) {
        return;
    }
    StepBoo(i);
    BounceBooOffWalls(i);
}

void dScMgTeresa_c::Boo_StateFadeAway(int i) {
    if (mBoos[i].mDelay != 0) {
        mBoos[i].mDelay--;
        if (mBoos[i].mDelay == 0)
            Sound::PlayBank2_2D(0x1f1);
    }

    if (mBoos[i].mFrame != 6) {
        mBoos[i].mAnimTimer++;
        if (mBoos[i].mAnimTimer >= 15) {
            mBoos[i].mAnimTimer = 0;
            mBoos[i].mFrame++;
            if (mBoos[i].mFrame >= 6)
                mBoos[i].mFrame = 3;
        }
    }

    StepBoo(i);
    BounceBooOffWalls(i);

    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;

    if (mBoos[i].mTurnTimer != 0) {
        mBoos[i].mTurnTimer--;
        if (mBoos[i].mTurnTimer != 0)
            return;
        mBoos[i].mHeading = mBoos[i].mHeading + 0x8000;
        mBoos[i].mFrame = 6;
        if (mBoos[i].mPlaySettleSound != 0)
            Sound::PlayBank2_2D(0x1f4);
        return;
    }

    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;
    mBoos[i].mFade++;
    mBoos[i].mTimer = 0;
    if (i == mLastSettledBoo) {
        G2S_SetBlendAlpha(0, 4, 0x10 - mBoos[i].mFade, 0x10);
    }
    if (mBoos[i].mFade >= 0x10) {
        mBoos[i].mInPlay = 0;
        mBoos[i].mVisible = 0;
    }
}

void dScMgTeresa_c::Boo_StateFadeIn(int i) {

    if (mBoos[i].mDelay != 0) {
        mBoos[i].mDelay--;
        if (mBoos[i].mDelay == 0) {
            if (mBoos[i].mPlaySettleSound != 0)
                Sound::PlayBank2_2D(0x1f1);
        }
    }

    mBoos[i].mAnimTimer++;
    if (mBoos[i].mAnimTimer >= 15) {
        mBoos[i].mAnimTimer = 0;
        mBoos[i].mFrame++;
        if (mBoos[i].mFrame >= 6)
            mBoos[i].mFrame = 3;
    }

    StepBoo(i);
    BounceBooOffWalls(i);

    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;

    if (mBoos[i].mFade >= 0x10)
        return;

    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;

    mBoos[i].mFade++;
    mBoos[i].mPriority = 0;
    mBoos[i].mTimer = 0;

    if (i == mLastSettledBoo) {
        G2S_SetBlendAlpha(0, 1, mBoos[i].mFade, 0x10);
    }

    if (mBoos[i].mFade < 0x10)
        return;

    mBoos[i].mTurnTimer = 128;
    mBoos[i].mTimer = 0;
    mBoos[i].mFade = 0;
    mBoos[i].mState = 7;
}

void dScMgTeresa_c::Boo_StateFound(int idx) {
    if (mSceneState != 4)
        return;
    if (mBoos[idx].mTimer != 0) {
        mBoos[idx].mTimer--;
        return;
    }
    VanishPuff_Spawn(idx);
    mBoos[idx].mVisible = 0;
    mBoos[idx].mInPlay = 0;
    if (mFirstFoundBoo != idx)
        return;
    Sound::PlayBank2_2D(0x1c7);
}

void dScMgTeresa_c::Boo_StateHiddenTimed(int idx) {
    if (mBoos[idx].mTimer != 0) {
        mBoos[idx].mTimer--;
        if ((s16)mBoos[idx].mTimer <= 0) {
            mBoos[idx].mTimer = 0;
            mBoos[idx].mState = 3;
            return;
        }
    }
    StepBoo(idx);
    BounceBooOffWalls(idx);
    if (FX_CosIdx(mBoos[idx].mHeading >> 4) >= 0)
        mBoos[idx].mAnim = 1;
    else
        mBoos[idx].mAnim = 0;
    TouchTestBoo(idx);
}

void dScMgTeresa_c::Boo_StateHidden(int i) {
    MoveBoo(i);
    CountRubbedTiles(i);
    TouchTestBoo(i);
    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
}

void dScMgTeresa_c::Boo_StateFadeOut(int i) {
    StepBoo(i);
    BounceBooOffWalls(i);
    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
    mBoos[i].mTimer++;
    if (mBoos[i].mTimer < 4)
        return;
    mBoos[i].mFade++;
    mBoos[i].mTimer = 0;
    if (i == 0)
        G2S_SetBlendAlpha(0, 1, 0x10 - mBoos[i].mFade, 0x10);
    if (mBoos[i].mFade < 0x10)
        return;
    mBoos[i].mFade = 0;
    mBoos[i].mState = 3;
    mBoos[i].mPriority = 1;
}

void dScMgTeresa_c::BounceBooOffWalls(int i)
{
    int xt = mBoos[i].mX >> 12;
    int yt = mBoos[i].mY >> 12;
    if (xt + 8 > 0xfe) {
        mBoos[i].mHeading = 0x8000 - mBoos[i].mHeading;
        mBoos[i].mX = 0xf6000;
    } else if (xt - 8 < 2) {
        mBoos[i].mHeading = 0x8000 - mBoos[i].mHeading;
        mBoos[i].mX = 0xa000;
    }
    if (yt + 8 > 0xbe) {
        mBoos[i].mHeading = -mBoos[i].mHeading;
        mBoos[i].mY = 0xb6000;
    } else if (yt - 8 < 2) {
        mBoos[i].mHeading = -mBoos[i].mHeading;
        mBoos[i].mY = 0xa000;
    }
}

void dScMgTeresa_c::StepBoo(int i) {
    int c, s;
    c = FX_CosIdx(mBoos[i].mHeading >> 4);
    mBoos[i].mX += FX_MUL(c, mBoos[i].mSpeed);
    s = FX_SinIdx(mBoos[i].mHeading >> 4);
    mBoos[i].mY += FX_MUL(s, mBoos[i].mSpeed);
}

void dScMgTeresa_c::StartBoosHiding()
{
    int i;
    for (i = 0; i < BOO_NUM; i++) {
        if (mBoos[i].mInPlay != 0 && mBoos[i].mState == 1)
            mBoos[i].mState = 2;
    }
}

void dScMgTeresa_c::Boo_StateFly(int i) {
    mBoos[i].mTimer = 0;
    StepBoo(i);
    BounceBooOffWalls(i);
    if (FX_CosIdx(mBoos[i].mHeading >> 4) >= 0)
        mBoos[i].mAnim = 1;
    else
        mBoos[i].mAnim = 0;
}

void dScMgTeresa_c::Boo_StateAppear(int idx) {
    unsigned char t;
    mBoos[idx].mVisible = 1;
    mBoos[idx].mTimer = 96;
    mBoos[idx].mSettled = 0;
    mBoos[idx].mState = 1;
    mBoos[idx].mSpeed = 0xc00;
    t = mBoos[idx].mKind;
    if (t == 0)
        return;
    if (t == 8)
        mBoos[idx].mSpeed -= 0x200;
    else
        mBoos[idx].mSpeed += (t - 1) << 9;
}

void dScMgTeresa_c::AnimateBoo(int i)
{
    if (mBoos[i].mState >= 6) return;
    mBoos[i].mAnimTimer++;
    if (mBoos[i].mAnimTimer < 15)
        return;
    mBoos[i].mAnimTimer = 0;
    mBoos[i].mFrame++;
    if (mBoos[i].mFrame >= 3)
        mBoos[i].mFrame = 0;
}

void dScMgTeresa_c::UpdateBoos()
{
    int i;
    for (i = 0; i < BOO_NUM; i++) {
        if (mBoos[i].mInPlay) {
            (this->*sTeresaBooStateTable[mBoos[i].mState])(i);
            AnimateBoo(i);
        }
    }
}

void dScMgTeresa_c::SpawnBoos() {
    int sel;
    int count;
    unsigned short phase;
    int i;
    int valA;
    int flagA;
    int valB;
    int cval;
    int full;

    sel = mLevel;
    if (sel >= 0xf) {
        int k;
        k = MG_RAND(4);
        if (k + 0xa == mBooPattern) {
            k += MG_RAND(3) + 1;
            if (k >= 4) k -= 4;
        }
        sel = k + 0xa;
    }
    mBooPattern = sel;
    count = sTeresaSpawnCounts[sel];
    mBooCount = count;

    phase = MG_RAND(16) << 12;
    valA = ((MG_RAND(8) << 3) + 0x60) << 12;
    valB = ((MG_RAND(8) << 3) + 0x60) << 12;

    i = 0;
    if (count <= 0) {
        return;
    }
    flagA = sTeresaSpawnBurst[sel];
    cval = sTeresaSpawnKinds[sel];
    full = 0x10000;
    do {
        if (flagA == 0) {
            mBoos[i].mX = ((MG_RAND(0xf) << 4) + 0x10) << 12;
            mBoos[i].mY = ((MG_RAND(0xb) << 4) + 0x10) << 12;
            mBoos[i].mHeading = MG_RAND(32) << 11;
        } else {
            mBoos[i].mX = valA;
            mBoos[i].mY = valB;
            int q = full / count;
            mBoos[i].mHeading = q * i + phase;
        }
        mBoos[i].mKind = cval;
        mBoos[i].mTimer = 0;
        mBoos[i].mAnimTimer = 0;
        mBoos[i].mTurnTimer = 0;
        mBoos[i].mDelay = 0;
        mBoos[i].mInPlay = 1;
        mBoos[i].mState = 0;
        mBoos[i].mSettled = 0;
        mBoos[i].mPriority = 0;
        mBoos[i].mFound = 0;
        mBoos[i].mFrame = 0;
        {
            int ang = mBoos[i].mHeading;
            if (FX_CosIdx(ang >> 4) >= 0) {
                mBoos[i].mAnim = 1;
            } else {
                mBoos[i].mAnim = 0;
            }
        }
        i++;
    } while (i < count);
}

void dScMgTeresa_c::UpdateRubbing() {
    int close;

    if (mRevealDone == 0)
        return;
    if (mRubLocked != 0)
        return;

    close = 0;
    ReadRubTouch();

    if (mRubTouching != 0) {
        /* Not Vector3: its C++ destructor changes this frame. */
        VecFx32 d;
        int dist;
        d.x = mRubX - mRubPrevX;
        d.y = mRubY - mRubPrevY;
        dist = cstd::sqrt(((d.x) * (d.x) + (d.y) * (d.y)));
        if (dist <= 3)
            close = 1;
        if (close == 0) {
            int snd;
            int cur;
            Mg_DrawLine(mRubPrevX, mRubPrevY, mRubX, mRubY, 0, 8);
            snd = func_020126e8(mRubX << 12);
            cur = mRubSound;
            mRubSound = func_02012468(cur, 2, 0x1F8, 4, 0, 0, snd, 0);
        }
    }
    if (close == 0) {
        mRubPrevX = mRubX;
        mRubPrevY = mRubY;
    }
}

void dScMgTeresa_c::ReadRubTouch() {
    int i = gActivePlayerSlot;
    if (gTouchInfo[i].held != 0 && mRubTouching == 0) {
        mRubX = gTouchInfo[i].x;
        mRubY = gTouchInfo[i].y;
        mRubPrevX = mRubX;
        mRubPrevY = mRubY;
        mRubTouching += 1;
    }
    if (gTouchInfo[gActivePlayerSlot].held == 0) {
        mRubTouching = 0;
    }
    if (mRubTouching == 0)
        return;
    mRubX = gTouchInfo[gActivePlayerSlot].x;
    mRubY = gTouchInfo[gActivePlayerSlot].y;
}

void dScMgTeresa_c::ResetRound() {

    for (int i = 0; i < BOO_NUM; i++) {
        mBoos[i].mX = 0;
        mBoos[i].mY = 0;
        mBoos[i].mTimer = 0;
        mBoos[i].mAnimTimer = 0;
        mBoos[i].mTurnTimer = 0;
        mBoos[i].mInPlay = 0;
        mBoos[i].mState = 0;
        mBoos[i].mSettled = 0;
        mBoos[i].mVisible = 0;
        mBoos[i].mPriority = 0;
        mBoos[i].mFound = 0;
        mBoos[i].mFrame = 0;
        mBoos[i].mAnim = 0;
        mBoos[i].mFade = 0;
    }
    for (int i = 0; i < EXCLAIM_NUM; i++) {
        mExclaims[i].x = 0;
        mExclaims[i].y = 0;
        mExclaims[i].timer = 0;
        mExclaims[i].active = 0;
        mExclaims[i].visible = 0;
    }
    for (int i = 0; i < VANISH_PUFF_NUM; i++) {
        mVanishPuffs[i].active = 0;
        mVanishPuffs[i].visible = 0;
    }
    mRubX = 0;
    mRubY = 0;
    mRubPrevX = 0;
    mRubPrevY = 0;
    pad_4bfc = 0;
    pad_4c00 = 0;
    pad_4c10 = 0;
    mSceneTimer = 0;
    mBg3FlashTimer = 0;
    mRubTouching = 0;
    mRoundCleared = 0;
    mRevealDone = 0;
    mTimeLeft = 0;
    mCountdownFrames = 0;
    mCountdownShown = 0;
    mCountdownRunning = 0;
    mFoundCount = 0;
    Reveal_Hide();
    Marker_Hide();
    mBg3FlashTick = 0;
    mBg3FlashStep = 0;
    BigBoo_Hide();
    mRubSound = 0;
    mFirstFoundBoo = 0xff;
    mRubLocked = 0;
}

void dScMgTeresa_c::Scene_StateResult() {
    BigBoo_Update();
    UpdateBoos();
    VanishPuff_UpdateAll();
    if (mSceneTimer == 0)
        return;
    mSceneTimer -= 1;
    if ((s16)mSceneTimer > 0)
        return;
    mSceneTimer = 0;
    if (mRoundCleared != 0) {
        dScMgBase_c *scene;
        Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
        scene = gMgScene;
        if (scene != 0) {
            if (scene->mHudScore < 0x270f) {
                scene->mHudScore += 1;
            }
            if (scene->mHudScore > scene->mBestHudScore) {
                scene->mBestHudScore = scene->mHudScore;
            }
        }
        Mg_SubmitScore(gMgScene ? gMgScene->mHudScore : 0);
    } else {
        if (Mg_IsNewRecord() != 0) {
            mCountdownShown = 0;
        }
        Mg_ShowBanner(8, 0x80, 0x60, 1, -1, 0xd);
        Mg_SetSharedState(MG_STATE_GAME_OVER);
    }
    mPromptEnabled = 0;
}

void dScMgTeresa_c::Scene_StateRoundEnd() {
    BigBoo_Update();
    UpdateRubbing();
    if (mSceneTimer != 0) {
        mSceneTimer -= 1;
        if ((s16)mSceneTimer > 0)
            return;
        if (mRoundCleared != 0) {
            FoundNumber_SpawnForFound();
            mBg3FlashTimer = 96;
            return;
        }
        FoundNumber_SpawnForFound();
        AreBoosSettled();
        mBg3FlashTimer = 96;
        return;
    }
    FoundNumber_UpdateAll();
    UpdateBoos();
    if (FoundNumber_CountBusy() != 0)
        return;
    if (mBg3FlashTimer != 0) {
        StepBg3Scroll();
        mBg3FlashTimer -= 1;
        if ((s16)mBg3FlashTimer < 0)
            mBg3FlashTimer = 0;
        return;
    }
    FoundNumber_ClearAll();
    mSceneState = 4;
    mSceneTimer = 96;
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    gSubScreenLayers = gSubScreenLayers & ~1;
    Sound::PlayBank2_2D(0x1f7);
    BigBoo_ShowResult();
    if (mRoundCleared == 0)
        return;
    Sound::PlayBank2_2D(0x1f2);
}

void dScMgTeresa_c::Scene_StatePlay() {
    if (mBannerTimer != 0) {
        mBannerTimer -= 1;
        if (mBannerTimer == 0) {
            FreeGfxSlotsById(0xd);
            if (mPromptBlinkCount == 0) {
                mPromptEnabled = 1;
                mPromptBlinkCount = 1;
                mPromptBlinkTimer = 0;
            }
        }
    }
  UpdateCountdown();
  UpdateRubbing();
  UpdateBoos();
  CheckRevealDone();
  StartCountdown();
  BigBoo_Update();
  CheckRoundEnd();
}

void dScMgTeresa_c::Scene_StateReveal() {
    Reveal_Update();
    Marker_Update();
    UpdateBoos();
}

void dScMgTeresa_c::Scene_StateInit() {
    mSceneState = 1;
}

#pragma push
/* Kept: with loop invariants hoisted, the constants spill, the stack frame
 * grows and buf moves to another slot. The hand-rotated do/while is kept
 * too: `for (j = 0; j < n; j++)` with y = cy - half + j is the right size
 * but schedules cx - half and cy - half ahead of the n <= 0 exit (12 words
 * off; corner in the for initialiser, a word short; measured 2026-11-07).
 * Re-measured (#821, 2026-11-16): 0x170 without the pragma against the ROM's
 * 0x15c; a ~(0xf << s) mask or a named shift local 0x16c; x = cx - half + i
 * 0x174. Round 64: what loop-invariant motion moves is the row terms
 * (y / 8) * 32 and (y & 7) * 4, hoisted out of the inner loop; a block-scope
 * `yy = y` copy inside the inner loop is copy-propagated away (0x170), and
 * the setting file-wide breaks TouchTestBoo and CountRubbedTiles. */
/* Kept (#821 r79): y recomputed from the row counter, the inner loop as a do-while and a tile
   local are 0x164, 0x168 and 0x170 against 0x15c. */
#pragma opt_loop_invariants off
/* dScMgTeresa_c::PaintSquare - slot 34, a STIPPLED brush.

   Same square walk and the same 4bpp nibble arithmetic as dScMgBase_c's, with
   one difference that changes what it is for: each cell is painted only if
   `sTeresaPaintMask[row] & (1 << (col*4))` is set, so the table is a
   bitmapped shape and this stamps that shape rather than a solid block.

   It also never consults the layer index at +0x6c -- it always draws into sub
   BG0 -- which is why `this` arrives and is never read.  The comment in
   include/dScMgTeresa_c.h predicted both of these before the slot was
   declared. */
void dScMgTeresa_c::PaintSquare(int cx, int cy, int val, int n)
{
    int half;
    int x0;
    int j;
    int x;
    int buf;
    int *addr;
    int i;
    int y;

    half = n / 2;
    j = 0;
    if (j < n) {
        y = cy - half;
        x0 = cx - half;
        do {
            for (i = 0; i < n; i++) {
                if (sTeresaPaintMask[j] & (1 << (i * 4))) {
                    x = i + x0;
                    if (x >= 0 && x < 0x100) {
                        if (y >= -0xc0 - sMgTopScreenYOffset && y >= 0 && y < 0xc0) {
                            addr = (int *)((char *)G2S_GetBG0CharPtr() +
                                           ((x / 8) + (y / 8) * 32) * 32 + (y & 7) * 4);
                            MI_CpuCopy32(addr, &buf, 4);
                            buf = (buf & (-1 ^ (0xf << ((x & 7) * 4)))) | (val << ((x & 7) * 4));
                            MI_CpuCopy32(&buf, addr, 4);
                        }
                    }
                }
            }
            y++;
            j++;
        } while (j < n);
    }
}
#pragma pop

/* Slot 20, OnNewRecord (see dScMgBase_c.h). A thunk:
   FreeGfxSlotsById(8). */
void dScMgTeresa_c::OnNewRecord()
{
    FreeGfxSlotsById(8);
}

/* Vtable slot 18, an override of dScMgBase_c::StartRound(int). The
   signature must repeat the base declaration exactly, or mwcc appends a slot
   instead of overriding. */
void dScMgTeresa_c::StartRound(int reset) {
    /* volatile: the cartridge stores the fill value to the stack and reloads
       it (strh/ldrh [sp]) before MIi_CpuClear16. Plain locals, by-value and
       by-reference wrappers all fold the constant; no other spelling found. */
    volatile unsigned short val;
    if (reset == 0) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc
           field address into its own register (add r2,r4,#0xbc / ldr [r2] /
           str [r2]) and the function grows a word; the cartridge re-issues
           ldr r1,[r4,#0xbc]. Same lever as dScMgHanachan_c::StartRound. */
        const unsigned int level = mLevel;
        mLevel = level + 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        mHudScore = 0;
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    }
    ResetRound();
    Reveal_Start();
    Marker_Show();
    SpawnBoos();
    char *dst = (char *)G2S_GetBG0CharPtr();
    val = 0x1111;
    MI_CpuFill16(dst, val, 0x6000);
    mSceneState = 0;
    FreeGfxSlotsById(0x1d);
    mBannerTimer = 32;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    void *h = LoadFile(0x101);
    DecompressLZ16(h, G2_GetOBJCharPtr());
    Deallocate(h);
}

/* Vtable slot 9. */
s32 dScMgTeresa_c::Render()
{
    DrawScoreTally(0xe0, 0x14, 1);
    FoundNumber_DrawAll();
    DrawCountdown();
    Exclaim_DrawAll();
    VanishPuff_DrawAll();
    DrawBoos();
    Reveal_Draw();
    Marker_Draw();
    BigBoo_Draw();
    return 1;
}

/* Vtable slot 6: runs the current state's handler from the pointer-to-member
   table sTeresaSceneStateTable, indexed by mSceneState. */
s32 dScMgTeresa_c::Behavior()
{
    (this->*sTeresaSceneStateTable[mSceneState].pmf)();
    Exclaim_UpdateAll();
    return 1;
}

/* Vtable slot 0. */
s32 dScMgTeresa_c::InitResources()
{
    void *f1;
    void *f2;
    void *t;
    /* volatile: each fill value round-trips through the stack before
       MIi_CpuClear16, as in StartRound. */
    volatile u16 fa;
    volatile u16 fb;
    volatile u16 fc;
    G2_SetBG2Priority(0);
    G2_BG2Mosaic(0);
    G2_SetBG2Offset(0, 0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000);
    {
        void *p = G2_GetBG2ScrPtr();
        fa = 0x5300;
        MI_CpuFill16(p, fa, 0x800);
    }
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    G2_SetBG3Priority(0);
    G2_BG3Mosaic(0);
    G2_SetBG3Offset(0, 0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9800,
                         GX_BG_CHARBASE_0x10000);
    t = LoadFile(0x9b);
    DecompressLZ16(t, G2_GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0x9c);
    GX_LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0x9e);
    GX_LoadBG3Scr(t, 0, 0x800);
    Deallocate(t);
    t = LoadFile(0x9d);
    GX_LoadBG3Scr(t, 0x800, 0x800);
    Deallocate(t);
    f1 = LoadFile(0x101);
    f2 = LoadFile(0x102);
    DecompressLZ16(f1, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(f2, 0, 0x100);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                      GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(0);
    G2S_SetBG0Offset(0, 0);
    G2S_SetBG0Priority(0);
    {
        void *p = G2S_GetBG0ScrPtr();
        fb = 0;
        MI_CpuFill16(p, fb, 0x800);
    }
    InitBrush(0, 0, 0);
    {
        void *p = G2S_GetBG0CharPtr();
        fc = 0x1111;
        MI_CpuFill16(p, fc, 0x6000);
    }
    gSubScreenLayers |= 4;
    G2S_SetBG2Priority(1);
    G2S_BG2Mosaic(0);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x2000,
                          GX_BG_CHARBASE_0x08000);
    t = LoadFile(0x9f);
    DecompressLZ16(t, G2S_GetBG2CharPtr());
    Deallocate(t);
    t = LoadFile(0xa0);
    GXS_LoadBGPltt(t, 0x60, 0x1a0);
    Deallocate(t);
    t = LoadFile(0xa1);
    GXS_LoadBG2Scr(t, 0, 0x800);
    Deallocate(t);
    DecompressLZ16(f1, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(f2, 0, 0x100);
    Deallocate(f1);
    Deallocate(f2);
    Mg_SetTopScreenYOffset(0x20);
    ResetRound();
    mBooPattern = 0xff;
    Marker_Show();
    Reveal_Start();
    SpawnBoos();
    mSceneState = 1;
    mBannerTimer = 32;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    sMgClearBannerX = 0x80;
    sMgClearBannerY = -128;
    mHudScore = 0;
    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMgTeresa_c through RTTI,
 * allocation size, vtable identity, and the MG_TERESA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgHideAndBooSeek_Spawn. */
void *dScMgTeresa_c_classInit() {
    return new dScMgTeresa_c;
}

BaseSpawnInfo<void> g_profile_MG_TERESA = {dScMgTeresa_c_classInit, 0x183, 0x183};

/* The four state tables, defined in retail copy order: the scene table,
 * the per-Boo table, the reveal-pair table, then the big-Boo flight table.
 * mwcc emits __sinit_d_s_mg_teresa.cpp from these definitions. */
PmfEntry sTeresaSceneStateTable[5] = {
    {&dScMgTeresa_c::Scene_StateInit},   {&dScMgTeresa_c::Scene_StateReveal},
    {&dScMgTeresa_c::Scene_StatePlay},   {&dScMgTeresa_c::Scene_StateRoundEnd},
    {&dScMgTeresa_c::Scene_StateResult},
};
Pmf sTeresaBooStateTable[8] = {
    &dScMgTeresa_c::Boo_StateAppear,      &dScMgTeresa_c::Boo_StateFly,
    &dScMgTeresa_c::Boo_StateFadeOut,     &dScMgTeresa_c::Boo_StateHidden,
    &dScMgTeresa_c::Boo_StateHiddenTimed, &dScMgTeresa_c::Boo_StateFound,
    &dScMgTeresa_c::Boo_StateFadeIn,      &dScMgTeresa_c::Boo_StateFadeAway,
};
Pmf sTeresaRevealStateTable[5] = {
    &dScMgTeresa_c::Reveal_StateStart,   &dScMgTeresa_c::Reveal_StateSlideIn,
    &dScMgTeresa_c::Reveal_StateAnimate, &dScMgTeresa_c::Reveal_StateDipAndRise,
    &dScMgTeresa_c::Reveal_StateFinish,
};
Pmf sTeresaBigBooStateTable[9] = {
    &dScMgTeresa_c::BigBoo_StateDropIn,    &dScMgTeresa_c::BigBoo_StateHover,
    &dScMgTeresa_c::BigBoo_StateFlyLeft,   &dScMgTeresa_c::BigBoo_StateFlyRight,
    &dScMgTeresa_c::BigBoo_StateOffscreen, &dScMgTeresa_c::BigBoo_StateExit,
    &dScMgTeresa_c::BigBoo_StateRemove,    &dScMgTeresa_c::BigBoo_StateGone,
    &dScMgTeresa_c::BigBoo_StateResult,
};
