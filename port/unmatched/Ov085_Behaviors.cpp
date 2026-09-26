/* HOST REIMPLEMENTATIONS of ov085 slot 6 for WALL_SIGN (183) and TOAD (185):
 * BOTH RETIRED. Nothing in this file is compiled any more; it stays, as a
 * note, because three target source lists in port/CMakeLists.txt still name
 * it (the shape lane SMALLS2 left its own emptied host files in).
 *
 * Neither body was a host COPY of a matched source when it was written: both
 * were transcribed from the ROM (extracted/overlays/overlay_0085.bin at load
 * base 0x02129020, every call target out of
 * config/arm9/overlays/ov085/relocs.txt, never out of the stale dsd export)
 * because src/ had neither function. main has matched both since.
 *
 * WALL_SIGN (ov085 0x0212eea4, 0x30c bytes), retired by run linkfull lane
 * SMALLS2: src/_ZN8WallSign8BehaviorEv.cpp (MATCHING 2004/b56) runs in its
 * place, reached through the generated reverse face for its flat name
 * (port/faces_sync.txt, port/slice_w31_smalls2.txt).
 *
 * TOAD (ov085 0x02129878, 0x204 bytes), retired by run linkfull lane SEATS3:
 * src/_ZN4Toad8BehaviorEv.cpp (MATCHING 2004/b56) is on port/slice_gate205.txt
 * and hal/actor_classes_ov085.cpp's slot 6 calls the member. Its faces_sync
 * row flipped from FORWARD to REVERSE, since the forward face was the thing
 * that defined ?Behavior@Toad@@UAEHXZ over the body that stood here. The
 * matched body makes the calls this one made, in the same order: RunState,
 * the ModelAnim's UpdateVerts, ClosestPlayer, the three Vec3 reads, AngleDiff,
 * the rec-room body turn, FindWithID on the held cap, the three approaches,
 * Animation::Advance, UpdateModelPose, ApplyOpacity, dCc_c Clear and Update.
 *
 * THE TWO CONVENTION HAZARDS this header used to carry, and where each went:
 *
 * (1) Toad::Behavior opens with a VIRTUAL call of ROM slot 3 on the ModelAnim
 *     at +0x108 (ModelAnim::UpdateVerts, arm9 0x0201686c). The old note said
 *     the host table numbers that slot Virtual10, so this body spelled the
 *     call qualified. That stopped being true when include/ModelBase.h spelled
 *     the destructor pair as two plain virtuals on the host: MSVC's own
 *     ModelAnim vftable is ROM-numbered now, and it is the one Toad's
 *     ModelAnim carries (d_a_kinopio.c constructs it through
 *     ??0ModelAnim@@QAE@XZ, whose table's word 3 is
 *     ?UpdateVerts@ModelAnim@@UAEXXZ; hal/cxxname_bridge.cpp's host-filled
 *     _ZTV9ModelAnim has UpdateVerts at 3 as well). So the matched TU's
 *     `mModelAnim.UpdateVerts()` reaches the ROM's target as it stands, and
 *     it needs no hostgen row.
 *
 * (2) WallSign::Behavior turns the PLAYER, not itself: three of its four
 *     approach calls take the Player at this+0x360 as their receiver. Its
 *     matched TU carries that shape.
 */
