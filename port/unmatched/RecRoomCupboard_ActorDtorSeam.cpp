/* HOST SEAM for lane cast-sweep1 / ov058 (level 50, the Rec Room).
 *
 * THIS IS NOT A HOST COPY AND IT REPLACES NOTHING. Both of
 * RecRoomCupboard's destructors are byte-matched src/ TUs and BOTH are in
 * port/slice_sweep1_ov058.txt; this file exists so they can LINK, and it adds
 * one definition rather than taking one away.
 *
 * THE PROBLEM. src/_ZN15RecRoomCupboardD1Ev.cpp and D0Ev.cpp are the //cpp
 * shape: each declares its own local `struct Actor { ~Actor(); };` and calls
 * `((Actor *)c)->~Actor()` to run the base subobject's destructor. MSVC emits
 * that call as ??1Actor@@QAE@XZ -- public, __thiscall, no parameters, `this`
 * in ecx. The port's definition of that body is _ZN5ActorD2Ev, an ordinary
 * CDECL C function taking the pointer on the stack. An /alternatename across
 * those two is a RECEIVER-SHAPE mismatch, not a spelling one: the call site
 * would put `this` in ecx and the callee would read whatever happened to be at
 * [esp+4]. That is the same wall ov024's PyramidTag D1 hit, where the answer
 * was to hold the TU out and transcribe the body. Here it costs one four-line
 * forwarder instead of two whole TUs, so the forwarder wins.
 *
 * WHY THE FORWARDER IS THE RIGHT BODY, from the ROM. Disassembled from
 * extracted/overlays/overlay_0058.bin (T4):
 *     021111a0  push {r4,lr}
 *     021111a4  mov  r4, r0
 *     021111a8  ldr  r12,=0x02111a70   (pool 0x021111d8)  the OWN table
 *     021111ac  ldr  r3, =0x02014a60   (pool 0x021111dc)  the element dtor
 *     021111b0  add  r0, r4, #0xd4
 *     021111b4  mov  r1, #5            five elements
 *     021111b8  mov  r2, #0x40         stride 0x40
 *     021111bc  str  r12, [r4]         install the own table
 *     021111c0  bl   0x0207328c        __destroy_arr
 *     021111c4  mov  r0, r4
 *     021111c8  bl   0x020112c8        Actor::D2   <-- THIS CALL
 *     021111cc  mov  r0, r4  /  return
 * The ~Actor() the source spells IS 0x020112c8, which the port defines as
 * _ZN5ActorD2Ev. The forwarder is a one-for-one bridge and introduces no
 * behaviour of its own.
 *
 * SCOPE. `Actor` is declared here with a NON-VIRTUAL destructor and no
 * members, which is what fixes the mangling at ??1Actor@@QAE@XZ. The real
 * Actor in include/Actor.h has a virtual one and mangles as ??1Actor@@UAE@XZ,
 * a different symbol, so this definition cannot capture a call meant for it.
 * Nothing else in the port defines ??1Actor@@QAE@XZ -- the link's own
 * unresolved list on the first build of this seat is where that was measured.
 * This header is deliberately NOT included anywhere; the file is a definition
 * site and nothing more.
 */
extern "C" void *_ZN5ActorD2Ev(void *self);

struct Actor {
    ~Actor();
};

Actor::~Actor()
{
    _ZN5ActorD2Ev(this);
}
