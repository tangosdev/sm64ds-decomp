/* RETIRED HOST COPY of src/_ZN10MrBlizzard8BehaviorEv.cpp (run linkfull lane
 * PMF2). The matched TU is on port/slice_w27_pmf2.txt and defines
 * ?Behavior@MrBlizzard@@UAEHXZ itself; this file defines nothing and stays only
 * because three target source lists name it by path.
 *
 * WHY THE HOST COPY EXISTED. The matched source dispatches the state cell's
 * tick half as a pointer to member, `(this->**(pp + 1))()`, and the ten cells
 * held raw cdecl bodies that read their receiver at [esp+4]. MSVC's
 * member-pointer call puts the receiver in ECX and pushes nothing, so the copy
 * called the word as a plain function pointer through the seat's own
 * PortMrBlizzardPair layout instead. (The "callee-saved register violation"
 * this class was once parked for was that same seam: a stale stack word read
 * as `this`. hal/actor_classes.inc's gate-192 note has the derivation.)
 *
 * WHY IT IS GONE. With /vmg /vmm target-wide (port/CMakeLists.txt block R8)
 * the matched Behavior's member pointer is the ROM's two-word {code, adj}
 * layout, and the seat in hal/actor_classes_ov081.cpp now writes a __fastcall
 * face into every one of the twenty words, so the ROM's own dispatch runs the
 * cell with the receiver where the face reads it.
 */
