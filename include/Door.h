#ifndef DOOR_H
#define DOOR_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

/* The plain warp door -- the leaf dActor_c child at ov100 0x021443f4..0x02145948,
 * distinct from StarDoor and VirtualDoor which are their own classes/headers.
 *
 * DERIVATION. tools/rtti_extract.py has the RTTI record at ov100 0x02148158,
 * mangled "8daDoor_c" -- the ROM struct name, which is NOT the name this
 * header uses -- with ONE base, dActor_c, at subobject offset 0.
 *
 * "Door" is a coined name, and the cartridge disagrees with it: _ZTS8daDoor_c
 * at ov100 0x0214814c is a data symbol whose BYTES spell "8daDoor_c", so
 * renaming this class to daDoor_c is what would let that string be
 * data-verified. The tree is currently inconsistent about it -- _ZTV4Door
 * (coined, from the earlier destructor-pair slice) sits three symbols away
 * from _ZTS8daDoor_c and _ZTI8daDoor_c (the ROM's own). That inconsistency
 * predates this header and is deliberately not resolved here; folding both
 * consumers onto this one header is what makes resolving it a single rename
 * instead of two. It is a leaf: nothing in the image derives from it.
 *
 * VTABLE. data_02148188 (_ZTV4Door) is 31 slots, the same count as dActor_c's
 * own table -- confirmed with tools/rtti_vtables.py --own daDoor_c, which also
 * shows the destructor pair already migrated under the coined name "Door"
 * (_ZN4DoorD1Ev / _ZN4DoorD0Ev, ov100 0x021443f4 / 0x02144424) by earlier
 * work, before this class had its own header. This class overrides five
 * slots beyond the destructor:
 *
 *   0   InitResources      ov100 0x021455a0  (src/_ZN4Door13InitResourcesEv.c)
 *   3   CleanupResources   ov100 0x0214542c  (src/_ZN4Door16CleanupResourcesEv.cpp)
 *   6   Behavior           ov100 0x02145550  (src/_ZN4Door8BehaviorEv.cpp)
 *   9   Render             ov100 0x021454c8  (src/_ZN4Door6RenderEv.cpp)
 *   12  OnPendingDestroy   ov100 0x021454c4  (src/_ZN4Door16OnPendingDestroyEv.c)
 *
 * (config/arm9/overlays/ov100/relocs.txt: 0x02148188/0x02148194/0x021481a0/
 * 0x021481ac/0x021481b8 -- the vtable words at slots 0/3/6/9/12 -- each load
 * exactly the addresses above.) All five bytes still match; only the symbol
 * NAMES were placeholders (func_ov100_0214xxxx), now renamed.
 *
 * REAL METHOD STATUS. CleanupResources and Behavior are genuine `Door::`
 * definitions. InitResources, Render and OnPendingDestroy still use the
 * historical free-function ABI spelling; each needs its own byte-verified
 * conversion rather than being inferred from the declaration alone.
 *
 * THEIR FIELD ACCESS, HOWEVER, IS NOW THIS HEADER'S. The naming slice left
 * Door described by two headers at once: this one, and the generated flat
 * placeholder include/daDoor_c.h, which restated dActor_c's fields inline as
 * pad_000[0x5c] + unk_05c/unk_060/... and which _ZN4Door6RenderEv.cpp and
 * _ZN4Door13InitResourcesEv.c both included. daDoor_c.h is DELETED; both
 * files now take a Door and name their fields, through the C++ class below
 * or the C-mode branch after it. Every offset is unchanged and both files
 * were re-measured: Render is byte-exact under the pinned 2004/b56, and
 * InitResources compiles to output IDENTICAL to its pre-fold bytes.
 *
 * Only src/_ZN4Door13InitResourcesEv.c ALREADY FAILED TO BYTE-MATCH before
 * the rename -- config/arm9/overlays/ov100/delinks.txt carries no `complete`
 * marker for it, so dsd supplies that range from the cartridge and the ROM
 * build never compiles the file -- and stays that way. The gap is one
 * instruction: candidate 0x300 against the ROM's 0x2fc, measured before and
 * after the fold and unchanged by it. build_pin reports that as "999 word(s)
 * differ", which is match.py's sentinel for "the sizes differ at all", not a
 * count.
 *
 * SIZE. Door_Spawn.c calls `_ZN7fBase_cnwEj(328)` -- 0x148 -- for a fresh Door,
 * then _ZN8dActor_cC2Ev and _ZN9ModelAnimC1Ev at +0xd4. dActor_c is 0xd0
 * (include/dActor_c.h) and ModelAnim is 0x64 (include/ModelAnim.h), so the
 * embedded ModelAnim runs 0xd4..0x138 (the same 4-byte alignment pad
 * include/dBgActor_c.h takes before its own Model member). That leaves
 * 0x138..0x147 (0x10 = 16 bytes) as this class's own storage, all of it
 * touched by the five sources above: two heap-owned pointers at 0x138/0x13c,
 * a callback-node pointer at 0x140 (read in Behavior as a pointer-to-member
 * dispatch, written in src/func_ov100_021453d8.cpp -- out of this slice), and
 * a key-model index byte at 0x144.
 *
 * 0x138 IS A Model*, and this header used to say the opposite -- that the
 * virtual calls through it "resolve to unidentified Model vtable slots" and
 * typing it was future work. They were never unidentified. include/Model.h
 * names slot 4 Virtual10(Matrix4x3&) at vtable offset 0x10 and slot 5
 * Render(const Vector3*) at 0x14, and those are exactly the two the ROM
 * dispatches in _ZN4Door6RenderEv (`ldr r2,[r2,#0x10]`, `ldr r2,[r2,#0x14]`);
 * CleanupResources' third call is slot 1, the deleting destructor. Four
 * independent things agree the object is a Model and not merely Model-shaped:
 * InitResources allocates it `_Znwj(0x50)` and sizeof(Model) is 0x50; it runs
 * _ZN5ModelC1Ev on the result; it stores a matrix at +0x1c, which is
 * Model::mat4x3; and Render hands it this door's own bone transforms. Typed
 * below, and the shadow structs both consumers carried for it are gone.
 * 0x13c and 0x140 stay void* -- 0x13c is written from both a SharedFilePtr
 * and an int global, so typing it is a separate question.
 *
 * Field NAMES elsewhere are placeholders and cannot change codegen. Offsets
 * and widths are observed.
 */

#ifdef __cplusplus

struct Door : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by Door_Spawn.c's own _ZN9ModelAnimC1Ev call at +0xd4 -- a
       relocation the ROM build checks, same idiom as include/dBgActor_c.h's
       mModel. */
    ModelAnim mModel;        /* 0x0d4 */

    /* This class's own storage, 0x138..0x147 -- see SIZE above.
       The decoration group. A door variant may hang a second model off itself:
       the key hanging on a locked door, or the star on a star door. Only
       InitResources fills it and only when data_ov100_02148204[param1] carries
       a second file (`e->sfp2`).
         mKeyModel  -- `new Model` + ModelBase::SetFile in InitResources,
                       Model::Virtual10(mModel.data.transforms) + Render in
                       Render (the local there is literally called `key`), and
                       `delete key` through Model's vtable slot 1 in
                       CleanupResources. Owned by this class.
         mKeyFile   -- the SharedFilePtr that model's file came from, handed to
                       Model::LoadFile and Release()d in CleanupResources. Three
                       sources: data_ov002_0211094c when the entry's b8 is
                       positive, func_02132894[mKeyModelIdx + 1] for the
                       param1 9..0xd (keyed-door) range, else
                       data_ov089_02132c50.
         mKeyModelIdx -- param1 - 8 for that same 9..0xd range, re-zeroed for
                       param1 0xc; indexes LoadKeyModels/func_02132894.
       [_ZN4Door13InitResourcesEv.c, _ZN4Door6RenderEv.cpp,
        _ZN4Door16CleanupResourcesEv.cpp] */
    Model *mKeyModel;          /* 0x138 -- owned, see SIZE above */
    void *mKeyFile;           /* 0x13c -- released through SharedFilePtr */
    /* Behavior casts this to a node whose +0x8 is a `void (Door::*)(int)` and
       calls it on this Door; written by src/func_ov100_021453d8.cpp.
       [_ZN4Door8BehaviorEv.cpp] */
    void *mCallbackNode;           /* 0x140 -- callback-node pointer, see SIZE above */
    s8   mKeyModelIdx;            /* 0x144 -- key-model index */
    u8   pad_145[0x3];

    /* --- vtable. Declared first, deliberately -- it is already the key
       function (see DERIVATION above): _ZN4DoorD1Ev.c / _ZN4DoorD0Ev.c define
       it as extern "C" free functions, never as a real `Door::~Door()`, so
       nothing here changes which TU the vtable is emitted from. --- */
    virtual ~Door();

    /* --- overrides of inherited fBase_c slots dActor_c left untouched (see
       include/dActor_c.h: "Slots 0, 3, 6, 9, 12 ... still point at the
       fBase_c implementations"). CleanupResources and Behavior are real
       Door methods; the remaining overrides retain their historical ABI
       spellings until each can be converted without changing ROM bytes. --- */
    virtual s32 InitResources();          /* slot 0 */
    virtual s32 CleanupResources();       /* slot 3 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
    virtual void OnPendingDestroy();      /* slot 12 */
};

/* Holds the chain to the size Door_Spawn.c's operator new(0x148) call
   evidences. A silently-added member anywhere fails this. */
typedef char Door_size_must_be_0x148[sizeof(Door) == 0x148 ? 1 : -1];

#else

/* Flat layout for the C translation units, which can express neither the base
   class nor the virtual functions -- the same split include/dActor_c.h and
   include/ModelAnim.h already carry, and for the same reason.

   This branch is what retired include/daDoor_c.h, the generated flat
   placeholder this class used to be described by. That header restated
   dActor_c's fields inline as pad_000[0x5c] + unk_05c/unk_060/... , so the
   two spellings of one object could drift and the C sources could not see
   that 0x05c..0x0cf is inherited storage dActor_c has already named. Nesting
   the bases instead makes drift impossible: the offsets below are not
   written down here at all, they are whatever dActor_c and ModelAnim say.

   Every field daDoor_c.h named has a home: 0x05c/0x060/0x064 are
   base.mPosX/Y/Z, 0x080/0x084/0x088 base.mScaleX/Y/Z, 0x08c/0x08e/0x090
   base.mAngleX/Y/Z, 0x0a4/0x0a8/0x0ac base.unk_0a4/mVertSpeed/unk_0ac,
   0x0e8 mModel.data.transforms, and mKeyModelIdx is this class's own and keeps
   its name. */
struct Door {
    struct dActor_c base;    /* 0x000..0x0cf */
    u8  pad_0d0[0x4];
    ModelAnim mModel;        /* 0x0d4..0x137 */
    Model *mKeyModel;          /* 0x138 -- owned, see SIZE above */
    void *mKeyFile;           /* 0x13c -- released through SharedFilePtr */
    void *mCallbackNode;           /* 0x140 -- callback-node pointer, see SIZE above */
    s8   mKeyModelIdx;            /* 0x144 -- key-model index */
    u8   pad_145[0x3];
};

/* The C++ branch's assert, restated over the nested spelling: if either base
   changes width the sum stops being 0x148 and this branch fails to compile,
   which is the whole point of nesting them rather than restating offsets. */
typedef char Door_size_must_be_0x148[sizeof(struct Door) == 0x148 ? 1 : -1];

/* So a source declaring a Door reads the same in both modes, the way
   include/ModelAnim.h does it. */
typedef struct Door Door;

#endif /* __cplusplus */

#endif /* DOOR_H */
