/* THE ONE ov092 TOX_BOX (actor 309, daOnms_c) HOST SITE IS RETIRED --
 * run link100, lane PMFB6, gate 2.
 *
 * This file host-copied ToxBox::Behavior because
 * src/_ZN6ToxBox8BehaviorEv.cpp forms its pointer-to-member over a
 * deliberately INCOMPLETE `struct ToxBox;` and MSVC picked the sixteen-byte
 * unknown-inheritance form, so `data_ov092_02132568[idx]` strode by 16 where
 * the ROM strides by 8 and index 1 already read the middle of record 0.
 *
 * THAT STOPPED BEING TRUE when port/CMakeLists.txt block R8 turned /vmg /vmm
 * on for every C++ source in the target. Re-measured for this gate off the
 * matched TU's own /FAsc listing under the port's own flags
 * (runs/link100/out/PMFB6/emit_gate2_out.txt):
 *
 *     mov ecx, ?data_ov092_02132568@@3PAUTableEnt@@A[esi*8+4]
 *     mov eax, ?data_ov092_02132568@@3PAUTableEnt@@A[esi*8]
 *     add ecx, edi
 *     call eax
 *
 * [esi*8] and [esi*8+4] -- the two words of ONE ROM record, ARITY ZERO, and
 * /Zp4 changes 0 listing lines outside the TITLE. The matched TU defines the
 * FLAT extern "C" name __ZN6ToxBox8BehaviorEv, which is exactly the symbol the
 * ov092 vtable fill calls, so this row needs no forwarder -- only the
 * /alternatename for the decorated table spelling, which is in
 * port/hal/fwd_forwarders.cpp with the gate's other table aliases.
 *
 * The nine cells and the two tail cells the .bss extent allows are seated in
 * hal/actor_classes_ov092.cpp's port_ov092_states_seat, one __fastcall face
 * per cell. Nothing is host-copied from this class any more.
 *
 * THE FILE IS KEPT rather than deleted because port/CMakeLists.txt names it
 * directly in three target_sources lines and port/slice_ov092.txt's refusal
 * text points at it; the refusal it records is now history and says so.
 * ToxBox::Render was never refused (its six-virtual local shadow is over a
 * plain Model at +0xd4, which cxxname_bridge.cpp:511 dual-fills), and
 * unmatched/ToxBox_ShadowFaces.cpp still carries the slice's one measured
 * closure gap.
 */
