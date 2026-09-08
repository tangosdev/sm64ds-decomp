/* RUN link100, LANE FWD -- ONE FORWARDER PER CLASS FOR THE MEMBER-DEFINING
 * Behavior ROWS.
 *
 * THE PROBLEM, in the words the link used when lane PMFB5 hit it:
 *
 *   actor_classes_ov060.cpp.obj : error LNK2019: unresolved external symbol
 *   __ZN10BowserFire8BehaviorEv referenced in function
 *   "int __fastcall bfire_behavior(void *,void *)"
 *
 * Nine matched TUs define a real C++ MEMBER -- `int BowserFire::Behavior()`,
 * which MSVC decorates ?Behavior@BowserFire@@QAEHXZ and calls __thiscall --
 * while the port's actor-class face calls the FLAT C name the decomp's file
 * is named after, with the receiver on the STACK. Two different symbols and
 * two different conventions, so an /alternatename cannot bridge them: it
 * would leave the member reading a receiver out of ecx that the caller never
 * set. What bridges them is a forwarder in the flat name's own convention
 * that takes `this` as its argument and calls the member through the real
 * class type, which is exactly the shape hal/cxxname_bridge.cpp's gate-9
 * method bridges already use for Platform and ShadowModel.
 *
 * WHAT THE FORWARDER IS NOT. It adds no call the ROM does not make: the ROM
 * calls each class's Behavior through the actor class table's slot 6, the
 * port's face is that slot, and the body it now reaches is the decomp's own
 * matched TU instead of a transcribed host copy. Nothing here changes a ROM
 * reference or a linker option.
 *
 * THE OTHER HALF OF EACH ROW is in the seat file that owns the class's
 * table: with the host copy retired, the matched TU dispatches the table
 * itself, and a matched TU dispatches a pointer to member as
 *     mov ecx, TAB[i*8+4] / mov eax, TAB[i*8] / add ecx, this / call eax
 * -- receiver in ecx, NOTHING pushed -- where the host copy called the code
 * word cdecl with an explicit self. So every cell of every table below now
 * holds a zero-argument __fastcall FACE rather than a plain cdecl body. The
 * per-row ROM stride, source pairs, span and emitted shape are in
 * port/slice_fwd.txt and runs/link100/out/FWD/.
 *
 * MANGLING. MSVC's decoration of a member function carries the method name,
 * the class name, the ACCESS specifier, the calling convention and the
 * signature -- and nothing about the class's members or its size. All nine
 * are a `struct` (public) declaring `int Behavior();`, so all nine decorate
 * ?Behavior@<Class>@@QAEHXZ, and every one was read off the matched TU's own
 * object with dumpbin /symbols before this file was written
 * (runs/link100/out/FWD/emit_gate1_out.txt).
 */

/* seven of the nine classes are declared in the tree's own include/ */
#include "KnockDownPlank.h"
#include "BowserFire.h"
#include "Whomp.h"
#include "MontyMole.h"
#include "RotatingUpDownPlatform.h"
#include "Cannon.h"
#include "Fish.h"

/* include/BowserPuzzleManager.h and include/Dorrie.h do NOT declare
   Behavior -- their matched TUs declare the class themselves -- so these two
   carry the matched TU's own declaration verbatim rather than an invented
   one. src/_ZN19BowserPuzzleManager8BehaviorEv.cpp declares
   `struct BowserPuzzleManager { unsigned char pad[0x336]; unsigned char idx;
   int Behavior(); };` and src/_ZN6Dorrie8BehaviorEv.cpp declares
   `class Dorrie { public: int Behavior(); };`; only the access and the
   signature reach the decoration, and both are public int(void). */
struct BowserPuzzleManager { int Behavior(); };
struct Dorrie { int Behavior(); };

extern "C" {

/* ov015 0x02112090 -- ov015 data_ov015_021149ec, 7 cells */
int _ZN14KnockDownPlank8BehaviorEv(void *self)
{ return ((KnockDownPlank *)self)->KnockDownPlank::Behavior(); }

/* ov060 0x021176d4 -- ov060 data_ov060_0211afb4, 8 cells */
int _ZN10BowserFire8BehaviorEv(void *self)
{ return ((BowserFire *)self)->BowserFire::Behavior(); }

/* ov064 0x021190b0 -- ov064 data_ov064_0211c904, 6 cells */
int _ZN19BowserPuzzleManager8BehaviorEv(void *self)
{ return ((BowserPuzzleManager *)self)->BowserPuzzleManager::Behavior(); }

/* ov065 0x02118df0 -- ov065 data_ov065_0211d7fc, 3 cells */
int _ZN6Dorrie8BehaviorEv(void *self)
{ return ((Dorrie *)self)->Dorrie::Behavior(); }

/* ov079 0x02125fcc -- ov079 data_ov079_02128280, 12 cells */
int _ZN5Whomp8BehaviorEv(void *self)
{ return ((Whomp *)self)->Whomp::Behavior(); }

/* ov080 0x02124530 -- ov080 data_ov080_02128438, 6 cells */
int _ZN9MontyMole8BehaviorEv(void *self)
{ return ((MontyMole *)self)->MontyMole::Behavior(); }

/* ov091 0x02132108 -- ov091 data_ov091_021354e0, 3 cells */
int _ZN22RotatingUpDownPlatform8BehaviorEv(void *self)
{ return ((RotatingUpDownPlatform *)self)->RotatingUpDownPlatform::Behavior(); }

/* ov098 0x0213b26c -- ov098 data_ov098_0213c8fc, 4 cells */
int _ZN6Cannon8BehaviorEv(void *self)
{ return ((Cannon *)self)->Cannon::Behavior(); }

/* ov100 0x02146b38 -- ov100 data_ov100_02148a1c, 7 cells (TWO dispatch sites, one table) */
int _ZN4Fish8BehaviorEv(void *self)
{ return ((Fish *)self)->Fish::Behavior(); }

}  /* extern "C" */

/* ---- THE TABLE ALIASES ---------------------------------------------------
 * Four of the nine matched TUs declare their table OUTSIDE their own
 * extern "C" block, so MSVC decorates the reference and it does not resolve
 * against the mounted C-linkage symbol. Each spelling below was read off the
 * TU's own object with dumpbin /symbols, not guessed
 * (runs/link100/out/FWD/undef_gate1.txt). The other five TUs' tables come in
 * as the plain _data_ name and need nothing.
 *
 * ?data_ov098_0213c8fc@@3PAUEntry@@A is NOT here: Cannon's spelling is
 * already carried by hal/cxx_aliases.cpp:1686 and a second directive for the
 * same LHS would be a duplicate, not a fix.
 */
/* KnockDownPlank's TU declares the table outside its extern "C" block */
#pragma comment(linker, "/alternatename:?data_ov015_021149ec@@3PAUTabEnt@@A=_data_ov015_021149ec")
/* BowserPuzzleManager's, the same shape */
#pragma comment(linker, "/alternatename:?data_ov064_0211c904@@3PAUEntry@@A=_data_ov064_0211c904")
/* Whomp's, an array of the bare pointer to member */
#pragma comment(linker, "/alternatename:?data_ov079_02128280@@3PAP8Actor@@AEHXZA=_data_ov079_02128280")
/* MontyMole's */
#pragma comment(linker, "/alternatename:?data_ov080_02128438@@3PAUEntry@@A=_data_ov080_02128438")
/* Whomp's TU also declares the arm9 global data_0209f318 as a plain int at C++ linkage; hal/cxx_aliases.cpp:929/932/947 already carry three OTHER spellings of the same storage (@@3PAUCamera@@A, @@3PAHA, @@3PAXA), so this is the fourth view of one address, not a new one */
#pragma comment(linker, "/alternatename:?data_0209f318@@3HA=_data_0209f318")

/* ---- THE TWO PLACEHOLDER NAMES -------------------------------------------
 * src/_ZN22RotatingUpDownPlatform8BehaviorEv.cpp calls two names that
 * include/decl_common.h declares and NOTHING in this link defines. They are
 * not undecompiled code and not a stand-in: the ROM's own relocations at
 * those two call sites name symbols this link already carries, and
 * port/unmatched/Ov091_HostSites.cpp's host copy -- the one this row
 * retires -- spelled both of them in full for exactly that reason. The
 * directives point the decomp's placeholder spelling at the ROM's own
 * target.
 */
/* include/decl_common.h:1896 declares ApproachLinearI and nothing defines it. RotatingUpDownPlatform::Behavior's call site at 0x021321a8 relocates to 0x0203ae58, which config/arm9/symbols.txt names ApproachLinear(int&, int, int) -- the RHS below. The two sibling state bodies func_ov091_02131ef0 and _02131f9c already spell that symbol in full and this link defines it */
#pragma comment(linker, "/alternatename:_ApproachLinearI=__Z14ApproachLinearRiii")
/* include/decl_common.h:1967, the same shape: the TU's own literal pool at 0x02132208 reads 0x0203923c, which config/arm9/symbols.txt names MeshColliderBase::UpdatePosWithVelocity */
#pragma comment(linker, "/alternatename:_UpdatePosWithVelocitySym=__ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_")
