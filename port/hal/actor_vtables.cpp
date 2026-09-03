// Actor-hierarchy vtables (gate 9), per the vtable law (clsn_vtable.cpp):
// MSVC slot order. include/ActorBase.h declares the dtor LAST, so MSVC and
// the ROM agree on slots 0..15 and diverge only at the tail -- the header
// was built for exactly this. Lifecycle slots forward to the class's own
// overrides where they exist and to the ActorBase/Actor defaults where
// they do not; the tail traps.
//
// Base-class vtable symbols the ctor chain installs and then overwrites
// (never dispatched between installs) are plain storage.
#include <stdio.h>
#include "vs_width.h"   /* run vs16: the port's player width */
#include <stdlib.h>

#include "ActorBase.h"
#include "ArrowSignRight.h"
#include "dsstate_seg.h"

// The lifecycle definitions are MSVC methods (ArrowSignRight.h/ActorBase.h
// real classes); InitResources alone is a C-named free function. Every shim
// calls QUALIFIED -- never virtual.
extern "C" int _ZN14ArrowSignRight13InitResourcesEv(char *self);

static int __fastcall sl_init(void *self, void *)
{ return _ZN14ArrowSignRight13InitResourcesEv((char *)self); }
static int __fastcall sl_cleanup(void *self, void *)
{ return ((ArrowSignRight *)self)->ArrowSignRight::CleanupResources(); }
static int __fastcall sl_behavior(void *self, void *)
{ return ((ArrowSignRight *)self)->ArrowSignRight::Behavior(); }
static int __fastcall sl_render(void *self, void *)
{ return ((ArrowSignRight *)self)->ArrowSignRight::Render(); }
static int __fastcall sl_binit(void *self, void *)
{ return ((ActorBase *)self)->ActorBase::BeforeInitResources(); }
static void __fastcall sl_ainit(void *self, void *, u32 a)
{ ((ActorBase *)self)->ActorBase::AfterInitResources(a); }
static int __fastcall sl_bclean(void *self, void *)
{ return ((ActorBase *)self)->ActorBase::BeforeCleanupResources(); }
static void __fastcall sl_aclean(void *self, void *, u32 a)
{ ((ActorBase *)self)->ActorBase::AfterCleanupResources(a); }
static int __fastcall sl_bbeh(void *self, void *)
{ return ((ActorBase *)self)->ActorBase::BeforeBehavior(); }
static void __fastcall sl_abeh(void *self, void *, u32 a)
{ ((ActorBase *)self)->ActorBase::AfterBehavior(a); }
static int __fastcall sl_bren(void *self, void *)
{ return ((ActorBase *)self)->ActorBase::BeforeRender(); }
static void __fastcall sl_aren(void *self, void *, u32 a)
{ ((ActorBase *)self)->ActorBase::AfterRender(a); }
static int __fastcall sl_pdes(void *self, void *)
{ ((ActorBase *)self)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall sl_heap(void *self, void *)
{ return ((ActorBase *)self)->ActorBase::OnHeapCreated(); }

#define ATRAP(n) \
    static void __fastcall a_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: ArrowSignRight vtable slot %d trap\n", n); \
        abort(); }
ATRAP(13) ATRAP(14) ATRAP(16) ATRAP(17)
/* The interaction tail. _ZTV14ArrowSignRight is a 32-slot Platform table in
   the ROM (ov098 0x0213c3d8; the next-symbol bound reads 23 and is wrong),
   and this array was [20], so a tail dispatch read past the end. The registry
   fill in hal/actor_classes_bob_world.cpp writes real bodies over 18..31 for
   the game; these traps are what the gate-9 smoke -- which builds this table
   and never runs the registry -- gets instead of adjacent memory. */
ATRAP(18) ATRAP(19) ATRAP(20) ATRAP(21) ATRAP(22) ATRAP(23) ATRAP(24)
ATRAP(25) ATRAP(26) ATRAP(27) ATRAP(28) ATRAP(29) ATRAP(30) ATRAP(31)

extern "C" void *_ZTV14ArrowSignRight[32] = {
    (void *)sl_init,     /* 0  InitResources */
    (void *)sl_binit,    /* 1  BeforeInitResources */
    (void *)sl_ainit,    /* 2  AfterInitResources */
    (void *)sl_cleanup,  /* 3  CleanupResources */
    (void *)sl_bclean,   /* 4  BeforeCleanupResources */
    (void *)sl_aclean,   /* 5  AfterCleanupResources */
    (void *)sl_behavior, /* 6  Behavior */
    (void *)sl_bbeh,     /* 7  BeforeBehavior */
    (void *)sl_abeh,     /* 8  AfterBehavior */
    (void *)sl_render,   /* 9  Render */
    (void *)sl_bren,     /* 10 BeforeRender */
    (void *)sl_aren,     /* 11 AfterRender */
    (void *)sl_pdes,     /* 12 OnPendingDestroy */
    (void *)a_trap13, (void *)a_trap14,
    (void *)sl_heap,     /* 15 OnHeapCreated */
    (void *)a_trap16, (void *)a_trap17,
    (void *)a_trap18, (void *)a_trap19,
    /* 20..30 Actor's own interaction list, 31 the sign's own Kill (ov098
       0x02137ccc). The registry fill seats them; until it runs they trap. */
    (void *)a_trap20, (void *)a_trap21, (void *)a_trap22, (void *)a_trap23,
    (void *)a_trap24, (void *)a_trap25, (void *)a_trap26, (void *)a_trap27,
    (void *)a_trap28, (void *)a_trap29, (void *)a_trap30, (void *)a_trap31,
};

// Base vtables the ctor chain installs transiently: storage only.
DSSTATE_BEGIN
extern "C" {
/* ov002 0x0210ae38, dBgActor_c in the ROM's RTTI. THIRTY-TWO words: 18..30
   are Actor's own list and 31 is Platform::Kill. It was [20], so a Platform
   destructor that dispatched anything past 19 while this vptr was installed
   read the next symbol. hal_fill_platform_vtable (hal/actor_classes.cpp)
   fills it now, the same contents it gives _ZTV10dBgActor_c -- the two host
   arrays stand in for the SAME ROM table and only differ in which name a
   destructor TU spells. */
void *_ZTV8Platform[32];
int data_0208e4b8[20];   /* ActorBase-era vtable-ish install in Actor ctor */
/* _ZTV5Actor, the base Actor vtable. 31 slots (the full Actor table, not the
   20-slot ActorBase shape): STAR_CAMERA (gate 90) is a bare Actor that leaves
   this installed as its final vtable and dispatches all 31, so it is FILLED by
   hal_fill_actor_base_vtable in hal/actor_classes_star.cpp. For every other
   class it is still only the transient ctor install, overwritten before any
   dispatch -- filling it is invisible to them (see that file's proof).
   void*[] (not int[]) so the fill's pointer stores are the array's own type. */
void *data_0208e3a4[31];
}

// ---- ActorBase::ActorBase() transcription ---------------------------------
// The ROM ctor is a hand-asm block (src/_ZN9ActorBaseC1Ev.cpp); this is its
// C transcription, field for field against the disassembly there. The spawn
// CONTEXT globals it reads (pending actor ID, area byte, the spawn-info
// pointer table for the two processing-list priorities) are storage here;
// the smoke seeds them the way func_02010e78/ActorDerived::Spawn would.
extern "C" {
void _ZN9ActorBase9SceneNodeC1Ev(void *node);
int func_0203b438(void *a, void *b, void *c);
int func_02043810(void *p);

/* the transient ActorBase vtable install. EIGHTEEN words -- ActorBase's own
   virtual list runs 0..17 (arm9 0x02099edc) -- and this was [8], so a ctor-time
   dispatch of anything from OnPendingDestroy up read past the end. */
int data_02099edc[18];
int data_02099e70[1];           /* next unique actor id */
int data_020a4b60[1];
unsigned short data_020a4b54;   /* PENDING ACTOR ID (the spawn context) */
unsigned char data_020a4b48;    /* pending area byte */
int data_020a4b64[1];
int data_020a4b6c[8];           /* the scene tree root the ctor links into */
void *data_020a4bb8_storage[512];
void **data_020a4bb8 = data_020a4bb8_storage;  /* actorID -> SpawnInfo* */

/* PORT_HOST_ABI: the matched TU is an mwccarm `asm` block (ARM hand-asm); MSVC has no inline ARM assembler.
   src/_ZN9ActorBaseC1Ev.cpp is `extern "C" asm void* _ZN9ActorBaseC1Ev(void* self)`
   followed by 70-odd ARM instructions -- an asm-hatch TU, not C. It is a match
   under the asm-primitive policy and it is unbuildable by any host compiler:
   MSVC's __asm accepts x86 only, and the block is register-allocated ARM
   (ldr r1,=data_02099edc / strh r2,[r0,#0xc] / bl chains). Re-derived from the
   ROM for this ruling: arm9 0x02043dec (size 0x160) reads e92d4030 / e24dd004
   / e1a04000 / e59f112c / e2845014 / e1a00005 -- stmdb sp!,{r4,r5,lr}; sub
   sp,sp,#4; mov r4,r0; ldr r1,[pc,#0x12c]; add r5,r4,#0x14; mov r0,r5 --
   instruction for instruction the head of that asm block. Same class as
   func_020733a8 below, which carries the same tag. The C transcription above
   is the faithful stand-in, written field for field against that block.
   NOT a stub: it is a full transcription, and the reason it cannot be retired
   is the source language, not a missing closure. */
void *_ZN9ActorBaseC1Ev(char *self)
{
    *(void **)self = data_02099edc;
    _ZN9ActorBase9SceneNodeC1Ev(self + 0x14);
    *(void **)(self + 0x24) = self;             /* sceneNode.actor */
    for (int off = 0x28; off <= 0x38; off += 0x10) {
        *(void **)(self + off) = 0;
        *(void **)(self + off + 4) = 0;
        *(void **)(self + off + 8) = self;
        *(unsigned short *)(self + off + 0xc) = 0;
        *(unsigned short *)(self + off + 0xe) = 0;
    }
    int id = data_02099e70[0];
    *(int *)(self + 4) = id;
    data_02099e70[0] = id + 1;
    *(int *)(self + 8) = data_020a4b60[0];
    *(unsigned short *)(self + 0xc) = data_020a4b54;
    *(unsigned char *)(self + 0x12) = data_020a4b48;
    func_0203b438(data_020a4b6c, self + 0x14, (void *)(size_t)data_020a4b64[0]);
    {
        unsigned short *info = (unsigned short *)data_020a4bb8[
            *(unsigned short *)(self + 0xc)];
        *(unsigned short *)(self + 0x28 + 0xc) = info[2];   /* prio at +4 */
        *(unsigned short *)(self + 0x28 + 0xe) = info[2];
        *(unsigned short *)(self + 0x38 + 0xc) = info[3];   /* prio at +6 */
        *(unsigned short *)(self + 0x38 + 0xe) = info[3];
    }
    {
        char *parent = (char *)(size_t)func_02043810(self);
        if (parent) {
            unsigned char pf = *(unsigned char *)(parent + 0x13);
            if (pf & 3)
                *(unsigned char *)(self + 0x13) |= 2;
            if (pf & 0xC)
                *(unsigned char *)(self + 0x13) |= 8;
        }
    }
    return self;
}
} /* extern "C" */

// ---- gate-9 storage and bridges -------------------------------------------
extern "C" {
void *_ZTV11ShadowModel[8];

// ov098's SharedFilePtr entry table used to be harness storage here. It is
// the overlay's own data now (gate 19 mounts ov098 per symbol), and the
// gate-9 smoke keeps its private copy in port/tests/smoke_actor.cpp.
} /* extern "C" */

// The extern "C" side of the cxxname bridges (see hal/cxxname_bridge.cpp).
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *self);
void hal_fileptr_release(void *self) { _ZN13SharedFilePtr7ReleaseEv(self); }
}



extern "C" {
/* PORT_HOST_ABI: ARM asm primitive (48-byte block copy), MSVC cannot assemble.
   asm primitive: plain 48-byte block copy (with writeback, unlike the
   FIFO-fixed variant) */
void Copy48Bytes(int *src, int *dst) { for (int i = 0; i < 12; ++i) dst[i] = src[i]; }
int data_020a0e68[12];     /* Matrix4x3 scratch (0x30 on DS) -- was [4],
                              and FromTranslation's overrun stomped the
                              actor list head 0x20 bytes downstream */
short data_0208e378;
short *data_0209b45c;      /* spawn default rotation ptr (null = none) */
short *data_0209b460;      /* spawn default position ptr */
/* The current area id. func_02010e78 writes it, Actor::Actor copies it into
   mAreaId. Every declaration of it is byte-sized (decl_common.h has it as
   signed char, func_02010e78.c as unsigned char) and symbols.txt puts the
   next bss symbol at 0x0209b450, so one byte here is the whole object.
   It used to be hosted twice -- this `_c` byte for the C++-mangled spelling
   and a stray int[4] in hal/player_bridges.cpp for the plain one -- which
   put the writer and the reader on different storage as soon as main's
   mangled-declaration sweep moved Actor::Actor onto the plain name. One
   object now; the alias keeps the old mangling pointing at it. */
signed char data_0209b44c;
/* Actor::BeforeBehavior's behaviour mask: the actor's 0xb0 flags must
   intersect it for the tick to run, and 0 means "no mask, run everyone".
   Zero is the value a level with no cutscene director sets. */
int data_0209b464;
int data_0209b468[4];      /* actor list head the ctor links into */
}
#pragma comment(linker, "/alternatename:?data_0209b44c@@3CA=_data_0209b44c")

extern "C" {
unsigned char data_0209f2d8_c;   /* mega-char state byte: none */
int data_0209ceec[4];            /* shadow scale globals */
int data_0209cef4[4];
}
#pragma comment(linker, "/alternatename:_data_0209f2d8=_data_0209f2d8_c")
#pragma comment(linker, "/alternatename:?data_020a0e68@@3UMatrix4x3@@A=_data_020a0e68")

extern "C" {
void Matrix4x3_FromRotationY(void *m, int a);
void hal_m43_roty(void *m, int a) { Matrix4x3_FromRotationY(m, a); }
}

extern "C" {
/* PORT_HOST_ABI: ARM/Thumb asm primitive (Y-rotation matrix), MSVC cannot assemble.
   asm primitive: 4x3 fx32 Y-rotation from (sin, cos):
   rows {c,0,-s},{0,1,0},{s,0,c},{0,0,0} */
void func_02052820(int *m, int s, int c)
{
    m[0] = c;      m[1] = 0; m[2] = -s;
    m[3] = 0;      m[4] = 0x1000; m[5] = 0;
    m[6] = s;      m[7] = 0; m[8] = c;
    m[9] = 0;      m[10] = 0; m[11] = 0;
}
}

// Cleanup-path heap wrappers: unnamed on the DS side here; the semantics
// are the gate-3a Memory layer's.
extern "C" {
/* TWO arguments -- 0x0203c1e8, Memory::Deallocate(void*, Heap*). Nothing in
   this TU calls it, but the declaration has to agree with the definition in
   hal/cxxname_bridge.cpp: at C linkage a one-argument spelling compiles and
   links against a two-argument definition without a word, and that silence is
   exactly how the actor teardown lost its heap argument. */
void Memory_Deallocate(void *p, void *heap);

/* Heap_Destroy IS ARM9 0x0203c74c, Heap::_Destroy, AND IT IS THE THIRD
   SPELLING OF THAT ONE ADDRESS IN THIS TREE. The other two are the C symbol
   _ZN4Heap8_DestroyEv (src/_ZN4Heap8_DestroyEv.cpp, the ROM's own tail-call
   veneer) and the C++ method Heap::_Destroy (the receiver-bridging face at the
   bottom of hal/lk4_solidheap_seat.cpp). This one exists because the matched
   teardown TU spells the callee by role at C linkage rather than by mangled
   name: src/_ZN9ActorBase21AfterCleanupResourcesEj.c declares
   "void Heap_Destroy(void*);" with the comment "0x0203c74c = Heap::_Destroy"
   beside it, and calls it on line 60 as
   "if (this->unk4C) Heap_Destroy(this->unk4C);".

   IT USED TO ABORT, and the banner above used to say Destroy is never reached
   so it traps. The trap was correct as a statement about coverage and wrong as
   a permanent answer: an actor that owns a heap and reaches vfSuccess 2 kills
   the process instead of freeing the heap, and nothing about that is a port
   decision the ROM would recognise. It could not be wired before run link60
   lane LK5. Until that landed, _ZN4Heap8_DestroyEv was reached through an
   /alternatename off ?_Destroy@Heap@@QAEXXZ and its host body took NO
   parameter, so the receiver this face holds had nowhere to go. LK5 deleted
   that directive, named the parameter in the byte-matched veneer TU (zero ARM
   cost, r0 is already in place for the tail call) and put a Heap::_Destroy
   face the other way round beside it. The veneer now takes and delivers a
   receiver, so a pushed-argument face onto it is a straight forward.

   NO ABI SEAM HERE, which is the whole reason this is three lines. Both sides
   are C linkage, __cdecl, one pointer argument, so the heap rides the first
   stack slot in and the first stack slot out. This is the same direction as
   Memory_Deallocate above (a role name onto the real body), not the ECX-versus-
   stack shape the four sibling families needed. */
void _ZN4Heap8_DestroyEv(void *thiz);
void Heap_Destroy(void *h)
{
    _ZN4Heap8_DestroyEv(h);
}
}

/* THE OTHER END OF THAT CHAIN, AND IT MOVED HERE FROM LK5'S FILE FOR A LINK
   REASON THIS LANE MEASURED RATHER THAN GUESSED.

   src/_ZN4Heap8_DestroyEv.cpp, the ROM's byte-matched veneer, tail-calls the
   flat C name _ZN4Heap7DestroyEv, and the definition of that name is a face
   onto the __thiscall method Heap::Destroy. LK5 wrote the face at the bottom of
   hal/lk4_solidheap_seat.cpp, next to the probe evidence that earned it, and
   that evidence is still the reference for it.

   THE TARGET SETS DO NOT MATCH, which is what forced the move. Three sets are
   in play and only one of them is right:

     hal/lk4_solidheap_seat.cpp   3 targets (smoke_player, walk_window,
                                  walk_window_hires)
     hal/actor_vtables.cpp        6 targets, those three plus smoke_actor,
                                  smoke_savestate and smoke_persist
     hal/heap_vtable.cpp          far more than six, including the anim and
                                  soak smokes

   Heap_Destroy above is compiled wherever THIS file is, and GATE9_GEN puts its
   caller, the matched actor teardown, in all six. So wiring it while the face
   sat in the 3-target file gave three LNK2019s on __ZN4Heap8_DestroyEv, and
   parking the face in heap_vtable.cpp instead moved the failure rather than
   fixing it: every anim and soak smoke then demanded ?Destroy@Heap@@QAEXXZ,
   which none of them has any business linking. Both readings are from this
   lane's own build output, not from reasoning about the file lists.

   THIS FILE IS THE ONLY ONE WHOSE TARGET SET IS EXACTLY THE SET THAT NEEDS THE
   CHAIN, because it is the file that creates the demand. The two src TUs behind
   the face reach the three smokes through HEAP_DESTROY_CHAIN_SRC in
   port/CMakeLists.txt and the three big targets through slice_gate16.

   The face itself is unchanged from LK5's: flat C in, qualified __thiscall out,
   no receiver invented anywhere. The shadow has to be a CLASS and not a struct
   for the same reason hal/heap_vtable.cpp says beside its own pair, the src TUs
   declare `class Heap` and mangle PAV. */
class Heap {
public:
    void Destroy();
};
extern "C" void _ZN4Heap7DestroyEv(void *thiz)
{ ((Heap *)thiz)->Heap::Destroy(); }

extern "C" {
int func_0204424c(char *c);
int hal_f0204424c(char *c) { return func_0204424c(c); }
}

extern "C" {
/* PORT_HOST_ABI: MSL C++ array-construct asm with EH landing pad, MSVC cannot assemble.
   MSL runtime array construction (asm on the DS, with EH frames the host
   does not need): apply the ctor forward across n elements. */
void func_020733a8(void *base, int n, int stride,
                   void (*ctor)(void *), void (*dtor)(void *))
{
    (void)dtor;
    char *p = (char *)base;
    for (int i = 0; i < n; ++i, p += stride)
        ctor(p);
}

/* PORT_HOST_ABI: the matched TU is an mwccarm `asm` block (ARM hand-asm) with an EH landing pad; MSVC has no inline ARM assembler.
   ...and its sibling, the array DESTROY. src/__destroy_arr.c is hand-asm for
   the same reason as func_020733a8 above -- an exception frame no C under the
   ROM's flags emits -- so there is no source to compile, only a block to read.
   Re-derived from the ROM for this ruling: arm9 0x0207328c (size 0x5c) reads
   e92d48f0 / e24dd018 / e1a0b00d / e1b05003 -- stmdb sp!,{r4,r5,r6,r7,r11,lr};
   sub sp,sp,#0x18; mov r11,sp; movs r5,r3 -- which is the head of that asm
   block instruction for instruction, fp anchor and all. The block reads:
   r0 base, r1 count, r2 size, r3 dtor; `mla r4, r7, r6, r0` puts the cursor
   one past the end and the loop steps DOWN by size before each call, so the
   elements are destroyed back to front. Both zero-count guards fall out of
   the two early returns. */
void __destroy_arr(void *base, int n, int size, void (*dtor)(void *))
{
    char *p;
    if (dtor == 0 || n == 0)
        return;
    p = (char *)base + (size_t)n * size;
    while (n-- > 0) {
        p -= size;
        dtor(p);
    }
}
}

// ---- gate-10 storage and stubs --------------------------------------------
extern "C" {
/* SOUND: these three were stubbed silent because Sound::Play opens by
   walking the NNS sound player out of data_020a5bb8, and nothing seated that
   root, so the first Play in a level took the process with it. hal/sdat/ now
   seats a real parsed SDAT there and hosts the ARM7 behind the command
   queue, so all three are back on their matched src (added to
   slice_gate10.txt) and the stubs are gone. */

void *_ZTV15MaterialChanger[8];
void *_ZTV15TextureSequence[8];
void *_ZTV25MovingCylinderClsnWithPos[12];
int VT0[20];    /* an unresolved shared-header vtable alias in ov002 TUs */
int VT2[20];    /* ...and its sibling: SphereClsn's D1 spells its three
                   sub-object vtables VT0/VT1/VT2. All three are installed
                   transiently during teardown and never dispatched.
                   CAUTION: these names are per-TU placeholders, so two
                   classes can spell DIFFERENT vtables VT0 and both resolve
                   here. A slot that is actually dispatched must never come
                   through one of them. */
void *data_02099204[20];   /* WithMeshClsn's own vtable, same treatment */
/* the coin counter GiveCoins increments, PER PLAYER -- HUD::Behavior reads
   data_0209f358[data_0209f250]. run vs16: sixteen. */
short data_0209f358[kPortMaxPlayers];
unsigned char data_0209f208; /* the current-star index PowerStar reads */
short data_0209f308;         /* the star-marker timer FUN_0202a130 arms */
unsigned char data_0209f1f4; /* ...and the flag it clears with it */

// ---- gate 16: two named refusals ------------------------------------------
//
// Both stand in front of a subsystem the port does not host, both are the
// answer that subsystem gives when it is not there, and both are here rather
// than in a class's own file so that hosting the subsystem retires exactly one
// line.
//
// Enemy::UpdateYoshiEat (ov002 0x020ade78) NO LONGER LIVES HERE. It was a
// constant-0 stub -- "is this actor inside Yoshi's mouth", answered 0 -- and
// that constant 0 was the Yoshi-swallow-key bug: an eaten enemy never entered
// the swallow states, so the intro rabbit's eat branch (which writes the
// caught-by player at rabbit+0x45c and arms its key + dialogue) was dead code.
// The function is now hosted, as its full swallow-arc state machine, in
// port/unmatched/Enemy_UpdateYoshiEat.cpp (host-transcribed; it is a 6o
// ordering floor, not byte-matchable in this lane). That file rides the
// LEVELBOOT_HOST_SOURCES slice, next to the enemy Behaviors that call it; the
// small actor smokes that also link this HAL object never reference the symbol,
// so it is gone from here rather than duplicated.

// Player::StartTalk is LIVE (2026-08-08, the talk-entry session). The decline
// stub is GONE: the matched src is in slice_gate10 (right after ShowMessage2).
// Both preconditions the old note named are met -- Message is hosted (the
// dialogue pump/compositor ticks the box) and the sign's read-state Main
// (0x020bb614) is a logic-verified near-miss host copy in
// SignPost_StateDispatch.cpp. StartTalk's yes-branch now really enters ST_TALK
// and hands off to ShowMessage2; the sign, the Bob-omb Buddy and every other
// talker reach the box through it.

// Player::ShowMessage2 was declined here (2026-08-07) because its yes-branch
// enters the message state and nothing ticked the dialogue box. THE DECLINE IS
// GONE (2026-08-08, the dialogue-pipeline session): the matched src is back in
// slice_gate10 (_ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj.cpp) and the
// box now ticks -- hal/message_pump.cpp runs Message::UpdateWindow +
// Message::Update every frame (Stage::UpdateMessage's own dialogue arm) and
// hal/message_compositor path rasters engine A's 2D over the 3D frame. The one
// earlier fear -- "dispatches into machinery that is not there" -- was the box
// setup func_0201f32c, which is ordinary matched src writing the 2D engine
// registers; it does not wild-call. The stub definition is removed so the
// matched symbol from the slice is the one that links.


/* The global event bitfield. Event::GetBit reads it and BLACK_BRICK_BLOCK's
   Behavior asks whether its own event has fired; nothing on the castle grounds
   sets one, so zero is the level's own state. */
int data_0209f34c;

/* ov002's Enemy vtable, installed by func_ov002_020aed98 and overwritten by
   the derived class's own factory two lines later. The same treatment
   _ZTV5Actor and _ZTV12ActorDerived already have: real storage so the store
   lands somewhere, never dispatched through. */
/* 31 words, not 20: it is a full Actor table (ov002 0x021081e4) and the last
   slot is Actor::OnAimedAtWithEggReturnVec. Storage only, but a short array
   is a short array. */
void *data_ov002_021081e4[31];

unsigned char data_02092128[0x40];
/* 0x0209f318 is a Camera POINTER, not storage -- every declaration of it
   in src is `Camera *` / `char *` / `short *` / `void *`, never an array.
   It was 32 zero bytes here, which reads as a null camera, and the states
   that poke the camera through it were faulting on the write rather than
   skipping: St_DeadHit_Init calls func_0200d89c(data_0209f318), whose
   whole body is `*(short *)(p + 0x18c) = 384`, so a null landed on
   address 0x18c. Point it at a zeroed block instead. Generous size, same
   convention as auto_bss: Camera.h evidences fields to 0x1a6 and the
   class is an Actor subclass, so 0x400 covers it with room.
   Verified regression-free: the eight walk probes come back
   byte-identical with this non-null. */
__declspec(align(8)) static unsigned char HAL_CAMERA[0x400];
void *data_0209f318 = HAL_CAMERA;
/* run mg16 lane MP3, field failure 2: FULL-EXTENT BACKING, for exactly the
   reason the paragraph below already gives for data_0209f4ac/data_0209f4ae.
   These three are per-player Ctrl fields on the DS -- held buttons at
   Ctrl+0x04, pressed-this-frame at +0x06, stick x at +0x08, all striding
   0x18 -- and hosting them as BARE SHORTS gave player 0 two bytes and every
   other player somebody else's memory.

   THE SYMPTOM WAS A BUTTON BLEEDING BETWEEN WORLDS. Crouch pressed in the
   child's window made MARIO crouch in the child's world: the crouch reader
   takes its buttons from these, the port fanned only the STICK fields per
   player, and with no storage past offset 0 there was nowhere for the
   child's own buttons to go. The stick fanned correctly and the buttons did
   not, which is why movement looked right and one button family did not.

   It was also an OUT-OF-BOUNDS WRITE the moment the port started fanning
   data_0209f4a0 at stride 0x18: a two-byte write 24 bytes past a two-byte
   object, into whatever the linker put next -- the same stray this file's
   next paragraph documents catching once already. */
unsigned char data_0209f49c[0x18 * kPortMaxPlayers];   /* 0.3.2: sixteen, see the hold loop in star_flow.cpp and the fan-out in walk_window.cpp */
unsigned char data_0209f49e[0x18 * kPortMaxPlayers];   /* 0.3.2: sixteen, see the hold loop in star_flow.cpp and the fan-out in walk_window.cpp */
unsigned char data_0209f4a0[0x18 * kPortMaxPlayers];   /* 0.3.2: sixteen, see the hold loop in star_flow.cpp and the fan-out in walk_window.cpp */
unsigned char data_0209f4ab;
/* Per-player controller records, stride 0x18, four players. The matched
   writers index BOTH bases by player: InitControllerMode stores the mode
   at data_0209f4ae + idx * 0x18, and the ov002 pause readers plus
   Stage::PS_Update read data_0209f4ac + idx * 0x18. Hosted as bare shorts
   these were two bytes each, so SetPlayerGlobals' four-player boot loop
   strayed: player 1's byte landed in data_020a0e40[5], and player 2's
   zeroed the low byte of _ZTV18MovingCylinderClsn[3] two symbols later,
   which happened to turn mcc_ownerid (low byte 0x80) into lb_d0 (low byte
   0x00) and sent the frame-0 cylinder pass into LakituBro's deleting
   destructor. The two bases are distinct fields of the same DS record and
   are only ever indexed apart, so separate full-extent backing is safe. */
unsigned char data_0209f4ac[0x18 * kPortMaxPlayers];   /* 0.3.2: sixteen, see the hold loop in star_flow.cpp and the fan-out in walk_window.cpp */
unsigned char data_0209f4ae[0x18 * kPortMaxPlayers];   /* 0.3.2: sixteen, see the hold loop in star_flow.cpp and the fan-out in walk_window.cpp */
int data_020a0e40[8];
}

extern "C" {
/* PORT_HOST_ABI: MSL C++ array-new-with-cleanup asm + EH landing pad, MSVC cannot assemble.
   MSL array new-with-ctor (asm on the DS, EH machinery the host skips):
   allocate count*size + an 8-byte cookie, record the count, construct
   forward. Layout mirrors the DS cookie so array-delete agrees. */
void *func_0203cc0c(unsigned size);
void *func_02073470(int count, int size, int cookie,
                    void (*ctor)(void *), void (*dtor)(void *))
{
    (void)dtor;
    char *raw = (char *)func_0203cc0c(count * size + cookie);
    if (!raw) return 0;
    ((int *)raw)[0] = size;
    ((int *)raw)[1] = count;
    char *base = raw + cookie;
    for (int i = 0; i < count; ++i)
        if (ctor) ctor(base + i * size);
    return base;
}
}

extern "C" {
/* __aeabi_idiv: the EABI signed-divide helper; host has native idiv.
   PORT_HOST_ABI: ARM asm primitive (ITCM 0x01ffabe4, quotient in r0 and
   remainder in r1), MSVC cannot assemble. main files the matched TU as
   src/_s32_div_f.c, a bannered HAND-ASM PRIMITIVE -- run linkw wave 9
   (lane w9-harvest) checked: it is an ARM asm block, so it cannot replace
   this host body no matter how the port catches up to main. */
int __aeabi_idiv(int n, int d) { return d ? n / d : 0; }
void *_ZTV18MovingCylinderClsn[12];

/* gate-10 BSS ring (spawn/camera/collision-config globals; zeros are the
   pre-scene defaults) */
/* SUBLEVEL_LEVEL_TABLE moved to romdata (gate 35): it is file-backed arm9
   data at 0x02075298, not BSS, and every star and coin record is filed under
   what it returns. The `short[64]` here was also half the right stride --
   decl_common.h declares it `signed char[]` and SublevelToLevel indexes it
   that way. */
int data_020991d8[8], data_02099264[8], data_02099274[8];
int data_02099338[8], data_02099348[8], data_02099358[8], data_02099368[8];
int data_020994cc[8];
/* data_02099fa4/fa8/fac moved to romdata (gate 35): they are file-backed arm9
   data, not BSS, and data_02099fac is the 3D sound distance limit -- zeroed,
   it culls every positional sound effect in the game and Sound::Play's
   response to the cull is a silent return. */
/* data_0209b008 moved to hal/camera_states.cpp: it is the first of the 19
   camera State objects, and the whole run has to carry the DS fn addresses */
int data_0209b478[8], data_0209b484[4], data_0209b488[4];
int data_0209b498[8], data_0209d574[8];
/* Sound::PlayLong's handle table: a header plus 0x40 slots of 0x14, which
   func_02011a28 initialises from +8 and func_02011934 hashes into. The ROM
   runs 0x0209b53c..0x0209baa0 = 0x564. It was int[8] while sound was
   stubbed, so the init would have written 1.2KB past it. */
int data_0209b53c[0x564 / 4];
/* data_0209f310 (the VS-star obtained array base) moved to
   hal/actor_classes_star.cpp: it and data_0209f311 are the ROM's own first two
   bytes of one contiguous run NumVsStarsObtained walks, so they are hosted as
   one grouped-section block there instead of two disjoint host objects. */
int data_020a0f10[8], data_020a4bec[8];
/* data_0209f340 is the CURRENT LVL_Overlay (Stage::GetSkyboxID,
   StartWithFarCamera and the render chain read its flag and skybox bits).
   With no level booted it parks on a zeroed block so every flag reads 0;
   the gate-14 boot repoints it at ov009's real record. */
static unsigned char hal_campref_blk[0x40];
unsigned char *data_0209f340 = hal_campref_blk;
}

extern "C" {
/* ModelAnim2's two tables: the ctor/dtors only STORE these pointers; any
   virtual dispatch through them lands on a null slot and the fault probe
   names it. Filled at runtime if a gate ever dispatches ModelAnim2. */
void *_ZTV10ModelAnim2[12];
void *VTable_Animation_ModelAnim2Thunk[12];
void *data_020a5bb8;            /* table root pointer (func_02050xxx family) */
int data_0209f5c0[8];
/* data_020ad560 USED TO BE A ZEROED int[0x3c/4] HERE (run linkw wave 4, lane
   w4-a), the exact counterpart of the cuboid's host array that wave 3 removed
   from hal/cxxname_bridge.cpp. It is the cylinder shadow's template BMD and it
   is not bss: 0x3c bytes of ov001 .data, mounted by name in
   port/ov001_syms.txt since wave 3 with its bone (0x020ad5dc), its material
   (0x020ad4c4) and the four words they reach. The host array is gone so the
   matched src/_ZN11ShadowModel12InitCylinderEv.cpp, which spells the arm9 name,
   resolves onto the ROM bytes instead of reading zeros over them. */
}
DSSTATE_END
/* The mount emits ov001's own spelling and the matched TU spells the arm9 name;
   both are data at the same address, so the alias is exact -- the same pattern
   hal/cxxname_bridge.cpp uses for the cuboid's data_020ad524 one line over.
   /alternatename only fires for an UNDEFINED symbol, which is why the host
   array above had to go rather than merely be renamed. Harmless in the three
   narrow harnesses that compile this file without the ov001 mount
   (smoke_actor, smoke_savestate, smoke_persist): nothing in them references
   either spelling, and the directive is inert for a symbol nobody asks for. */
#pragma comment(linker, "/alternatename:_data_020ad560=_data_ov001_020ad560")

