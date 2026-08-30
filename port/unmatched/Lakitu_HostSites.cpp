/* HOST COPIES of three ov077 LAKITU (actor 265, daJgm_c) sites MSVC cannot
 * deliver from the matched source, run rel0215 lane cast-ov077. Each matched
 * TU stays byte-locked in src/ as the proof and is simply not referenced from
 * port/slice_ov077.txt.
 *
 * (1) src/_ZN6LakituD1Ev.cpp -- THE C++-MANGLING WALL, not an ABI subtlety.
 *     That TU is a REAL C++ class definition ending `virtual ~Lakitu();` with
 *     the body `Lakitu::~Lakitu() {}`. mwccarm mangles that Itanium and emits
 *     _ZN6LakituD1Ev; MSVC mangles it ??1Lakitu@@UAE@XZ and emits NOTHING by
 *     the name slot 16 of the vtable needs, so the symbol the seat wants is
 *     simply absent. It would also emit its own COMDAT vtable ??_7Lakitu@@6B@
 *     and store THAT at this[0] instead of the ROM-shaped _ZTV6Lakitu the
 *     registry fills -- a silently wrong table on every destroy -- and it would
 *     call each member destructor under MSVC's own mangling (??1Model@@QAE@XZ),
 *     none of which this port defines. Three separate failures from one TU.
 *
 *     THE BODY BELOW IS THE ROM'S, NOT AN INVENTION. Lakitu's D1 is
 *     0x02123740..0x02123798 and its D0 is 0x02123798..0x02123804. Read out of
 *     config/arm9/overlays/ov077/relocs.txt the two make the SAME seven calls
 *     in the SAME order and load the SAME vtable, and D0 adds exactly one more
 *     call plus one more load:
 *       D1 0x02123754 0x020373f8 _ZN12WithMeshClsnD1Ev
 *          0x0212375c 0x02014a60 _ZN25MovingCylinderClsnWithPosD1Ev
 *          0x02123764 0x02015a2c _ZN15TextureSequenceD1Ev
 *          0x0212376c 0x02015ff8 _ZN11ShadowModelD1Ev
 *          0x02123774 0x02016d20 _ZN5ModelD1Ev
 *          0x0212377c 0x0201691c _ZN9ModelAnimD1Ev
 *          0x02123784 0x020112c8 _ZN5ActorD2Ev
 *          0x02123794 load       0x0212786c  (_ZTV6Lakitu)
 *       D0 the identical seven at 0x021237ac..0x021237dc, then
 *          0x021237ec 0x0203c1e8 _ZN6Memory10DeallocateEPvP4Heap
 *          0x021237fc load 0x0212786c   0x02123800 load 0x020a0eac (the heap)
 *     So D1 IS D0 minus the deallocate, and the member offsets come from
 *     src/_ZN6LakituD0Ev.c, which is a plain C body the ROM matches and which
 *     this lane leaves in the slice untouched. Nothing here is guessed from a
 *     vtable slot (T5): every call and both offsets are relocation-confirmed.
 *
 *     D1 stores _ZTV6Lakitu and D0 stores _ZTV7daJgm_c. Those are ONE table
 *     under two spellings (the ROM's own typeinfo record at 0x0212783c reads
 *     "7daJgm_c"); hal/actor_classes_ov077.cpp defines _ZTV6Lakitu and bridges
 *     the RTTI spelling onto it, the SkiLift/MotherPenguin/HootTheOwl shape.
 *
 * (2) src/_ZN6Lakitu6RenderEv.cpp -- THE ModelAnim SLOT-5 COLLISION, the
 *     Whomp/Fish/Butterfly case that gates 17/18/21/23 and lane gate-194 all
 *     walked into. It dispatches `((VObj *)&mModelAnim)->m(0)` through a LOCAL
 *     six-virtual shadow, so its "slot 5" is the ROM's ModelAnim::Render --
 *     but hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC order, where
 *     slot 5 is Virtual18 (two arguments where the shadow passes one), and it
 *     cannot be dual-filled because something real occupies the slot. The
 *     second dispatch in the same body is on &mModel, whose host table IS
 *     dual-filled and would have been fine; the function still has to come out
 *     of the slice as a whole, so both are spelled qualified below.
 *     Offsets are Lakitu.h's own: unk_0b0 0xb0, mModelAnim 0xd4, unk_0dc 0xdc,
 *     unk_12c 0x12c, mModel 0x138, mTextureSequence 0x1b0, unk_3f4 0x3f4.
 *
 * (3) src/func_ov077_02124718.cpp and src/func_ov077_02124754.cpp -- THE PMF
 *     DISEASE, the SoundObject/Cap/MrBlizzard/BabyPenguin/Unagi/HootTheOwl
 *     shape. Both form
 *         struct C; typedef void (C::*PMF)();
 *         struct C { char pad[0x3f0]; PMF *pp; };
 *         PMF *p = c->pp (+ 1); (c->**p)();
 *     -- a real C++ pointer-to-member on a DELIBERATELY INCOMPLETE class.
 *     MSVC's representation for that is the "unknown inheritance" general
 *     form, not a one-word function pointer, so it does not reproduce the
 *     ROM's own 8-byte {function, delta} record. Read from the recovered
 *     source's own struct declaration, not inferred from a crash.
 *     02124754 dispatches cell[0] (enter) and 02124718 cell[1] (tick), off the
 *     state pointer at self+0x3f0. Every delta word in the ROM's own records
 *     is 0 (verified against overlay_0077.bin, see port/ov077_syms.txt), so a
 *     plain call through the fn word is the same call.
 *
 * THE SEAT that makes (3) legal is port_ov077_states_seat() in
 * hal/actor_classes_ov077.cpp: the mounted PMF SOURCE records hold DS CODE
 * addresses (ovdata.py's pointer pass only rebases pointers into other mounted
 * DATA), so each record's fn word is rewritten with its HOST body's address --
 * validated against the ROM's own address first -- BEFORE __sinit_ov077_
 * 02127240 copies the ten records into data_ov077_02127bc4.
 */
#include "Model.h"
#include "ModelAnim.h"

extern "C" {

/* ---- (1) the destructor ------------------------------------------------ */
extern int _ZTV6Lakitu[];
void _ZN12WithMeshClsnD1Ev(void *);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
void _ZN15TextureSequenceD1Ev(void *);
void _ZN11ShadowModelD1Ev(void *);
void _ZN5ModelD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void _ZN5ActorD2Ev(void *);
int _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *comp);

/* PORT_HOST_ABI: mwcc-Itanium destructor name MSVC cannot emit; ROM call
 * order and offsets from relocs.txt + src/_ZN6LakituD0Ev.c. */
int *_ZN6LakituD1Ev(int *t)
{
    t[0] = (int)(size_t)_ZTV6Lakitu;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x204);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1c4);
    _ZN15TextureSequenceD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN5ModelD1Ev((char *)t + 0x138);
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}

/* ---- (2) Render -------------------------------------------------------- */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN6Lakitu6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)
        return 1;
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x1b0, c + 0xdc);
    /* ((VObj *)&mModelAnim)->m(0) -- the ROM slot-5 Render, spelled qualified */
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    if (*(int *)(c + 0x3f4) == 1) {
        unsigned int v = ((unsigned int)(*(int *)(c + 0x12c) << 4)) >> 0x10;
        if (v >= 0x19 && v <= 0x3a)
            ((Model *)(c + 0x138))->Model::Render(0);
    }
    return 1;
}

/* ---- (3) the two state dispatch sites ---------------------------------- */
struct PortOv077Pmf { unsigned int fn; int delta; };
typedef void (*PortOv077StateFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
void func_ov077_02124754(void *cv)   /* cell[0], the ENTER half */
{
    PortOv077Pmf *p = *(PortOv077Pmf **)((char *)cv + 0x3f0);
    ((PortOv077StateFn)(size_t)p[0].fn)(cv);
}

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
void func_ov077_02124718(void *cv)   /* cell[1], the TICK half */
{
    PortOv077Pmf *p = *(PortOv077Pmf **)((char *)cv + 0x3f0);
    ((PortOv077StateFn)(size_t)p[1].fn)(cv);
}

}  /* extern "C" */
