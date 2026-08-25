// The minigame seat's SECOND WAVE: the storage, the two name-spelling faces and
// the named traps the first link asked for. Run link60, lane MG1.
//
// port/slice_mg1.txt is a relocation closure and the linker is the referee. The
// first link named 95 unresolved externals; 33 were matched TUs the closure
// reaches but the slice did not list, and they went into the slice's second
// block. The other 62 are here, in four groups, and each group is a different
// kind of statement.
//
// ---- 1. THREE arm9 BSS WORDS, SIZED BY ROM SPAN ---------------------------
//
// The ordinary treatment hal/scene_boot.cpp already gives seven of them for the
// star select. config/arm9/symbols.txt has all three as kind:bss and the next
// symbol in each case is four bytes on, so four bytes is the ROM's own span and
// not the width the one caller happens to touch -- the undersized-global rule.
// The DS clears bss at boot, so a zero-initialised host global reads exactly
// what the real machine reads.
//
// ---- 2. TWO NAME-SPELLING FACES, NEITHER A STAND-IN FOR A BODY -------------
//
// _ZTV14dScMgCurling_c. src/MgShuffleShell_Spawn.c writes the class vtable
// through this spelling, and no config holds that name. IT IS NOT A GUESS
// THOUGH, and that is worth separating from the ov007 lane's VT0/VT1/VT2 case:
// the ROM's own RTTI string at 0x0213c2d0 reads "14dScMgCurling_c", so the
// decomp recovered the right class name and the port simply has the table under
// its dsd address. The address is settled by the ROM twice: the RTTI, and
// config/arm9/overlays/ov006/relocs.txt's
//     from:0x020e3850 kind:load to:0x0213c304 module:overlay(6)
// where 0x020e3850 is inside MgShuffleShell_Spawn (0x020e3820, 0x34 bytes).
//
// func_020adc74. src/func_ov006_020e3578.c (InitResources) spells its callee
// with no overlay in the name, and no arm9 symbol exists at 0x020adc74. The
// reloc reads module:overlays(3,4), and ov003 can never be co-resident with
// ov006 while ov004 always is -- func_0201a798 loads the pair together -- so
// ov004's body is the only answer residency allows. Same shape as the twelve
// ov001 sprite-template faces the star-select seat carries.
//
// ---- 3. SIX NAMED TRAPS, AND WHY A PLAUSIBLE BODY WOULD BE WORSE -----------
//
// Five ov004/ov006 addresses in this closure have NO delink block in their
// overlay's delinks.txt and no src file anywhere, and one arm9 address has no
// config symbol at all. They are not decompiled and this lane does not guess
// them; port/tools/inferred_stub_guard exists to refuse exactly the plausible
// body that would go here. Each trap is named, counted, and reports itself, so
// "none of them fired" is a measurement rather than an absence.
//
// THREE OF THE SIX ARE ON dScMgCurling_c's OWN PATH and one of those three is
// a live STATE TARGET: func_ov006_020e1854 is the code word of the
// pointer-to-member pair at 0x0213c2bc, so this class cannot reach every one of
// its own states even after section 4 of port/mg_fanout_costs.txt is solved.
// (It has no trap here because nothing in the slice references it by name --
// the pair word is mounted DATA holding a DS address -- and a trap with no
// caller is a definition the linker drops. The omission cannot go quiet: a
// future slice that reaches it fails the link with an unresolved external,
// which is the same reasoning the ov007 lane applied to func_ov007_020c49bc.)
// AMENDED, run link60 lane MG2. THE ADDRESS IS REFERENCED NOW, by the address
// switch in unmatched/MgCurling_StateDispatch.cpp, which is what a dispatch
// fix does to every state word. It still gets no symbol here: inventing a
// definition for a body with no source is exactly the guess
// port/tools/inferred_stub_guard refuses. The case is handled at the dispatch
// site instead, where the report can say which state was wanted rather than
// only that something was missing.
//
// AMENDED AGAIN, run link60 lane CT1. "This class cannot reach every one of its
// own states" is NO LONGER TRUE and this is the correction. The state is
// transcribed from the ROM -- 0x300 bytes read out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0, the base proved
// against the very reloc named above, the pair word at 0x0213c2bc -- and it
// lives in unmatched/MgCurling_State_020e1854.cpp. The dispatch case calls it
// instead of reporting, so all twenty-five states run.
//
// WHAT STILL STANDS, unchanged and deliberately so: it STILL gets no symbol
// here, and the reasoning in the paragraph above is exactly why. The decomp has
// no body for 0x020e1854. The host body carries a port_ name
// (port_mg_curling_st_020e1854, the same shape as port_player_st_climb_main),
// so nothing in this tree claims a decompilation that does not exist.
//
// The refusal that keeps saying so is stategen.py's, and ONLY stategen's.
// inferred_stub_guard has no row for this address and cannot have one: it
// ratchets bodies in src/ that were guessed from a vtable slot and carry the
// "recovered from vtable slot identity" marker, and it only counts them once a
// port vtable fill SEATS them. There is no src file here at all, so there is
// nothing for it to ratchet. Naming it as a second net overstated the cover.
//
// ---- 4. THE mwcc POINTER-TO-MEMBER TU, WHICH IS THE WALL -------------------
//
// src/func_ov004_020b87e0.cpp is EXCLUDED from the slice and trapped here
// instead, and it is the single most important thing in this file because it is
// where the minigame seat stops.
//
// Twenty-one of the first link's ninety-five unresolved externals came from
// that one TU, all of the form
//
//   unresolved external symbol "void (__thiscall C::* data_ov004_020bc97c)(void)"
//     (?data_ov004_020bc97c@@3P8C@@AEXXZQ1@)
//
// and the mangling is the whole story: MSVC encodes the member-pointer TYPE into
// the symbol name, so a global the ov004 mount emits as the C symbol
// _data_ov004_020bc97c can never satisfy it. That is only the visible half.
// THE TU IS UNCOMPILABLE BY MSVC IN THREE INDEPENDENT WAYS, any one of which is
// fatal on its own:
//
//   the SIZE. mwcc's pointer-to-member is eight bytes, {code, adjustment}.
//   MSVC's single-inheritance one is four. The TU's own `struct C` places
//   field_8 at +8 after a leading PMF, which is only true at eight bytes, so
//   MSVC lays the object out wrong before any dispatch happens.
//   the CONTENT. Those twenty-one globals live in the ov004 mount and hold raw
//   DS code addresses. There is no host code at any of them.
//   the DISPATCH. `(self->*self->onUpdate)()` compiles to the ARM Itanium
//   sequence in the ROM -- virtual bit in the adjustment's LSB, `this` advanced
//   by the adjustment arithmetic-shifted right one -- and MSVC emits its own
//   incompatible shape.
//
// THE PORT ALREADY KNOWS THE ANSWER AND IT IS A LANE, NOT A LINE.
// port/unmatched/Player_ChangeState.cpp is the precedent: a host copy of the
// dispatching TU with the member-pointer site replaced by hal_call_state_fn, an
// address switch from DS code address to a real __thiscall call, generated into
// hal/player_states.inc with 197 cases. func_ov004_020b87e0 needs the same for
// its twenty, and dScMgCurling_c needs it for its own twenty-five across five
// more TUs. That is costed in port/mg_fanout_costs.txt section 4 and it is the
// next lane.
//
// SO THE TRAP IS THE HONEST SEAT. It returns without dispatching and says so.
// A minigame whose framework reaches this function does not run past it, and
// the run reports that rather than jumping to a DS address as a host one.
//
// SEATED, run mg5 lane BASESET, AND EVERYTHING ABOVE IS NOW HISTORY. The trap
// is deleted from this file and func_ov004_020b87e0 is a host copy in
// port/unmatched/MgBase_StateSetter.cpp, which carries the derivation: the
// twenty globals with their code words and relocation rows, the ROM
// disassembly of the table build and the dispatch, and the object layout the
// offsets force. src/func_ov004_020b87e0.cpp stays off every slice.
//
// READ THE SECTION ABOVE FOR WHAT THE TRAP COST RATHER THAN FOR WHAT TO DO.
// "A minigame whose framework reaches this function does not run past it" was
// exactly right and it was a FREEZE in a player's hands, twice on 0.2.8.108:
// Wanted!'s fifth-win score collapse asked for state 1 and Coincentration's tap
// asked for state 16, and both times the frame loop and the music kept running
// over a class that could not advance.

#include <cstdio>
#include <cstdlib>
#include "dsstate_seg.h"

/* EVERY HOSTED DS GLOBAL IN THIS FILE IS INSIDE THE SAVE-STATE SPAN, and it
   took wiring the file to find that out. MG1 wrote sections 1 and 1b and left
   them out of every target, so hal/dsstate_seg.h's guard -- which reads the
   LINKED map -- had never had an opinion on them. The first link that closed
   named all twelve at once:

     dsstate_guard: 12 hosted DS symbol(s) are OUTSIDE .dsstate and would NOT
     be captured by a save state

   They are DS storage by definition, so all twelve belong in the captured
   span and the bracket goes round the whole block rather than round the three
   this lane added. Nothing here is host bookkeeping: the header's "what must
   not go in here" list is the playlog path, the frame counter and the window
   pacing, and this file has none of those. A seat that is derived but not
   wired cannot be checked by a guard that reads a map, which is the general
   form of it and is worth the next lane knowing. */
DSSTATE_BEGIN

extern "C" {

/* ---- 1. the arm9 bss words, sized by ROM SPAN and not by field width ---- */
unsigned int data_0209d458;      /* next symbol +4  */
unsigned int data_0209d460;      /* next symbol +4  */
unsigned int data_0209d4b8;      /* next symbol +4  */
unsigned int data_0209d474;      /* next symbol +4  */
unsigned int data_0209d488;      /* next symbol +4  */
/* data_0209caf4 USED TO BE DEFINED HERE, at the right 728 bytes: this file
   applied the undersized-global rule to it (config/arm9/symbols.txt's next
   symbol is data_0209cdcc) and that sizing was correct. What it could not do
   from here was put the storage NEXT TO its four siblings, and the symbol is
   the fifth piece of one save object the delink split five ways. Run link60
   Stage 5 lane SV1 moved it to hal/level_boot.cpp's SAVEBLK group as
   .dsstate$savblk0004, because SaveData::SetDefaultValuesMg writes 0x2e4 bytes
   from data_0209cae4 and that span only exists if the five are contiguous. The
   sizing argument travelled with it and is quoted there. Both files are in the
   same three targets, so no build lost the symbol. */

/* ---- 1b. two NON-bss words, carrying the ROM's own bytes ---------------- */
/* arm9 .data, four bytes by span, read at (0x0208e424 - 0x02004000) out of
   extracted/arm9_dec.bin. It is ff 00 00 00: a -1 sentinel in the low byte,
   which is what a caller testing it for "not set" needs to see. Zeroing it
   would be a different value, so the bytes are transcribed rather than
   defaulted. */
unsigned char data_0208e424[4] = { 0xff, 0x00, 0x00, 0x00 };

/* ov000 .data, twenty-four bytes by span, read at (0x020ad494 - 0x020aa420)
   out of extracted/overlays/overlay_0000.bin. IT IS A FILENAME STRING,
   "thum_shinkei_ncg.bin" followed by four NULs -- shinkei is the memory-match
   minigame -- and a minigame's InitResources loads it by name, so an empty
   host global would be a load of "" rather than a load of nothing.
   WHY IT IS HOSTED HERE RATHER THAN MOUNTED. ov000 has no mount and must not
   get one from this lane: its footprint 0x020aa420..0x020bf4e0 COVERS ov004's
   whole window, and port/ov004_ov006_binding_diff.txt section 1 measured what
   an ov000 mount would do -- 20 of this build's rebased cross-mount pointers
   are withdrawn, all twenty of them ov006 -> ov004 rows this same pair
   created. A single 24-byte string is not worth that, and the binding diff
   asks the next lane not to do it by accident. */
unsigned char data_ov000_020ad494[24] =
    "thum_shinkei_ncg.bin\0\0\0";

/* arm9 .data, EIGHT bytes by span -- the next symbol is data_0208ee00. Read at
   (0x0208edf8 - 0x02004000): "LZ77" and four NULs, the compression magic the
   archive loader compares a header against. Named by src/func_02018568.c, one
   of the sixth wave's five. */
unsigned char data_0208edf8[8] = "LZ77\0\0\0";

/* arm9 .data, FOUR bytes by span -- the next symbol is data_0208eb58. Read at
   (0x0208eb54 - 0x02004000): ff 00 00 00, the same -1-in-the-low-byte sentinel
   as data_0208e424 below and hosted for the same reason, that a zeroed word is
   a DIFFERENT value to a caller testing it for "not set". Named by
   src/func_02018770.c. */
unsigned char data_0208eb54[4] = { 0xff, 0x00, 0x00, 0x00 };

/* arm9 .data, TWENTY bytes by span -- the next symbol in
   config/arm9/symbols.txt is data_0208ec88. Read at (0x0208ec74 - 0x02004000)
   out of extracted/arm9_dec.bin: "myFS_OpenFileFast" plus three NULs, a
   sibling of data_0208ecd8 below and the same kind of thing, a NitroFS
   entry-point name the card loader hands its own resolver. Reached through
   src/func_02018dc4.c, one of the archive-loader TUs the two SharedFilePtr
   constructors pull in. Named by review's converged link as the sixth wave's
   one hosted word and measured here; nothing in the port's fs seam consumes
   it, and the ROM's bytes are transcribed rather than zeroed for the reason
   the next block gives. */
unsigned char data_0208ec74[20] = "myFS_OpenFileFast\0\0";

/* arm9 .data, twenty-eight bytes by span -- the next symbol is data_0208ecf4,
   which is the 13-entry archive-mount table hal/scene_boot.cpp's LoadArchive
   face describes. Read at (0x0208ecd8 - 0x02004000) out of
   extracted/arm9_dec.bin: the string "myFS_ConvertPathToFileID" plus four
   NULs, the NitroFS entry-point name the card loader hands its own resolver.
   The port's fs seam (hal/fs.cpp) resolves file ids lazily and never reaches
   that resolver, so nothing consumes the string -- but it is a named symbol
   the closure references, and transcribing the ROM's bytes is cheaper than
   reasoning about whether a zeroed one would ever be read. */
unsigned char data_0208ecd8[28] = "myFS_ConvertPathToFileID\0\0\0";

}  /* extern "C" */

DSSTATE_END

/* ---- 2. the two name-spelling faces ------------------------------------ */
#pragma comment(linker, "/alternatename:__ZTV14dScMgCurling_c=_data_ov006_0213c304")
#pragma comment(linker, "/alternatename:_func_020adc74=_func_ov004_020adc74")

/* ---- 2b. THE FOUR ALIAS ROWS A GENERATOR MUST NOT WRITE ------------------
 *
 * The other nineteen alias rows of this seat's wall are generated into
 * hal/scene_mg_faces_gen.cpp by port/tools/facegen.py and verified by it.
 * These four are here because facegen refused three of them and MISCLASSIFIED
 * the fourth, and both of those are the tool behaving as its docstring says
 * it should -- "judgment rows go to a human". The refusal text is quoted
 * verbatim so a reader can see what was refused and what the ruling was.
 *
 *   "struct-typed global: PMF pair tables travel in this spelling, rule it
 *    by hand"
 *
 * RULED AGAINST THE ROM, one at a time, by reading the words out of
 * extracted/overlays/overlay_0004.bin at (addr - 0x020ad660):
 *
 *   data_ov004_020bc27c   00000380 00000300 00000340
 *     THREE PLAIN INTS. src/func_ov004_020b3278.cpp spells it
 *     `struct S3 { int v[3]; }` and reads tmp.v[i] as an int argument. Not a
 *     pair table; twelve bytes that mean the same twelve bytes on both ABIs.
 *     ALIAS IS CORRECT.
 *   data_ov004_020bc904   020b7c04 00000000
 *   data_ov004_020bc914   020b7fec 00000000
 *     THESE TWO ARE mwcc MEMBER POINTERS -- {code, adjustment} with the code
 *     word inside ov004's .text (0x020ad660..0x020b944c) and the adjustment
 *     zero -- and the refusal was right to stop on them. The alias is still
 *     the correct answer, and THE TEST IS WHAT THE CONSUMER SPELLS, NOT WHAT
 *     THE CONSUMER DOES. src/func_ov004_020b7cd0.cpp and
 *     src/func_ov004_020b72d4.cpp each declare `struct Pair { int a; int b; }`
 *     and store .a and .b into the object's own eight-byte state field at
 *     +8/+0xc. That struct is eight bytes on MSVC and eight in the ROM, so the
 *     copy lands exactly the ROM's two words and the alias is sound. The DS
 *     code address that arrives in that field is then dispatched by
 *     unmatched/MgBase_StateDispatch.cpp, which keys on the DS address.
 *
 *     DO NOT RESTATE THIS AS "A COPY IS SAFE AND A CALL IS NOT". A consumer
 *     that only copies, but copies through a struct holding a REAL MSVC member
 *     pointer, moves four bytes where the ROM moves eight and shifts every
 *     field after it in the same object, with no call anywhere. The rule is:
 *     TWO INTS ALIAS, A MEMBER-POINTER TYPE NEEDS A HOST COPY, called or not.
 *     That is why these two rows are aliases and the seven in section 4 are
 *     not, and it is also why the four ov004 TUs that dispatch an object FIELD
 *     need host copies even though no global of theirs was ever unresolved.
 *
 * AND ONE THE GENERATOR DID NOT REFUSE AND SHOULD HAVE. facegen classified
 *
 *     ?data_ov006_02141950@@3PAUEntry@@A          ALIAS
 *
 * which is the twenty-five-entry dScMgCurling_c STATE TABLE -- the exact
 * table port/mg_fanout_costs.txt section 4 opens with. Its WALL test is
 * `"P8" in sym`, and a struct that WRAPS a member pointer hides the P8:
 *
 *     PMF table[]                    ->  ?..@@3PAP8C@@AEXXZA    caught
 *     struct Entry { PMF pmf[1]; }[] ->  ?..@@3PAUEntry@@A      MISSED
 *
 * The struct-typed refusal does not catch it either, because it tests for
 * the by-value spelling @@3U and an array of that struct is spelled @@3PAU.
 * NO ALIAS IS WRITTEN FOR IT ANYWHERE. Its one consumer,
 * src/func_ov006_020e3528.cpp, is host-copied in
 * unmatched/MgCurling_StateDispatch.cpp, so after that host copy the symbol
 * is referenced by nothing and an alias for it would be a dead directive.
 *
 * A ROUND-2 ADDENDUM: THE FOUR @@3PAU ROWS, THEIR PROVENANCE AND THE PRICE.
 * The three rulings above are this section's only hand-ruled alias rows and
 * all three are @@3U by-value spellings. The GENERATED file beside this one
 * also carries four @@3PAU rows that the pre-hardening generator
 * auto-aliased and nobody individually adjudicated at the time:
 *
 *     ?data_ov004_020beb68@@3PAUBase@@A   ?data_ov004_020bf428@@3PAUPair@@A
 *     ?data_ov004_020beb68@@3PAUObj@@A    ?data_ov004_020bf4f8@@3PAUPair@@A
 *
 * The hardened facegen refuses EVERY @@3PAU spelling, because @@3PAU is the
 * state table's spelling class above, so regenerating this wall sends these
 * four to a human. THE RULING FOR ALL FOUR IS ALIAS, and the adjudication
 * is written here so the next regenerator does not read the refusal as a
 * bug: all three addresses sit in ov004's .bss, 0x020beb60..0x020bfec0 by
 * that overlay's delinks.txt, zero-initialised storage the ROM never
 * images, so none of them can be a ROM .data pair table and the mount
 * hosts them as plain zeroed spans. (Scout round 2's reviewer adjudicated
 * this; the span containment is re-derived from the delinks lines each
 * time it is quoted.) THE PRICE, measured by reclassifying this wall's
 * nineteen generated rows under the hardened tool: fifteen re-emit and
 * these four refuse, so a wall of this shape costs about four rulings like
 * this paragraph. The trade is deliberate: the spelling class that once
 * shipped a stride-corrupting alias can never auto-pass again.
 */
#pragma comment(linker, "/alternatename:?data_ov004_020bc27c@@3US3@@A=_data_ov004_020bc27c")
#pragma comment(linker, "/alternatename:?data_ov004_020bc904@@3UPair@@A=_data_ov004_020bc904")
#pragma comment(linker, "/alternatename:?data_ov004_020bc914@@3UPair@@A=_data_ov004_020bc914")

/* ---- 2d. ONE MORE, FROM THE SEVENTH WAVE --------------------------------
 *
 * src/func_0201834c.c calls the archive allocator by its ITANIUM name at C
 * linkage, and the matched body src/_ZN6Memory8AllocateEji.cpp is a real C++
 * definition that MSVC mangles its own way. Confirmed off the compiled object
 * rather than derived from the source:
 *
 *     ?Allocate@Memory@@YAPAXIH@Z   void * __cdecl Memory::Allocate(unsigned int,int)
 *
 * facegen refused the row:
 *
 *   "reverse face with 2 args: hand-write it against the cxxname_bridge
 *    precedent"
 *
 * and refusing was right, but the reason it gave is not this symbol's. Memory
 * is a NAMESPACE, not a class, and Allocate is a __cdecl free function inside
 * it -- the Y and the A in the mangle say so. So no face is needed and no
 * argument has to be re-landed: both sides are __cdecl with the same two
 * arguments, and a plain alias is exactly right. facegen reads
 * ?Allocate@Memory@@... as a method because its MSVC_METHOD pattern cannot
 * tell a namespace qualifier from a class one, and its ALIAS_FN path only
 * recognises free functions at global scope (?name@@YA...). The tool's own
 * ALIAS rule -- "plus __cdecl free functions onto a same-named C definition"
 * -- covers this row; the classifier just cannot see that it does.
 */
#pragma comment(linker, "/alternatename:__ZN6Memory8AllocateEji=?Allocate@Memory@@YAPAXIH@Z")

/* ---- 2e. UnloadArchive, A FACE THIS LANE DERIVED AND ANOTHER LANE LANDED -
 *
 * THE DEFINITION LIVES IN hal/stage_slot0.cpp (lane SD0, same stage), which
 * landed first and carries the identical empty body as one of a three-body
 * family. Both lanes derived the same design independently; at the merge the
 * two definitions collided (LNK2005) and this one yielded. The derivation
 * below stands as this lane's evidence, and the caller-arity paragraph at the
 * bottom is recorded nowhere else, so the block stays.
 *
 * PORT_HOST_ABI. The eighth wave's src/func_02018770.c calls UnloadArchive,
 * and taking the matched body would have meant taking the thing
 * hal/scene_boot.cpp's LoadArchive face already refused once. That file's
 * reasoning is this file's, and it is repeated rather than re-derived:
 *
 *   src/UnloadArchive.c walks data_0208ecf4, the ROM's 13-entry archive-mount
 *   table of {ptr, heap, idBase, idEnd, shortName, narcPath} WHOSE ENTRIES ARE
 *   DS STRING POINTERS, and calls func_02018908 into the card loader beneath
 *   it. The port never mounts an archive: hal/fs.cpp's port_fs_archive_fill
 *   resolves archive-interior file ids (>= 0x8000) lazily out of
 *   port_archive_map, so on the host an archive is never "mounted" and never
 *   "not mounted". There is nothing for an unmount to undo, and hosting a
 *   pointer-bearing ROM table to drive one would be "a fake, not a fix".
 *
 * THE OBSERVABLE IS NOTHING. The ROM's UnloadArchive returns void, has no
 * out-parameter, and its only effect is to zero two words of a table the host
 * does not have. So the face is empty, and empty is the whole of it.
 *
 * AND ITS ONE CALLER DROPS THE ARGUMENT. src/func_02018770.c:9 declares
 * `extern void UnloadArchive(void);` and calls it with none, while the real
 * definition takes `int i` -- the same ARM ride-through family as the two
 * defects port/mg_fanout_costs.txt section 6 records for slots 5 and 7, where
 * r0 already holds the value and the host callee reads the stack. Here it is
 * INERT rather than fixed, and the difference is worth stating: this face
 * ignores its argument, so a garbage index cannot select a wrong archive. The
 * face keeps the ROM's arity so that a future caller which does pass one is
 * still calling the same function. (The arity note applies to the surviving
 * definition in hal/stage_slot0.cpp, which also takes and ignores the int.)
 */

/* ---- 2c. THE ONE ARGUMENT-LANDING FACE ----------------------------------
 *
 * src/func_ov004_020b08f0.cpp (dScMgBase_c::AfterInitResources, vtable slot 2)
 * declares a local `struct Scene` with a non-virtual
 * `void AfterInitResources(unsigned int)` and calls it, which MSVC mangles
 * __thiscall as ?AfterInitResources@Scene@@QAEXI@Z. facegen refused the row:
 *
 *   "no Itanium body for Scene::AfterInitResources"
 *
 * and the refusal is right twice over. There is no _ZN5Scene18AfterInit-
 * ResourcesEj in the image, because hal/scene_actor_faces.cpp's header
 * explains that the three Scene:: veneer TUs are deliberately NOT in
 * port/slice_scene1.txt -- and if there were one it would be the wrong
 * target anyway. AN ALIAS CANNOT CHANGE A CALLING CONVENTION, and this is
 * that rule's other half: the caller is __thiscall with an argument, and
 * src/_ZN5Scene18AfterInitResourcesEj.cpp is a `void f(void)` transcription
 * of a 0xc-byte ARM tail-call veneer (ldr ip,[pc]; bx ip; .word 0x2013ef4)
 * whose arguments ride through in r0/r1. On the host it would drop both.
 *
 * So the face lands them, and it lands them where the veneer points --
 * ActorDerived::AfterInitResources at 0x02013ef4 -- rather than at the
 * veneer. That is not a new ruling: hal/scene_actor_faces.cpp already made
 * it for the scene-1 seat and exports it as port_scene_after_init, and this
 * face is the __thiscall spelling of the same edge. Getting it wrong is not
 * cosmetic: vfSuccess == 1 marks the actor for destruction, so a garbage
 * argument decides on the first frame whether the minigame scene survives.
 */
extern "C" void port_scene_after_init(void *self, unsigned vfSuccess);

struct Scene { void AfterInitResources(unsigned int flags); };

void Scene::AfterInitResources(unsigned int flags)
{
    port_scene_after_init(this, flags);
}

/* ---- 3 and 4. the traps ------------------------------------------------ */
static unsigned g_mg_trap_hits;

static void mg_trap(const char *name)
{
    static int said[8];
    ++g_mg_trap_hits;
    /* one line per distinct site, so a loop cannot flood the log and a single
       entry cannot hide in one */
    unsigned h = 0;
    for (const char *p = name; *p; ++p) h = h * 31u + (unsigned char)*p;
    h %= 8u;
    if (!said[h]) {
        said[h] = 1;
        std::fprintf(stderr, "  [scene] UNMATCHED ov004/ov006 body entered: "
                     "%s (returns 0; port/hal/scene_mg_faces.cpp)\n", name);
        std::fflush(stderr);
    }
}

extern "C" unsigned port_mg_trap_hits(void) { return g_mg_trap_hits; }

/* ---- lane PANEL's end-of-run readout of the results panel ----------------
 *
 * The seat above draws the three buttons. Whether the player can USE them is a
 * different claim and it needs a different number, because the two halves of
 * the panel live in different files: src/func_ov004_020ae858.cpp DRAWS off
 * self+0x4646, and src/func_ov004_020aeb24.cpp (matched, untouched) is the
 * STYLUS HIT TEST that WRITES it -- it boxes the stylus point against the three
 * button centres and on a hit stores the button's index there and 0x10 at
 * +0x4644. So +0x4646 moving from -1 to 0/1/2 is the hit test accepting a tap,
 * read off the same field the renderer reads, and the two halves check each
 * other rather than being asserted separately.
 *
 * Printed at exit rather than per frame so it cannot flood a log or perturb a
 * frame, and only when SM64DS_MG_RESULTS_PROBE is set. Unset, this is one getenv
 * at exit and nothing else.
 *
 * The probe that reads it and raises the panel through the ROM's own slot 27 is
 * OVERLAY's instrument in hal/scene_mg.cpp; this readout is a reader, not a
 * driver, and must not grow its own copy of it. (On the pre-merge mg12-panel
 * branch that made this readout inert by construction: the lane dropped its
 * byte-identical cherry-pick of the probe after a trial merge showed it
 * conflicting with OVERLAY's later fixes. Both branches are merged now, the
 * probe is live here, and func_ov006_020d01e0 is seated - the mg12 burst drove
 * this readout on the merged tree and it printed. What still stands between the
 * panel and visible pixels at that measurement: the renderer runs and writes all
 * 128 sub-OAM slots, but the three button centres compute off the 256x192 screen
 * ((-128,48), (384,96), (128,224)) and the +0x4640 animation counter stays 0
 * three hundred frames after the raise, so every sprite parks on the hide row.
 * The next floor is whatever fills +0x4634..+0x463e and steps +0x4640.)
 */
extern "C" { extern void *data_ov004_020beb68; }

static void panel_end_readout(void)
{
    const char *e = std::getenv("SM64DS_MG_RESULTS_PROBE");
    if (!e || !e[0])
        return;
    char *g = (char *)data_ov004_020beb68;
    if (!g) {
        std::printf("[panel-end] no live dScMgBase_c\n");
        std::fflush(stdout);
        return;
    }
    std::printf("[panel-end] up=%d selected=%d hold=%d anim=%u "
                "buttons (%d,%d) (%d,%d) (%d,%d)\n",
                *(int *)(g + 0x4628), (int)*(short *)(g + 0x4646),
                (int)*(short *)(g + 0x4644), *(unsigned *)(g + 0x4640),
                (int)*(short *)(g + 0x4634), (int)*(short *)(g + 0x4636),
                (int)*(short *)(g + 0x4638), (int)*(short *)(g + 0x463a),
                (int)*(short *)(g + 0x463c), (int)*(short *)(g + 0x463e));
    std::fflush(stdout);
}

namespace {
struct PanelEndReg { PanelEndReg() { std::atexit(panel_end_readout); } };
PanelEndReg g_panel_end_reg;
}

extern "C" {

/* Lane CUR2's two seated bodies, in port/unmatched/. */
void port_mg_curling_collide_020e1dc8(char *self, int idx);
void port_mg_curling_collide_020e20bc(char *self, int idx);

/* Run mg5 lane WTIMER's seated body, in port/unmatched/MgHud_ScaledNumber.cpp. */
void port_mg_hud_scaled_number_020b2220(int x, int y, int num, int a3, int a4,
                                        int scale, int angle);

/* ---- SEATED, run mg12 lane PANEL ---------------------------------------
 *
 * THE LAST ov004 TRAP IN THIS FILE IS GONE, and it was the RESULTS PANEL's
 * label renderer. func_ov004_020ae858 draws the three buttons a minigame's
 * results screen offers -- the play-again row -- and returning 0 from it is
 * exactly the defect the owner reported as the play-again buttons never
 * appearing: dScMgBase_c::BeforeRender (src/func_ov004_020b04f4.cpp) hands the
 * WHOLE frame to this body while the panel is up and returns, so with a stub
 * behind it nothing submits the labels to either engine.
 *
 * THE MEASUREMENT THAT PROVED IT was run mg12 lane OVERLAY's: raising the panel
 * through the ROM's own slot 27 on scene 384 and holding it for 300 frames gave
 * 300 entries into this body, exactly one per frame, with OAM engine A 0 placed,
 * OAM engine B 0 placed and the whole bottom half a black backdrop.
 *
 * THE SIGNATURE CHANGED WITH THE SEAT, and as with the bodies below that is not
 * cosmetic: the trap returned int where the ROM returns VOID. No path in the ROM
 * body sets r0 on the fall-through -- the early-out is `if (pred) return;` and
 * the tail is a bare epilogue -- so the int was never the ROM's. The one caller
 * discards it, which is why nothing ever caught it. include/decl_common.h is
 * corrected in the same commit.
 *
 * IT IS A REAL DECOMPILATION, honestly NONMATCHING. src/func_ov004_020ae858.cpp
 * differs in 10 of 118 words at mwccarm 2004/b56, the closest of all 25 installed
 * builds, on the base-materialization/addressing floor; every reloc and the whole
 * literal pool are identical and its banner carries the measurement. The symbol
 * comes from port/slice_mg1.txt, so this line is simply deleted.
 *
 * WHAT IT STILL GETS NO SYMBOL FOR: nothing changes about the decomp hole.
 * config/arm9/overlays/ov004/delinks.txt still covers no part of 0x020ae858 --
 * the blocks run to 0x020ae5c4 and resume at 0x020aea30, and 0x020ae5c4 + 0x294
 * + 0x1d8 is exactly that gap, so this body and the line rasteriser above fill it
 * between them. 0x020ae858 remains open delink work.
 */

/* ---- SEATED, run mg10 lane F371 ----------------------------------------
 *
 * func_ov004_020ae5c4 IS NOT A TRAP ANY MORE, and it was the family's LINE
 * RASTERISER. Run mg9 lane BOO stood the trap here rather than in a lane's own
 * hal file because the body is the FAMILY's: seven ov006 TUs across the family
 * call it (func_ov006_020d1ba0, _020d2580, _020d3ba0, _0211f9fc, _0212157c,
 * _02121d64 and _02124088), so a per-lane copy would have become a duplicate
 * symbol. That reasoning was right and it is why this single line is simply
 * deleted rather than moved.
 *
 * IT IS A REAL DECOMPILATION. src/func_ov004_020ae5c4.c is a Bresenham walk
 * from (x0,y0) to (x1,y1) that stamps vtable slot 34 at every lattice point it
 * visits, and the symbol comes from port/slice_mg1.txt. The seven parameters
 * lane BOO derived off the prologue were correct and the new body spells the
 * same seven.
 *
 * AND DELETING IT IS WHAT MAKES SLOT 34 REACHABLE. This body is the tree's
 * ONLY slot-34 dispatcher -- eight `ldr Rd,[Rn,#0x88]` sites, every one of them
 * inside it, each storing one word at [sp] before the blx. While it was
 * trapped, slot 34 was unreachable on every scene rather than merely unreached,
 * and mb_v34 below could carry a five-argument defect with no potential witness.
 * That is repaired in the same commit and the witness is a run of scene 371.
 */

/* ---- SEATED, run mg7 lane L369 -----------------------------------------
 *
 * func_ov004_020b1710 IS NOT A TRAP ANY MORE, and it was the minigame HUD's
 * value drawer. The trap took (void *) and returned int where the ROM takes
 * four arguments and returns void, so the x, the y and the number the six
 * matched call sites in src/func_ov004_020b14f0.c pass were all sitting on
 * the stack with nothing reading them -- the same defect class as
 * func_ov004_020b2220 below, found the same way: a 3000-frame boot of scene
 * 369 under SM64DS_FAULTS_FATAL=1 entered this one 5657 times.
 *
 * IT IS A REAL DECOMPILATION, not a host copy. src/func_ov004_020b1710.c
 * byte-matches at mwccarm 1.2/base, 1.2/sp2 and 1.2/sp2p3 and carries its own
 * delink block in config/arm9/overlays/ov004/delinks.txt, so there is nothing
 * to route to and no port_ name to spell: the symbol comes from
 * port/slice_mg1.txt and this line is simply deleted. Two of the six bodies
 * that file's header calls unmatched are real decompilations now, which
 * left func_ov004_020ae858 as the last ov004 trap in this file, and run mg12
 * lane PANEL has now seated that one too.
 */

/* ---- SEATED, run mg5 lane WTIMER ----------------------------------------
 *
 * THE THIRD OF THE THREE ov004 ADDRESSES IS NOT A TRAP ANY MORE, and it was
 * the "Wanted!" countdown timer. func_ov004_020b2220 is the minigame HUD's
 * scaled number drawer, and returning 0 from it is exactly the defect the
 * owner reported on 2026-08-19 as "Also time is not showing in wanted":
 * dScMgLuigi_c computed the countdown correctly, drew the language label
 * sprite above it correctly, and then handed the number to a stub.
 *
 * THE MEASUREMENT THAT FOUND IT. A 300-frame headless boot of scene 366 on
 * cons 2fbc2c0a1 reported 127 entries into port_mg_trap_hits where every
 * sibling minigame scene (368, 374, 376, 378, 390) reports exactly 1. A
 * per-site census of that counter split the 127 as
 *     func_0202e78c              1     the long-standing pre-existing one
 *     func_ov004_020b2220      126     this body, roughly every other frame
 * and an argument probe on the trap showed the timer arriving correctly and
 * counting down (val=10 on the first calls, val=9 by call 64). The value was
 * never the problem. Nothing drew it.
 *
 * THE SIGNATURE CHANGED WITH THE SEAT, and as with the two curling bodies
 * below that is not cosmetic: the trap took (void *) where the ROM and all six
 * of its matched callers take seven arguments, so x, y, the number, the scale
 * and the angle were all already on the stack and the trap never looked at any
 * of them. It also returned int where every caller declares void.
 *
 * WHAT STILL GETS NO SYMBOL HERE: nothing changes about the decomp hole.
 * config/arm9/overlays/ov004/delinks.txt still covers no part of 0x020b2220 --
 * the blocks run 0x020b1ea4..0x020b2220 and then jump to 0x020b2444 -- and
 * there is still no src TU for it on any branch, origin/main included. The
 * body is a host copy in port/unmatched/ carrying its provenance banner and a
 * port_ name, so nothing in this tree claims a decompilation that does not
 * exist. 0x020b2220 remains open decomp work.
 */
void func_ov004_020b2220(int x, int y, int num, int a3, int a4,
                         int scale, int angle)
{
    port_mg_hud_scaled_number_020b2220(x, y, num, a3, a4, scale, angle);
}

/* ---- SEATED, run link60 lane CUR2 ---------------------------------------
 *
 * TWO OF THE FIVE ARE NOT TRAPS ANY MORE. func_ov006_020e1dc8 and
 * func_ov006_020e20bc are dScMgCurling_c's two shell-against-shell collision
 * bodies, and returning 0 from them is exactly the defect the owner reported
 * on 2026-08-15 as "no collision against each other on the shells": the
 * shells slid through one another because the only two functions in the class
 * that move a shell out of another shell's way were both faces.
 *
 * They are transcribed from the ROM in port/unmatched/, the same treatment
 * and the same naming rule CT1 applied to func_ov006_020e1854 -- a port_ name,
 * so nothing in this tree claims a decompilation that does not exist. The
 * derivation, the base proof and the callee census are on those two files;
 * port/curling_round2.txt is the lane's evidence trail.
 *
 * THE SIGNATURES CHANGED WITH THE SEAT, and that is not cosmetic. The traps
 * took (void *) and the ROM takes (self, idx): the matched callers
 * src/func_ov006_020e2c08.c and src/func_ov006_020e2868.c both declare and
 * pass two arguments, so the second was already on the stack and the trap
 * simply never looked at it. A body that ignored it would collide shell 0
 * every time.
 *
 * WHAT STILL GETS NO SYMBOL HERE: nothing changes about the decomp holes.
 * Neither address has a delink block in config/arm9/overlays/ov006/delinks.txt
 * and neither has a src TU. port/tools/inferred_stub_guard has no row for
 * either and cannot have one -- it ratchets guessed bodies in src/, and these
 * are host copies in port/unmatched/ carrying provenance banners.
 */
void func_ov006_020e1dc8(char *self, int idx)
{
    port_mg_curling_collide_020e1dc8(self, idx);
}

void func_ov006_020e20bc(char *self, int idx)
{
    port_mg_curling_collide_020e20bc(self, idx);
}

/* ---- SEATED: GX::DisableAllBanks ------------------------------------------
 *
 * NOT A TRAP ANY MORE, and it was the missing airship. 0x0202e78c is
 * _ZN2GX15DisableAllBanksEv in config/arm9/symbols.txt: thirteen calls, one
 * per VRAM bank family, each already matched in src/ and already in this
 * link (func_02053ee0..func_02054018). The minigame framework's graphics init
 * (func_ov004_020b265c -> func_ov004_020b2980) calls it FIRST, so every bank
 * the 3D game had mapped is released before InitResources re-banks for 2D.
 * With this trapped as return-0, scene 368's engine A BG2 character load
 * (LoadFile 0x46, the Bob-omb Squad airship hull) landed in whatever banking
 * the 3D game left behind, the layer read as 8192/8192 transparent texels,
 * and the ship flew with its wings and no body. That census line was read
 * tonight as "there is nothing above the top screen to draw"; it was this.
 *
 * The call order is the ROM's, read off the disassembly at 0x0202e78c, not
 * alphabetical and not the symbol table's. */
extern "C" int func_02053f58(void); extern "C" int func_02054018(void);
extern "C" int func_02054004(void); extern "C" int func_02053f6c(void);
extern "C" int func_02053fa8(void); extern "C" int func_02053f94(void);
extern "C" int func_02053f80(void); extern "C" int func_02053fe0(void);
extern "C" int func_02053fbc(void); extern "C" int func_02053f44(void);
extern "C" int func_02053f30(void); extern "C" int func_02053f08(void);
extern "C" int func_02053ee0(void);

int func_0202e78c(void *)
{
    func_02053f58(); func_02054018(); func_02054004(); func_02053f6c();
    func_02053fa8(); func_02053f94(); func_02053f80(); func_02053fe0();
    func_02053fbc(); func_02053f44(); func_02053f30(); func_02053f08();
    func_02053ee0();
    return 0;
}

/* THE WALL WAS HERE AND IT IS GONE. Run mg5, lane BASESET.
 *
 * func_ov004_020b87e0 stood at this spot as a named trap that incremented
 * g_mg_trap_hits, printed one line and set no state. Section 4 of this file's
 * header records what it was and why; what follows is what replaced it.
 *
 * It is now a host copy in port/unmatched/MgBase_StateSetter.cpp: the src body
 * statement for statement, the twenty-entry member-pointer table re-typed as
 * {code, adjustment} int pairs read out of the mount's own bytes, and the
 * dispatch routed through port_mg_call0 like every other framework state. All
 * twenty of its addresses have matched src TUs and delink blocks, so not one
 * case in that switch is a floor.
 *
 * NO SYMBOL IS DEFINED HERE FOR IT ANY MORE. That is the point of removing the
 * trap rather than leaving it beside the host copy: two definitions of
 * func_ov004_020b87e0 in one build is a link error, and a trap kept "just in
 * case" behind an #if is a second opinion nobody reads.
 *
 * g_mg_trap_hits and port_mg_trap_hits() STAY. The counter is shared with
 * mg_trap() above and hal/scene_mg.cpp prints it as "unmatched ov004/ov006
 * traps entered"; the setter was one of the seven sites feeding it and is now
 * six. A census that used to read 2 on a scene 378 sweep because the setter was
 * called twice now reads 0, and that drop is the seat rather than a regression.
 */

}  /* extern "C" */
