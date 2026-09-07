/* HOST COPY of src/_ZN10KingBobOmb8BehaviorEv.cpp -- the mwcc
 * pointer-to-member dispatch for the eleventh time in this port, and the
 * widest one yet: EIGHTEEN states of two halves each.
 *
 * The OTHER half of this pair, src/KingBobOmb_SetState.cpp, is no longer host
 * copied: run link100 lane PMF3 put it back on the slice once /vmg /vmm made
 * MSVC's pointer-to-member the ROM's own 8-byte record. Behavior stays because
 * it does more than dispatch -- it COMPARES the state pointer against four
 * records by address, and that comparison is what decides whether the king is
 * carried, thrown or walking. See the note where the setter used to be.
 *
 * KING_BOB_OMB'S STATE IS A POINTER, not an index. Every other class in this
 * gate keeps a number at a fixed offset and indexes a table; the king keeps
 * the ADDRESS of a two-PMF record at +0x420 and the matched pair reads it back
 * with pointer arithmetic:
 *
 *     struct C; typedef int (C::*PMF)();
 *     struct C { char pad[0x420]; PMF *pp; };
 *     KingBobOmb_SetState:  c->pp = p; q = c->pp;
 *                           if (*q == 0) return 1; return (c->**q)();
 *     Behavior:             if (*(void **)((char *)pp + 8) != 0)
 *                               { PMF *p = pp + 1; (this->**p)(); }
 *
 * Both lines are the same problem. `C` is INCOMPLETE where the typedef is
 * formed, so MSVC widens PMF to its four-word general representation: `pp + 1`
 * steps sixteen bytes where the ROM steps eight, and `*(char *)pp + 8` -- the
 * null test the ROM writes by hand -- reads the second half of a record that
 * is now twice as wide. Behavior also COMPARES pp against four of the state
 * records by address, which is what decides whether the king is being carried,
 * thrown, or walking, so getting the stride wrong changes behaviour rather
 * than just crashing.
 *
 * The records themselves are {init PMF, main PMF}, sixteen bytes, and
 * __sinit_ov078_02126660 builds all eighteen out of thirty-six 8-byte statics
 * at ov078 0x02126ce0..0x02126df8. Those statics are seated in
 * hal/actor_overlays.cpp before that sinit runs, the rabbit's treatment, so
 * the copy propagates host addresses wherever the ROM propagates DS ones.
 * Every one of the thirty-six halves is matched src, so this file carries no
 * trap.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortKingPmf { unsigned fn; int delta; };
/* the ROM's record: two PMFs, sixteen bytes */
struct PortKingState { PortKingPmf init, main; };

/* the four records Behavior compares its own state pointer against */
extern PortKingState data_ov078_0212703c, data_ov078_0212707c,
    data_ov078_021270bc, data_ov078_021270fc;
extern int data_ov078_02126e00[];      /* the collider's offset Vector3 */

int _ZN5Actor13DistToCPlayerEv(void *self);
void _ZN14BlendModelAnim7AdvanceEv(void *self);
unsigned short DecIfAbove0_Short(unsigned short *p);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *clsn);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wmc,
                                              unsigned flags);
int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
    void *self, const void *v);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void func_ov078_02125de0(void *self);
void func_ov078_02125c98(void *self);
extern void *data_0209f318;            /* the camera the king registers with */

int KingBobOmb_SetState(void *self, void *state);
int _ZN10KingBobOmb8BehaviorEv(void *self);

}  /* extern "C" */

/* Slot 3 of the king's own BlendModelAnim, which Behavior dispatches through a
   four-virtual shadow. The port fills _ZTV14BlendModelAnim in MSVC order, so
   the shadow's index and the array's disagree; the qualified call is what the
   ROM means. */
#include "BlendModelAnim.h"

typedef void (*PortKingFn)(void *);

static void port_king_call(const PortKingPmf *m, void *self, const char *half)
{
    if (m->fn & 1) {
        std::fprintf(stderr, "FATAL: KingBobOmb %s half is a VIRTUAL member "
                     "pointer (%08x/%d); the ROM stores none there\n", half,
                     m->fn, m->delta);
        std::abort();
    }
    ((PortKingFn)(size_t)m->fn)((char *)self + m->delta);
}

/* +0x420 is the state pointer both the setter and Behavior read. */
#define PORT_KING_STATE(s) (*(PortKingState **)((char *)(s) + 0x420))

/* KingBobOmb_SetState IS NO LONGER HOST-COPIED. src/KingBobOmb_SetState.cpp is
   on port/slice_pmf3.txt (run link100 lane PMF3). With /vmg /vmm target-wide
   MSVC's pointer-to-member IS the ROM's 8-byte {function, delta} record, so
   `pp + 1` steps eight and the null test reads the record's own function word,
   and the emitted body is a TAIL JUMP: the caller's cdecl frame survives, the
   seated body reads its receiver from [esp+4], and the state pointer the
   caller pushed second sits unread at [esp+8]. The adjustment is proven twice.
   port_king_bob_omb_states_seat (hal/actor_overlays.cpp) aborts the binary on
   any nonzero delta in any of the thirty-six halves before the king can
   dispatch, and those same thirty-six source pairs at ov078
   0x02126ce0..0x02126df8 were re-read out of overlay_0078.bin with their
   relocations: every adjustment word is ROM zero.

   The declaration at the top of this file now names the matched TU, and
   Behavior below still calls it. One difference worth naming: the host body
   returned 1 unconditionally, the matched TU returns whatever the dispatched
   init half returns, which is what the ROM does. Behavior ignores the value.
   port_king_call and PORT_KING_STATE stay because Behavior uses both. */

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch; MSVC's PMF over an
 * incomplete class is the wider general representation. See the header. */
extern "C" int _ZN10KingBobOmb8BehaviorEv(void *self)
{
    char *c = (char *)self;
    PortKingState *st;

    if (_ZN5Actor13DistToCPlayerEv(c) < 0x1770000)
        *(void **)((char *)data_0209f318 + 0x114) = c;

    st = PORT_KING_STATE(c);
    if (st->main.fn != 0)
        port_king_call(&st->main, c, "main");

    /* ((Base *)&mBlendModelAnim)->v3() -- the shadow declares four virtuals
       and takes the fourth, so ROM slot 3, which on a BlendModelAnim is
       UpdateVerts. hal/blend_vtable.cpp fills that array in MSVC order, where
       slot 3 is Virtual10 and takes an argument, so the dispatch is spelled
       qualified rather than through the array. */
    *(int *)(c + 0x328) = *(int *)(c + 0x4fc) << 0xc;
    ((BlendModelAnim *)(c + 0x2cc))->BlendModelAnim::UpdateVerts();
    _ZN14BlendModelAnim7AdvanceEv(c + 0x2cc);

    if (st == &data_ov078_0212707c) {
        void *r1 = *(void **)(c + 0x494);
        int skip = 0;
        if (r1 != 0 && (*(int *)(c + 0xb0) & 0x4000) != 0 &&
            *(int *)((char *)r1 + 0xc8) != 0)
            skip = 1;
        if (!skip)
            func_ov078_02125de0(c);
        func_ov078_02125c98(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    DecIfAbove0_Byte((unsigned char *)(c + 0x505));
    DecIfAbove0_Byte((unsigned char *)(c + 0x504));

    if (st != &data_ov078_021270bc)
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x33c);
    else
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0x33c);

    if (st != &data_ov078_021270bc || *(unsigned char *)(c + 0x499) == 1)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x110, 0);

    if (st == &data_ov078_0212703c || st == &data_ov078_021270fc) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x110) != 0 ||
            _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) == 0 ||
            (*(int *)(c + 0x4d8) - 0x28000) > *(int *)(c + 0x60))
            KingBobOmb_SetState(c, &data_ov078_021270bc);
    }

    {
        int v[3];
        v[0] = data_ov078_02126e00[0];
        v[1] = data_ov078_02126e00[1];
        v[2] = data_ov078_02126e00[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
            c + 0x33c, v);
    }
    {
        int v[3];
        v[0] = data_ov078_02126e00[0];
        v[1] = data_ov078_02126e00[1];
        v[2] = data_ov078_02126e00[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
            c + 0x37c, v);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x33c);
    _ZN12CylinderClsn6UpdateEv(c + 0x33c);
    _ZN12CylinderClsn5ClearEv(c + 0x37c);
    _ZN12CylinderClsn6UpdateEv(c + 0x37c);

    func_ov078_02125de0(c);
    func_ov078_02125c98(c);
    return 1;
}
