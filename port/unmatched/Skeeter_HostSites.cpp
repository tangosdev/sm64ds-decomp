/* PORT_HOST_ABI. SKEETER (230, 9daMenbo_c), ov090. Run rel0215 wave 2, lane
 * cast-ov090.
 *
 * Three sites MSVC cannot deliver from src/, each refused for a named reason.
 * The byte-matched originals stay in src/ and are dropped from the CMake
 * source list by port/slice_ov090.txt; nothing here edits src/.
 *
 * (1) src/_ZN7Skeeter6RenderEv.cpp -- THE ModelAnim SLOT-5 COLLISION, the
 *     Whomp/Fish/Spiny case (T1). The source draws through a six-virtual local
 *     shadow over the embedded ModelAnim at +0x30c, so it reaches index 5 in
 *     MSVC's own numbering; but hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in
 *     MSVC numbering, where slot 5 is Virtual18 and Render has moved. The host
 *     copy names ModelAnim::Render outright. Offsets are Skeeter.h's own:
 *     unk flags 0xb0, mScaleX 0x80, mModelAnim 0x30c. The scale argument is
 *     &mScaleX, not 0 -- Skeeter is the only one of the four water classes
 *     that passes one, and it is read off the source, not assumed.
 *
 * (2) src/func_ov090_02131e00.cpp -- THE PMF DISEASE, the SoundObject / Cap /
 *     MrBlizzard / BabyPenguin / Unagi / HootTheOwl / Lakitu shape. The source
 *     forms
 *         struct C; typedef int (C::*PMF)();
 *         struct C { char pad[0x370]; PMF *pp; };
 *         c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)();
 *     -- a real C++ pointer-to-member on a DELIBERATELY INCOMPLETE class.
 *     MSVC's representation for a pointer-to-member of an incomplete class is
 *     the "unknown inheritance" general form, four words, not the ROM's own
 *     8-byte {function, delta} record. This is the STATE SETTER: it stores the
 *     cell pointer at self+0x370 and then dispatches record 0, the ENTER half.
 *     Every delta word in ov090's sixteen PMF source records is 0 in the ROM
 *     (verified against overlay_0090.bin, see port/ov090_syms.txt), so a plain
 *     call through the fn word is the same call.
 *
 * (3) src/_ZN7Skeeter8BehaviorEv.cpp -- the same disease at the TICK half.
 *     Its `PmfNode { char pad[8]; ActorFn fn; }` reaches record 1 of whichever
 *     cell self+0x370 points at. Transcribed body-for-body from the matched
 *     source with ONLY that call replaced; every other statement, offset and
 *     constant is the source's.
 *
 * THE SEAT that makes (2) and (3) legal is port_ov090_states_seat() in
 * hal/actor_classes_ov090.cpp: the mounted PMF SOURCE records hold DS CODE
 * addresses (ovdata.py's pointer pass only rebases pointers into other mounted
 * DATA), so each record's fn word is rewritten with its HOST body's address --
 * validated against the ROM's own address first -- BEFORE
 * __sinit_ov090_02133ce8 copies Skeeter's eight records into the four bss
 * cells data_ov090_021344e4 / _021344f4 / _02134504 / _02134514.
 */
#include "ModelAnim.h"

extern "C" {

/* _ZN7Skeeter6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* ---- (2) and (3) the two state dispatch sites -------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* func_ov090_02131e00 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov090_02131e00.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
/* ---- (3) Behavior, transcribed with the record-1 dispatch hosted -------- */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
struct SkVec3 { s32 x, y, z; };

int  _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *wm);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void func_ov090_02131e50(void *c);
int  _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void *self, void *wm, void *anim, u32 n);
void func_02035684(int *p, int v);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, u32 n);
int  _ZN5Enemy11UpdateDeathER12WithMeshClsn(void *self, void *wm);
void func_ov090_02131378(void *c);
void _ZN5Actor8PoofDustEv(void *self);
void _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);
int  AngleDiff(int a, int b);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *v, u32 n, s32 fix, s16 s);
void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cyl);
int  func_02035638(u8 *p);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void func_020aea30(void *self, void *p, int n);
u16  DecIfAbove0_Short(u16 *p);
void _ZN9Animation7AdvanceEv(void *self);
void func_ov090_021310b4(void *c);
extern signed char data_0209f2f8;

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN7Skeeter8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}  /* extern "C" */
