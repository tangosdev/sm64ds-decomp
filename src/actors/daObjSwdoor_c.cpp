//cpp
/* The switch-operated shutters' shared base -- ov002/daObjSwdoor_c.
 *
 * The class is abstract in slots 0 (InitResources), 3 (CleanupResources) and 6
 * (Behavior), so its setup, teardown and per-frame step are not vtable slots at
 * all: each of the two leaves -- daObjBSwdoor_c (ov014) and daObjCvShutter_c
 * (ov021) -- calls func_ov002_020bad10, func_ov002_020baba8 and
 * func_ov002_020bac18 below with its own file table. Render (slot 9) is the one
 * real slot this class fills. include/daObjSwdoor_c.h carries the class banner.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here.
 *
 * Folded from four one-function sources, which this promotion deletes. They are
 * listed by the symbol each one carried, in ROM address order:
 *   [0] 0x020baba8  func_ov002_020baba8
 *   [1] 0x020babf0  _ZN13daObjSwdoor_c6RenderEv
 *   [2] 0x020bac18  func_ov002_020bac18
 *   [3] 0x020bad10  func_ov002_020bad10
 *
 * TWO OF THOSE FOUR WERE NOT IN THE CANDIDATE SPAN. build/tu_map.json bounds
 * this class's run at Render's end, because func_ov002_020bac18 carries no
 * mangled name and no vtable slot and so no label bounds the run on the right.
 * The manifest's boundary_evidence records the tree-wide call-site census that
 * moves the right edge out to 0x020badd0, and the base-rate control that makes
 * that census discriminating.
 *
 * THE DESTRUCTOR PAIR IS NOT CLAIMED. _ZN13daObjSwdoor_cD0Ev (0x020bab0c) and
 * _ZN13daObjSwdoor_cD1Ev (0x020bab64) sit immediately below this range and keep
 * their own delinks entries. This TU emits both anyway -- Render is the class's
 * key function, so _ZTV13daObjSwdoor_c is anchored here and its slots 16 and 17
 * odr-use them -- but the cartridge orders them D0-then-D1 and mwccarm emits the
 * vague-linkage pair of an inline-in-class destructor D1-then-D0 in every source
 * form measured on THIS TU. The manifest's boundary_evidence records those
 * compiles and why the forms that emit D0 first are refuted by the ROM. Nothing
 * here forces or reorders them; the copies are licensed as deadstrip-duplicate
 * against their ROM homes.
 */

#include "daObjSwdoor_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* The externs the two .c-derived members below arrived with. Two of them
 * disagreed across the pre-fold shards -- the shard for 0x020bac18 spelled
 * dBgActor_c::UpdateModelPosAndRotY as returning void and the shard for
 * 0x020bad10 spelled it as returning int -- and one TU can only carry one
 * spelling. The `int` form is kept because both call sites discard the
 * result, so the choice is codegen-neutral; it is the only reconciliation this
 * fold makes. */
extern "C" {
unsigned int _ZN5Event6GetBitEj(unsigned int bit);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int bankId, const Vector3 *pos);
int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *thiz);
int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *thiz);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
int func_020393d4(void *, void *);
int _ZN4dBgW6EnableEP8dActor_c(void *, void *);
int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020bad10, 0x020bad10, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bad10
/* The shared resource setup every daObjSwdoor_c leaf calls: load the model file
 * into dBgActor_c's Model at 0xd4, place it, load the collision file into the
 * dBgW_KcMbg at 0x124 with the leaf's CLPS block, hand the mesh collider its
 * per-frame update hook and enable it, then seed the four bytes the leaves add
 * -- the open/close sense at 0x31e and the shutter state at 0x31f out of
 * dBgActor_c's tail padding, the leaf's mTimer at 0x320 and its mEventBit at
 * 0x321, all decoded out of the actor's spawn parameter word at 0x8.
 *
 * Reached only through the two leaves' own InitResources (ov014 0x021112a4 as a
 * direct call, ov021 0x02112ecc through a literal-pool load), so the
 * address-derived name and the void * signature stay: nothing in the ROM names
 * it and no vtable slot holds it.
 *
 * SPELLINGS ARE THE PRE-FOLD ONES, deliberately, apart from one declaration: the
 * shard spelled parameter 4 of dBgW_KcMbg::SetFile through a file-local
 * `typedef int Fix12`, and `Fix12` is a class template once the header is in
 * scope, so the parameter is spelled `int` -- the type the typedef named --
 * directly. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov002_020bad10(void* c, void** f) {
  void* mdl;
  void* kcl;
  mdl = _ZN5Model8LoadFileER13SharedFilePtr(f[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, mdl, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(f[1]);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, kcl, (char*)c+0x2ec, 0x1000, *(short*)((char*)c+0x8e), f[2]);
  func_020393d4((char*)c+0x124, &_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
  _ZN4dBgW6EnableEP8dActor_c((char*)c+0x124, c);
  *(unsigned char*)((char*)c+0x31e) = *(int*)((char*)c+8) & 1;
  *(unsigned char*)((char*)c+0x321) = (*(unsigned int*)((char*)c+8) >> 1) & 0x1f;
  *(unsigned char*)((char*)c+0x31f) = 0;
  *(unsigned char*)((char*)c+0x320) = 5;
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020bac18, 0x020bac18, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bac18
/* The per-frame step the leaves install in the Behavior slot this class leaves
 * null: a three-state shutter. State 0 waits on the event bit the setup half
 * decoded, counts the leaf's mTimer down and, at zero, arms the travel timer to
 * 0x40, clears the actor's bit 0 at 0xb0 and plays bank-3 sound 0x3f from the
 * actor's position. State 1 walks the yaw at 0x8e by 0x100 a frame in whichever
 * direction 0x31e selected until the timer runs out, then sets the bit back.
 * State 2 is the terminal state and does nothing. Every state ends by putting the model
 * back where dBgActor_c says the actor is.
 *
 * Reached only through the two leaves' own Behavior (ov014 0x02111274 and
 * ov021 0x02112e88, both direct calls), so the address-derived name and the
 * char * signature stay: nothing in the ROM names it and no vtable slot holds
 * it -- slot 6 is a bare zero word in this class's table.
 *
 * SPELLINGS ARE THE PRE-FOLD ONES, deliberately, apart from two declarations
 * that could not survive the fold: the shard's file-local `typedef unsigned char
 * u8` and `typedef struct { int x, y, z; } Vector3` both name types the header
 * already supplies, so they are dropped and the header's spellings used. The
 * `LM`/`LMS`/`LMI` launder macros and the raw offsets are kept exactly as they
 * were -- writing either through a member changes what mwccarm CSEs. */
extern "C" {  /* .c-derived member: C linkage for the whole block */

#define LM(p) ((u8*)(int)(p))
#define LMS(p) ((short*)(int)(p))
#define LMI(p) ((int*)(int)(p))

int func_ov002_020bac18(char *c)
{
    u8 state = *(u8*)(c + 0x31f);

    switch (state) {
    case 0:
        if (_ZN5Event6GetBitEj(*(u8*)(c+0x321)) != 0) {
            if (DecIfAbove0_Byte((u8*)(c+0x320)) == 0) {
                *(u8*)(c+0x320) = 0x40;
                (*LM(c+0x31f))++;
                *LMI(c+0xb0) &= ~1;
                _ZN5Sound9PlayBank3EjRK7Vector3(0x3f, (const Vector3*)(c+0x74));
            }
        }
        break;
    case 1:
        if (DecIfAbove0_Byte((u8*)(c+0x320)) != 0) {
            if (*(u8*)(c+0x31e) != 0) {
                *LMS(c+0x8e) += 0x100;
            } else {
                *LMS(c+0x8e) -= 0x100;
            }
        } else {
            (*LM(c+0x31f))++;
            *LMI(c+0xb0) |= 1;
        }
        break;
    case 2:
        break;
    }

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN13daObjSwdoor_c6RenderEv, 0x020babf0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjSwdoor_c6RenderEv
/* daObjSwdoor_c::Render -- vtable slot 9, ov002 0x020babf0.
 * This class's only real slot; declaring it out of line makes this file the
 * key function (see include/daObjSwdoor_c.h). Render dispatches through
 * dBgActor_c's own mModel (0xd4), as every sibling in this series does. */
s32 daObjSwdoor_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- func_ov002_020baba8, 0x020baba8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020baba8
/* The teardown half of the setup at 0x020bad10: disable the mesh collider if it
 * is live, then release the leaf's two SharedFilePtrs. Reached only through a
 * literal-pool load in each of the two leaves (ov014 0x02111260,
 * ov021 0x02112e74). Instruction-for-instruction the same body every class in
 * this ov002 platform series carries its own copy of, which is why each copy
 * sits inside its own class's run rather than being shared. */
extern "C" {
int func_ov002_020baba8(char* c, void** p){
  if(((dBgW *)(c+0x124))->IsEnabled())
    ((dBgW *)(c+0x124))->Disable();
  ((SharedFilePtr *)(p[0]))->Release();
  ((SharedFilePtr *)(p[1]))->Release();
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* BELOW THIS RANGE, NOT CLAIMED -- _ZN13daObjSwdoor_cD0Ev (0x020bab0c) and
 * _ZN13daObjSwdoor_cD1Ev (0x020bab64).
 *
 * NOT WRITTEN OUT. ~daObjSwdoor_c is defined inline in the class body --
 * include/daObjSwdoor_c.h records why: both descendants inline its vptr store
 * instead of calling it, which the compiler can only do from a visible body.
 * Render above is this class's key function, so this TU emits
 * _ZTV13daObjSwdoor_c and its slots 16 and 17 odr-use the two variants anyway.
 * Their emission ORDER is the compiler's, not the source's, and it does not
 * agree with the cartridge; see the file banner.
 * -------------------------------------------------------------------------- */
