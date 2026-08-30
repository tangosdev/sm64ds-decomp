/* HOST COPY of src/_ZN13PrincessPeach8BehaviorEv.cpp -- PRINCESS_PEACH's
 * (actor 186, ov085) slot 6, with ONE line changed and the rest transcribed.
 *
 * ---- WHY IT CANNOT LINK, and it is not a missing body ----------------------
 *
 * The matched TU is real decomp and it is correct against the ROM. What it
 * cannot survive is the port's ModelAnim vtable numbering. The ROM at
 * 0x0212a554 does
 *
 *     add   r0, r4, #0xd4        the object's ModelAnim member
 *     ldr   r1, [r0]             its vptr
 *     ldr   r1, [r1, #0xc]       SLOT 3
 *     blx   r1
 *
 * and slot 3 of _ZTV9ModelAnim (arm9 0x0208e980, port/tools/vtspan.py) is
 * _ZN9ModelAnim11UpdateVertsEv. The matched TU spells that dispatch as
 *
 *     struct Sub { virtual int g0(); virtual int g1(); virtual int g2();
 *                  virtual int g3(); };
 *     ((Sub*)((char *)&mModelAnim))->g3();
 *
 * -- a ROM-NUMBERED shadow, index 3. hal/cxxname_bridge.cpp fills the host
 * _ZTV9ModelAnim in MSVC numbering, where the Itanium D1/D0 pair folds into
 * one slot and every later ROM slot N becomes MSVC N-1:
 *
 *     MSVC 0 = dtor (ROM 0+1)      MSVC 3 = Virtual10  (ROM 4)
 *     MSVC 1 = DoSetFile (ROM 2)   MSVC 4 = Render     (ROM 5)
 *     MSVC 2 = UpdateVerts (ROM 3) MSVC 5 = Virtual18  (ROM 6)
 *
 * so index 3 is ma2_virtual10, which is ModelAnim::Virtual10(Matrix4x3 &) --
 * a method taking a matrix reference this call site does not pass. Not a
 * link error, not a byte-gate signal: a wrong slot with a wrong arity, every
 * frame, on a class that appears in the game's opening.
 *
 * This is the wave-19 collision and the ruling is the settled one. _ZTV5Model
 * is DUAL-FILLED at [4] and [5] with mv_render precisely so a ROM-numbered
 * shadow over a plain Model still lands on Render -- which is why WALL_SIGN's
 * Render and this class's own Render link. _ZTV9ModelAnim cannot be
 * dual-filled, because ROM slot 5 IS Virtual18's MSVC slot, and that is why
 * LAKITU_BRO's and the RABBIT's renders are host copies. The same array, one
 * slot lower.
 *
 * THE ONE CHANGED LINE dispatches the same object through the same host table
 * at the index that stands for ROM slot 3. Kept virtual rather than folded
 * into a direct call to ModelAnim::UpdateVerts: the ROM's instruction is a
 * dispatch, the member's dynamic type is whatever _ZN9ModelAnimC1Ev installed,
 * and a class that ever seats a different table there should still reach its
 * own override.
 *
 * ---- THE REST IS THE ROM, LINE FOR LINE -----------------------------------
 *
 * extracted/overlays/overlay_0085.bin, base 0x02129020, 0x0212a52c, 0x5c bytes:
 *
 *     bl 0x0212a430                    the MAIN state dispatch
 *     bl 0x02129dbc                    aim the head at the closest player
 *     ldr r0,[r4,#0x354]; cmp #1       the state index
 *     bl 0x02015c3c  (+0x124)          Animation::Advance, skipped in state 1
 *     <the slot-3 dispatch above>      (+0xd4)
 *     bl 0x02015024  (+0x160)          CylinderClsn::Clear
 *     bl 0x02014ff0  (+0x160)          CylinderClsn::Update
 *     bl 0x02129fdc                    the per-frame matrix and drop shadow
 *     mov r0,#1
 *
 * State 1 is the TALK state (func_ov085_0212a328 enters it and writes 1 to
 * +0x354), which is why the animation is held still while a message is up.
 */

// PORT_HOST_ABI: ModelAnim vtable numbering (ROM slot 3 is MSVC index 2; the
// array cannot be dual-filled because ROM slot 5 is Virtual18's MSVC slot).
extern "C" {

void func_ov085_0212a430(void *c);
void func_ov085_02129dbc(void *c);
void func_ov085_02129fdc(void *c);
void _ZN9Animation7AdvanceEv(void *a);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);

}  /* extern "C" */

/* Three virtuals, so ->m2() is MSVC index 2 -- ma2_updateverts, the host's
   stand-in for ROM slot 3. The ROM's own shadow declares four and calls the
   fourth; that is the whole of the difference. */
struct PeachModelAnimDispatch {
    virtual void m0();
    virtual void m1();
    virtual void m2();
};

extern "C" int _ZN13PrincessPeach8BehaviorEv(void *self)
{
    char *c = (char *)self;

    func_ov085_0212a430(c);
    func_ov085_02129dbc(c);
    if (*(int *)(c + 0x354) != 1)
        _ZN9Animation7AdvanceEv(c + 0x124);
    ((PeachModelAnimDispatch *)(c + 0xd4))->m2();
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    _ZN12CylinderClsn6UpdateEv(c + 0x160);
    func_ov085_02129fdc(c);
    return 1;
}
