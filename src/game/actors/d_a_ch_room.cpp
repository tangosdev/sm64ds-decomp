//cpp
/* An area-transition trigger box -- ov002/daChRoom_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x020b07f8..0x020b09b0, eight
 * functions and nothing else, and the production build links this object rather
 * than eight per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daChRoom_c') is d_a_ch_room.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov002 0x021085b8 is a
 * __si_class_type_info whose _ZTS at 0x021085c4 reads exactly `10daChRoom_c`,
 * and whose one base, at subobject offset 0, is arm9 0x0208e390 --
 * _ZTI8dActor_c. That is why the header says `struct daChRoom_c : dActor_c` and
 * why the RTTI below has ROM homes to be compared against at all: a coined name
 * is a length-prefixed mangled string that matches nothing at any address, so
 * it can never be word-compared, and a class whose records cannot be compared
 * cannot have a key-function TU.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * NAMING THE CLASS PUTS ITS VAGUE-LINKAGE DATA IN THIS OBJECT, because this TU
 * defines the key function. Nine records come out; romdata_check compares each
 * against the cartridge with relocations applied before production isolation
 * discards it:
 *
 *   _ZTV10daChRoom_c    ov002 0x021085f8  VERIFIED, 124 bytes = 31 slots
 *   _ZTI10daChRoom_c    ov002 0x021085b8  VERIFIED
 *   _ZTI8dActor_c / _ZTI7dBase_c / _ZTI7fBase_c   arm9   VERIFIED
 *   the four _ZTS records                                PARTIAL
 *   nothing                                              DIFFERS
 *
 * A tenth symbol comes out of this object that is not one of those records:
 * _ZN7Vector3D1Ev. Behavior declares two Vector3 locals, so the TU re-emits
 * Vector3's own vague-linkage destructor; the cartridge keeps one copy at arm9
 * 0x020072c0 and compiler_only_output carries it as deadstrip-duplicate, which
 * makes rombuild compare the body against those cartridge bytes before
 * discarding it.
 *
 * THE VTABLE CLAIM IS SCOPED TO THE SLOTS, deliberately. A symbols.txt _ZTV
 * address is the ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI pointer} header word pair is emitted here and
 * word-compared by nothing, so the 31 slots are proved and those two words are
 * not. The slots themselves were diffed against _ZTV8dActor_c and differ in
 * exactly the seven this class overrides -- see include/daChRoom_c.h.
 */

/* NO PRAGMAS. None of the eight legacy sources carried one, so there is no
 * file-global/positional question to resolve here. */

/* Includes: the union of the eight legacy files', reconciled -- the class's own
 * header now carries the real base clause and the five overrides, so nothing
 * here shadows it. */
#include "daChRoom_c.h"
#include "types.h"
#include "decl_common.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"

/* Local declarations that have no real header yet, carried from the legacy
 * files. Deliberately left as they are rather than unified with other files'
 * spellings of the same five symbols: the tree currently disagrees with itself
 * about every one of them (Vec3_Sub is declared with three different parameter
 * spellings elsewhere, data_0209f394 as an array of Player, void, HUDInfo or
 * Obj pointers, data_020a0ebc as char, int, Vector3 and Triple), there is no
 * header that owns them, and these are the spellings this run's bytes were
 * matched under. Picking a winner is a tree-wide change with its own verify
 * cycle, not a step on the way to a promotion. */
extern "C" {
extern void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
extern void Vec3_RotateYAndTranslate(Vector3* out, void* m, s16 ang, Vector3* in);
extern u8 data_0209f250;
extern char* data_0209f394[];
extern char data_020a0ebc;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daChRoom_c_classInit, 0x020b0980, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daChRoom_c_classInit
/* recovered: globals resolved, declarations from a shared header */
/* recovered: globals resolved */
/* resolved: VT = _ZTV10daChRoom_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Reconstructed source-style name: SM64DS proves daChRoom_c through RTTI,
 * allocation size, vtable identity, and the CH_ROOM registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daChRoom_c_Spawn. */
int *daChRoom_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(daChRoom_c));
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)(_ZTV10daChRoom_c + 2); }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN10daChRoom_c13InitResourcesEv, 0x020b0938, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daChRoom_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daChRoom_c::InitResources()
{
  mScaleX=(((param1&0xf)+1)*0x64000)>>1;
  mScaleY=(((param1>>4&0xf)+1)*0x64000);
  mAngleY=-mAngleY;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN10daChRoom_c8BehaviorEv, 0x020b0868, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daChRoom_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daChRoom_c::Behavior()
{
    char* obj;
    Vector3 d;
    Vector3 r;
    int v;

    obj = data_0209f394[data_0209f250];
    Vec3_Sub(&d, (Vector3*)(obj + 0x5c), (Vector3*)&mPosX);
    Vec3_RotateYAndTranslate(&r, &data_020a0ebc, mAngleY, &d);

    v = r.x;
    if (v < 0) v = -v;
    if (v < mScaleX) {
        if (r.y > -0x96000) {
            if (r.y < mScaleY) {
                int z = r.z;
                int az = (z < 0) ? -z : z;
                if (az > 0x64000 && az < 0x190000) {
                    int area = (z < 0) ? mAngleX : mAngleZ;
                    *(char*)(obj + 0xcc) = (char)area;
                    ChangeArea((char)area);
                }
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN10daChRoom_c6RenderEv, 0x020b0860, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daChRoom_c6RenderEv
int daChRoom_c::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN10daChRoom_c16OnPendingDestroyEv, 0x020b085c, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daChRoom_c16OnPendingDestroyEv
void daChRoom_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN10daChRoom_c16CleanupResourcesEv, 0x020b0854, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daChRoom_c16CleanupResourcesEv
int daChRoom_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN10daChRoom_cD0Ev, 0x020b081c, size 0x38 */
/* -------------------------------------------------------------------------- */
/* _ZN10daChRoom_cD0Ev (vtable slot 17, the deleting destructor) is NOT
 * hand-written here. A hand-written mangled D0 next to a real out-of-line D1
 * ICEs mwccarm 2004/b56 (ELFgen.c:483); the compiler synthesizes D0 itself from
 * D1. The two legacy files src/_ZN10daChRoom_cD0Ev.cpp and
 * src/_ZN10daChRoom_cD1Ev.cpp had each independently reconstructed the SAME
 * `daChRoom_c::~daChRoom_c(){}` -- the standard D0/D1 collapse artifact of one
 * destructor split across two one-function files. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN10daChRoom_cD1Ev, 0x020b07f8, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daChRoom_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vptr store, then the call into ~dActor_c.
 *
 * (no definition here: `virtual ~daChRoom_c() {}` is in include/daChRoom_c.h,
 * and that placement is load-bearing rather than stylistic -- out of line,
 * mwccarm emits D0 before D1 and adds a homeless D2, and objisolate then
 * refuses this whole TU. The header carries the reasoning and the leaf
 * measurement that makes it safe.) */
