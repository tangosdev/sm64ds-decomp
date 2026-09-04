//cpp
/* A sound-emitting level object -- ov002/daSoundObj_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x020f934c..0x020f975c, seven
 * functions and nothing else, and the production build links this object rather
 * than seven per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daSoundObj_c') is d_a_sound_obj.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov002 0x0210c048 is a
 * __si_class_type_info whose _ZTS at 0x0210c054 reads exactly `12daSoundObj_c`,
 * and whose one base, at subobject offset 0, is arm9 0x0208e390 -- _ZTI8dActor_c.
 * That is why the header says `struct daSoundObj_c : dActor_c` and why the RTTI
 * below has ROM homes to be compared against at all: a coined name is a
 * length-prefixed mangled string that matches nothing at any address, so it can
 * never be word-compared, and a class whose records cannot be compared cannot
 * have a key-function TU.
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
 *   _ZTV12daSoundObj_c    ov002 0x0210c0dc  VERIFIED, 124 bytes = 31 slots
 *   _ZTI12daSoundObj_c    ov002 0x0210c048  VERIFIED
 *   _ZTI8dActor_c / _ZTI7dBase_c / _ZTI7fBase_c   arm9   VERIFIED
 *   the four _ZTS records                                PARTIAL
 *   nothing                                              DIFFERS
 *
 * THE VTABLE CLAIM IS SCOPED TO THE SLOTS, deliberately. A symbols.txt _ZTV
 * address is the ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI pointer} header word pair is emitted here and
 * word-compared by nothing, so the 31 slots are proved and those two words are
 * not. The four PARTIAL rows are the known dsd extent shortfall on _ZTS records
 * -- a range that stops short, not a disagreement about bytes.
 */

/* THE ONE PRAGMA, RESOLVED. The legacy sources carried a single directive,
 * `#pragma opt_loop_invariants off` on InitResources, and it is the FILE-GLOBAL
 * last-wins kind (like opt_propagation and optimize_for_size), not the
 * positional kind (`long_calls`, which does not appear in this TU). Bracketing
 * it around that one member had no effect whatever -- the closing `on` set the
 * whole file's effective state back -- so it is left unbracketed below; see the
 * note there for why an `off` setting is safe for the other six members.
 */

/* Includes: the union of the seven legacy files', reconciled -- the class's own
 * header now carries the real base clause and the members, so nothing here
 * shadows it. */
#include "daSoundObj_c.h"
#include "decl_Actor.h"
#include "decl_common.h"

/* Local declarations that have no real header yet. The two typedefs are the
 * legacy files', confirmed byte-neutral against each other.
 *
 * TODO: SoundObjectCallbackOwner is very probably daSoundObj_c itself. The two
 * callbacks below take (self, u16 *) and read self+0xd4/0xd8/0xde/0xe0 -- the
 * four members this class declares -- and Behavior calls them with &mCounter,
 * which is this+0xdc. So they are two more members, and SoundObjectCallback is
 * plausibly `int (daSoundObj_c::*)(void *)` with the cast at its one use site
 * deleted. Not done here because it cannot be settled statically: the table at
 * data_ov002_0211110c lives in .bss (0x0210d9a0..0x021111a0), so it is
 * populated at runtime and its entries cannot be read out of the image. It is a
 * verify-cycle question -- change it and ask whether the seven functions still
 * byte-match -- not a change to make on the way to a merge. Until then the
 * shadow struct is a coined name, not a class the ROM spells. */
/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow struct 'SoundObjectCallbackOwner' */
struct SoundObjectCallbackOwner;

/* shadow typedef 'int' */
typedef int (SoundObjectCallbackOwner::*SoundObjectCallback)(void *);

extern "C" {
extern int _ZTV12daSoundObj_c[];
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist, int loop);
extern SoundObjectCallback data_ov002_0211110c[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
/* Both other legacy declarations of _ZN5Sound7PlaySubEjjj5Fix12IiEb (from the
 * files for ::Behavior and ::InitResources) had the same params under a
 * different spelling (bool vs int, Fix12i vs Fix12 -- both int) -- confirmed
 * byte-neutral, kept the one above. */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daSoundObj_c_Spawn, 0x020f972c, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daSoundObj_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(228);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)(_ZTV12daSoundObj_c + 2); }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12daSoundObj_c13InitResourcesEv, 0x020f95e0, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daSoundObj_c13InitResourcesEv
/* opt_loop_invariants is FILE-GLOBAL last-wins (applies to the whole TU,
 * not just code textually after it -- unlike #pragma long_calls). Left
 * unbracketed: no other member in this TU has a loop, so an "off" setting
 * anywhere in the file is safe for all of them. */
#pragma opt_loop_invariants off
int daSoundObj_c::InitResources()
{
    dActor_c *actor;
    int g;

    if (param1 > 7)
        return 0;

    mSoundID = *(s32 *)((char *)data_ov002_0210c080 + param1 * 0xc);
    mVolume = *(s32 *)((char *)data_ov002_0210c084 + param1 * 0xc);
    mCounterLimit = *(u16 *)((char *)data_ov002_0210c088 + param1 * 0xc);
    unk_0e0 = *(u8 *)((char *)data_ov002_0210c08a + param1 * 0xc);

    actor = 0;
    g = data_0208e430;

    if (g == 0x20 || g == 0x29 || g == 0x21 || g == 0x1e || g == 0x50 ||
        (g >= 0x19 && g <= 0x1d) || g == 0x4f || g == 0x22 || g == 0x2a || g == 0x21)
    {
        while (1)
        {
            actor = FindWithActorID(0x167, actor);
            if (actor == 0)
                break;
            if (actor != this)
            {
                actor->MarkForDestruction();
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(g, 0x7f, 0, 0x7f000, 0);
            }
        }
    }

    mCounter = 0;
    mAreaId = -1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12daSoundObj_c8BehaviorEv, 0x020f94fc, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daSoundObj_c8BehaviorEv
int daSoundObj_c::Behavior()
{
    SoundObjectCallbackOwner *owner = (SoundObjectCallbackOwner *)this;
    int result = (owner->*data_ov002_0211110c[param1])(&mCounter);
    if (result == 0 && mSoundID == data_0208e430
        && (mCounter <= 0xa || data_0209b49c > 0x7f)) {
        goto skip;
    }
    MarkForDestruction();
    if (data_0208e430 != 0x22) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(data_0208e430, 0x7f, 0, 0x7f000, 0);
    }
skip:
    if (param1 != 6) {
        if (data_0209b490 < mVolume)
            mCounter = mCounterLimit;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020f9468, 0x020f9468, size 0x94 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* kPoolDist is the tree's existing spelling for this literal, not coined here:
   src/func_0200f7f0.c and the two Sound::Play*SecretSound files give the same
   0x8777 the same name. Its sibling 0xcb33 in func_ov002_020f93a8 has no such
   precedent and is left bare. */
static const int kPoolDist = 0x8777;
int func_ov002_020f9468(char* a, unsigned short* counter){
  int ret = 0;
  if ((int)*counter < (int)*(unsigned short*)(a+0xde) - 0xf) {
    int v = *(int*)(a+0xd8);
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(*(unsigned int*)(a+0xd4), v, 0x7f, (0x7f - v)<<0xc, *(unsigned char*)(a+0xe0));
    *counter += 1;
    goto done;
  }
  if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(*(unsigned int*)(a+0xd4), 0x7f, 0, kPoolDist, *(unsigned char*)(a+0xe0)))
    return 1;
done:
  return ret;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020f93a8, 0x020f93a8, size 0xc0 */
/* -------------------------------------------------------------------------- */
extern "C" {
int func_ov002_020f93a8(char* c, unsigned short* p) {
    if (p[0] < *(unsigned short*)(c+0xde)) {
        int d8 = *(int*)(c+0xd8);
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(*(int*)(c+0xd4), d8, 0x7f, ((0x7f - d8) << 12) / 5, 1) != 0)
            *(unsigned short*)(c+0xde) = 1;
        p[0]++;
    } else {
        if (*(unsigned short*)(c+0xde) == 0) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(*(int*)(c+0xd4), 0x7f, 0, 0xcb33, 1) != 0)
                return 1;
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN12daSoundObj_cD0Ev, 0x020f9370, size 0x38 */
/* -------------------------------------------------------------------------- */
/* _ZN12daSoundObj_cD0Ev (vtable slot 17, the deleting destructor) is NOT
 * hand-written here. A hand-written mangled D0 next to a real out-of-line D1
 * ICEs mwccarm 2004/b56 (ELFgen.c:483); the compiler synthesizes D0 itself
 * from D1 once D1 is a real destructor -- see EnemySwitchTag for the same
 * shape. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN12daSoundObj_cD1Ev, 0x020f934c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daSoundObj_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, then the tail into ~dActor_c.
 *
 * (no definition here: `virtual ~daSoundObj_c() {}` is in
 * include/daSoundObj_c.h, and that placement is load-bearing rather than
 * stylistic -- out of line, mwccarm emits D0 before D1 and adds a homeless D2,
 * and objisolate then refuses this whole TU. The header carries the reasoning
 * and the leaf measurement that makes it safe.) */
