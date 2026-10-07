#ifndef DADOOR_C_H
#define DADOOR_C_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

class Player;

/* The plain warp door -- the leaf dActor_c child whose code is the ov100
 * linker unit 0x021443f4..0x021458d4, src/actors/daDoor_c.cpp (its registry
 * factory included; daStarGate_c starts at 0x021458d4). Distinct from
 * StarDoor and daChRoom_c, which are their own classes/headers.
 *
 * DERIVATION. tools/rtti_extract.py has the RTTI record at ov100 0x02148158,
 * mangled "8daDoor_c", with ONE base, dActor_c, at subobject offset 0.
 *
 * NAME. The cartridge names this class: _ZTS8daDoor_c at 0x0214814c,
 * _ZTI8daDoor_c at 0x02148158, and the vtable _ZTV8daDoor_c at 0x02148188
 * that the factory and destructor store. Earlier work coined the name
 * "Door" for it (_ZTV4Door, _ZN4DoorD1Ev and so on); every one of those now
 * spells the ROM's name. It is a leaf: nothing in the image derives from it.
 *
 * VTABLE. data_02148188 (_ZTV8daDoor_c) is 31 slots, the same count as dActor_c's
 * own table -- confirmed with tools/rtti_vtables.py --own daDoor_c, which also
 * shows the destructor pair already migrated as a method pair
 * (_ZN8daDoor_cD1Ev / _ZN8daDoor_cD0Ev, ov100 0x021443f4 / 0x02144424) by earlier
 * work, before this class had its own header. This class overrides five
 * slots beyond the destructor:
 *
 *   0   InitResources      ov100 0x021455a0
 *   3   CleanupResources   ov100 0x0214542c
 *   6   Behavior           ov100 0x02145550
 *   9   Render             ov100 0x021454c8
 *   12  OnPendingDestroy   ov100 0x021454c4
 *
 * all five in src/actors/daDoor_c.cpp.
 *
 * (config/arm9/overlays/ov100/relocs.txt: 0x02148188/0x02148194/0x021481a0/
 * 0x021481ac/0x021481b8 -- the vtable words at slots 0/3/6/9/12 -- each load
 * exactly the addresses above.)
 *
 * REAL METHOD STATUS. The destructor and all five overrides are genuine
 * `daDoor_c::` definitions in src/actors/daDoor_c.cpp, byte-exact under the
 * pinned 2004/b56 and built from that one translation unit. InitResources
 * was the last to convert: it was a C free function taking the object
 * explicitly, and its param1 shift keeps the redundant cast that reaches the
 * ROM's folded read-modify-write (described at that line).
 *
 * The flat C placeholder that once restated dActor_c's fields inline
 * (pad_000[0x5c] + unk_05c/unk_060/...) is gone; every member names its
 * fields through the class below.
 *
 * SIZE. daDoor_c_classInit's `new daDoor_c` calls fBase_c's operator new
 * with 328 -- 0x148 -- then _ZN8dActor_cC2Ev and _ZN9ModelAnimC1Ev at +0xd4. dActor_c is 0xd0
 * (include/dActor_c.h) and ModelAnim is 0x64 (include/ModelAnim.h), so the
 * embedded ModelAnim runs 0xd4..0x138 (the same 4-byte alignment pad
 * include/dBgActor_c.h takes before its own Model member). That leaves
 * 0x138..0x147 (0x10 = 16 bytes) as this class's own storage, touched by the sources in src/actors/daDoor_c.cpp: two pointers at
 * 0x138/0x13c, a state pointer at 0x140 (read in Behavior as a
 * pointer-to-member dispatch, written by func_ov100_021453d8), a key-model
 * index byte at 0x144 and a countdown byte at 0x145 (touched only by the
 * state helpers). 0x146..0x147 are untouched padding.
 *
 * 0x138 IS A Model*, and this header used to say the opposite -- that the
 * virtual calls through it "resolve to unidentified Model vtable slots" and
 * typing it was future work. They were never unidentified. include/Model.h
 * names slot 4 Virtual10(Matrix4x3&) at vtable offset 0x10 and slot 5
 * Render(const Vector3*) at 0x14, and those are exactly the two the ROM
 * dispatches in _ZN8daDoor_c6RenderEv (`ldr r2,[r2,#0x10]`, `ldr r2,[r2,#0x14]`);
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
 *
 * SM64DS RTTI names the implementation daDoor_c. The reconstructed
 * factory daDoor_c_classInit (historical alias
 * Door_Spawn) constructs it for the DOOR
 * registry profile.
 */

#ifdef __cplusplus

struct daDoor_c : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by daDoor_c_classInit's own _ZN9ModelAnimC1Ev call at +0xd4 -- a
       relocation the ROM build checks, same idiom as include/dBgActor_c.h's
       mModel. */
    ModelAnim mModel;        /* 0x0d4 */

    /* This class's own storage, 0x138..0x147 -- see SIZE above.
       The decoration group. A door variant may hang a second model off itself. Only
       InitResources fills it and only when data_ov100_02148204[param1] carries
       a second file (`e->keyFile`). Which doors carry one: variant 1, the
       star doors (the file is chosen by star count) and the key doors (one
       shared file); what each of those models shows is not identified.
         mKeyModel  -- `new Model` + ModelBase::SetFile in InitResources,
                       Model::Virtual10(mModel.data.transforms) + Render in
                       Render (the local there is literally called `key`), and
                       `delete key` through Model's vtable slot 1 in
                       CleanupResources. Owned by this class.
         mKeyFile   -- a separate file the door pre-loads (Model::LoadFile) only
                       in game mode 0 and Release()s in CleanupResources; it is
                       not mKeyModel's file (that is e->keyFile). Three
                       sources: data_ov002_0211094c when the entry's
                       starsNeeded is positive, data_ov089_02132894[mKeyModelIdx
                       + 1] for a key door in the param1 9..0xd range, else
                       data_ov089_02132c50; it stays null for a door with
                       neither stars nor a key.
         mKeyModelIdx -- param1 - 8 for the 9..0xd range, re-zeroed for param1
                       0xc; mKeyModelIdx + 1 (taken before that re-zero, so
                       param1 - 7) indexes LoadKeyModels/data_ov089_02132894.
       [src/actors/daDoor_c.cpp] */
    Model *mKeyModel;          /* 0x138 -- owned, see SIZE above */
    void *mKeyFile;           /* 0x13c -- released through SharedFilePtr */
    /* The current state: a 16-byte {enter, execute} pair of
       pointers-to-member, one of the nine tables data_ov100_021488a4 ..
       data_ov100_02148924. func_ov100_021453d8 stores it and calls the enter
       half; Behavior calls the execute half, a `void (daDoor_c::*)(int)` at
       +0x8, on this daDoor_c. [src/actors/daDoor_c.cpp] */
    void *mState;                 /* 0x140 -- the state pair, see SIZE above */
    s8   mKeyModelIdx;            /* 0x144 -- key-model index */
    /* A countdown/phase byte. func_ov100_02144528 and func_ov100_02145080 tick
       it with DecIfAbove0_Byte and also set it directly (0x40 and 0x78 ticks,
       or the 0/1 result of Player::TryExitWhiteDoorWithStar), and it is zeroed
       when a swing starts. The comments in src/actors/daDoor_c.cpp say what
       each does with it. */
    u8   mTimer;                  /* 0x145 */
    u8   pad_146[0x2];

    /* --- vtable. The out-of-line destructor is the key function:
       src/actors/daDoor_c.cpp defines it, so that TU emits D1, D0, the vtable
       and the RTTI. --- */
    virtual ~daDoor_c();

    /* --- overrides of inherited fBase_c slots dActor_c left untouched (see
       include/dActor_c.h: "Slots 0, 3, 6, 9, 12 ... still point at the
       fBase_c implementations"), all five real daDoor_c methods. --- */
    virtual s32 InitResources();          /* slot 0 */
    virtual s32 CleanupResources();       /* slot 3 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
    virtual void OnPendingDestroy();      /* slot 12 */

    /* The helpers the state tables' pointer-to-member pairs name, and the
       ones those call: each takes the door as its implicit this exactly
       where the old C sources took it explicitly. The state record's
       execute half always receives the player (as Player* or, where the
       body only forwards it, the int the dispatch carries). The original
       names are not recovered; the addresses stand in. */
    int func_ov100_02144468(int p);         /* state 021488a4 execute */
    int func_ov100_021444e8(int a1);        /* state 02148924 execute */
    int func_ov100_02144528(Player *pl);    /* state 02148904 execute */
    void func_ov100_021446f8(Player *r1);   /* state 02148904 enter */
    int func_ov100_02144730(Player *arg1);  /* states 021488f4/02148914 execute */
    int func_ov100_0214491c();              /* state 02148914 enter */
    int func_ov100_02144950(Player *pl, int unused); /* state 021488f4 enter */
    int func_ov100_021449c8(Player *a2);    /* state 021488e4 execute */
    int func_ov100_02144a38(Player *p);     /* state 021488e4 enter */
    int func_ov100_02144bf4(Player *a2);    /* state 021488d4 execute */
    int func_ov100_02144c64();              /* state 021488d4 enter */
    int func_ov100_02144c6c(Player *r1);    /* state 021488c4 execute */
    int func_ov100_02144ccc();              /* state 021488c4 enter */
    int func_ov100_02144cf8(Player *b);     /* state 021488b4 execute */
    int func_ov100_02145080(Player *arg1);  /* the exit timing */
    void func_ov100_02145170(Player *pl, Vector3 *a, Vector3 *b); /* place the player */
    int func_ov100_021451c4(void *r5, Player *r4);  /* send the player through */
    int func_ov100_021452e4(Player *r1);    /* player inside the door's box */
    Player *func_ov100_02145370();          /* the current player */
    int func_ov100_021453d8(void *p, int a2); /* the state installer */
};

/* Holds the chain to the size daDoor_c_classInit's operator new(0x148) call
   evidences. A silently-added member anywhere fails this. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDoor_c_size_must_be_0x148[sizeof(daDoor_c) == 0x148 ? 1 : -1];
#endif

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
   0x0e8 mModel.data.transforms, and mState, mKeyModelIdx and mTimer are
   this class's own. */
struct daDoor_c {
    struct dActor_c base;    /* 0x000..0x0cf */
    u8  pad_0d0[0x4];
    ModelAnim mModel;        /* 0x0d4..0x137 */
    Model *mKeyModel;          /* 0x138 -- owned, see SIZE above */
    void *mKeyFile;           /* 0x13c -- released through SharedFilePtr */
    void *mState;                 /* 0x140 -- the state pair, see SIZE above */
    s8   mKeyModelIdx;            /* 0x144 -- key-model index */
    u8   mTimer;                  /* 0x145 -- countdown byte */
    u8   pad_146[0x2];
};

/* The C++ branch's assert, restated over the nested spelling: if either base
   changes width the sum stops being 0x148 and this branch fails to compile,
   which is the whole point of nesting them rather than restating offsets. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDoor_c_size_must_be_0x148[sizeof(struct daDoor_c) == 0x148 ? 1 : -1];
#endif

/* So a source declaring a daDoor_c reads the same in both modes, the way
   include/ModelAnim.h does it. */
typedef struct daDoor_c daDoor_c;

#endif /* __cplusplus */

#endif /* DADOOR_C_H */
