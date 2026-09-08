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
/* run link100 lane FWD gate 2 */
#include "MansionSteps.h"
/* run link100 lane PMFB6 gate 2: the two member-defining EXTENT rows */
#include "Key.h"
#include "UpDownLiftBbh.h"

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



/* ov063 0x0211ce74 -- data_ov063_0211ef38, 4 cells. Run link100 lane FWD gate
   2: the tenth member-defining row, held out of gate 1 only because its
   source-pair seat is ov63_bringup in hal/actor_classes_ov063.cpp. The ROM
   strides it `add r3,r1,r0,lsl #3` at 0211ce8c on the pool word 0211cefc, the
   matched TU emits [eax*8]/[eax*8+4] with arity 0, its four sources are
   whole-pair {code,0} and its span is 32 bytes = exactly the four filled
   cells. */
int _ZN12MansionSteps8BehaviorEv(void *self)
{ return ((MansionSteps *)self)->MansionSteps::Behavior(); }

/* ---- run link100 LANE PMFB6, GATE 2: THE TWO MEMBER-DEFINING EXTENT ROWS --
   Lane FWD measured Key and UpDownLiftBbh clean on stride, pairs and dispatch
   shape and refused both for one reason only: each table is the LAST symbol in
   its overlay's config symbols.txt, so the next-symbol rule cannot bound the
   extent. delinks.txt bounds it instead -- a bss symbol cannot run past the end
   of .bss -- and both seats now cover the maximum span the section allows, with
   an aborting face in every cell past the filled run (ov089's two; ov095's one
   slack cell has no mounted storage and its file says so). The measurements are
   in runs/link100/out/PMFB6/rom_gate2.txt and emit_gate2_out.txt.

   Both matched TUs define a real C++ MEMBER -- ?Behavior@Key@@QAEHXZ and
   ?Behavior@UpDownLiftBbh@@QAEHXZ, read off their own objects -- while the
   port's actor-class face and vtable fill call the flat C name with the
   receiver on the stack. Same two symbols, same two conventions, same bridge.
   Both classes declare `int Behavior();` in the tree's own include/, so
   neither needs an invented declaration.

   ToxBox is the third row of that gate and is NOT here: its matched TU defines
   the FLAT extern "C" name already, so it needs no forwarder -- only the table
   alias below. */
int _ZN3Key8BehaviorEv(void *self)
{ return ((Key *)self)->Key::Behavior(); }

int _ZN13UpDownLiftBbh8BehaviorEv(void *self)
{ return ((UpDownLiftBbh *)self)->UpDownLiftBbh::Behavior(); }

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
/* gate 2. MansionSteps::Behavior's table, read off its object with dumpbin */
#pragma comment(linker, "/alternatename:?data_ov063_0211ef38@@3PAP8C@@AEXXZA=_data_ov063_0211ef38")
/* gate 2. func_ov064_02117d24's table, the same shape. That TU defines the
   FLAT C name and needs no forwarder -- only this alias. */
#pragma comment(linker, "/alternatename:?data_ov064_0211c750@@3PAUEntry@@A=_data_ov064_0211c750")
/* run link100 lane PMFB6 gate 2. Key's table: its TU declares
   `struct PmfEntry { PMF pmf; }; extern PmfEntry data_ov089_02132cec[];`
   OUTSIDE its own extern "C" block, so MSVC decorates the reference. Read off
   the TU's object, not guessed (runs/link100/out/PMFB6/undef_gate2.txt). */
#pragma comment(linker, "/alternatename:?data_ov089_02132cec@@3PAUPmfEntry@@A=_data_ov089_02132cec")
/* run link100 lane PMFB6 gate 2. ToxBox's table, the same shape, and this row
   needs no forwarder: its matched TU defines the flat extern "C" name. */
#pragma comment(linker, "/alternatename:?data_ov092_02132568@@3PAUTableEnt@@A=_data_ov092_02132568")
/* UpDownLiftBbh's table needs NO directive: that TU declares it INSIDE its own
   extern "C" block, so it comes in as the plain _data_ov095_02137910. */

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

/* ---- RUN link100 LANE PMFB7 GATE 1 ---------------------------------------
 * Nine of this gate's twelve object-field rows define a real C++ MEMBER
 * (?Behavior@<Class>@@QAEHXZ, read off each TU's own object) while the port's
 * actor class table calls the flat name the decomp's file is named after. The
 * bridge is the same one FWD wrote for the table rows: a forwarder in the flat
 * name's own convention that takes `this` as its argument and calls the member
 * through the real class type. Three of the twelve -- MantaRay, Shark and
 * Skeeter -- define the flat extern "C" name themselves and need nothing here.
 * Every one of the nine declares `int Behavior();` in the tree's own
 * include/<Class>.h, so all nine decorate ...@@QAEHXZ, and that was checked
 * object against object in runs/link100/out/PMFB7/undef_gate1.txt.
 */
#include "Bullet.h"
#include "WaterSuction.h"
#include "Submarine.h"
#include "BowserPuzzlePiece.h"
#include "LavaBubble.h"
#include "WaterRing.h"
#include "Snufit.h"
#include "Swoop.h"
#include "CheepCheep.h"

extern "C" {
/* ov002 0x020fee44 -- descriptor data_ov002_02111190, main half at +8 */
int _ZN6Bullet8BehaviorEv(void *self)
{ return ((Bullet *)self)->Bullet::Behavior(); }
/* ov026 0x0211233c -- cell data_ov036_02113f58, tick half at +8 */
int _ZN12WaterSuction8BehaviorEv(void *self)
{ return ((WaterSuction *)self)->WaterSuction::Behavior(); }
/* ov026 0x0211200c -- cells data_ov026_02113f2c / _02113f3c */
int _ZN9Submarine8BehaviorEv(void *self)
{ return ((Submarine *)self)->Submarine::Behavior(); }
/* ov064 0x021198bc -- cell data_ov064_0211c934, .b half at +8 */
int _ZN17BowserPuzzlePiece8BehaviorEv(void *self)
{ return ((BowserPuzzlePiece *)self)->BowserPuzzlePiece::Behavior(); }
/* ov064 0x02118850 -- cells data_ov064_0211c7b8 / _0211c7c8 */
int _ZN10LavaBubble8BehaviorEv(void *self)
{ return ((LavaBubble *)self)->LavaBubble::Behavior(); }
/* ov064 0x02119ffc -- cells data_ov064_0211c954 / _0211c944 */
int _ZN9WaterRing8BehaviorEv(void *self)
{ return ((WaterRing *)self)->WaterRing::Behavior(); }
/* ov065 0x02116b84 -- cells data_ov065_0211d650/60/70/80 */
int _ZN6Snufit8BehaviorEv(void *self)
{ return ((Snufit *)self)->Snufit::Behavior(); }
/* ov065 0x02117b64 -- cells data_ov065_0211d6e0/f0/700/710 */
int _ZN5Swoop8BehaviorEv(void *self)
{ return ((Swoop *)self)->Swoop::Behavior(); }
/* ov090 0x02133430 -- cells data_ov090_02134584 / _02134594 */
int _ZN10CheepCheep8BehaviorEv(void *self)
{ return ((CheepCheep *)self)->CheepCheep::Behavior(); }
}  /* extern "C" */

/* ---- RUN link100 LANE PMFB7 GATE 2 ---------------------------------------
 * Six of gate 2's nine rows define a real C++ MEMBER (?Behavior@<Class>@@QAEHXZ,
 * read off each TU's own object) while the port's actor class table calls the
 * flat name; Chuckya, Eyerok and ChiefChilly define the flat extern "C" name
 * themselves and need nothing here. Every one of the six declares
 * `int Behavior();` in the tree's own include/<Class>.h, so all six decorate
 * ...@@QAEHXZ, checked object against object in
 * runs/link100/out/PMFB7/undef_gate2.txt.
 */
#include "HeaveHo.h"
#include "KingBobOmb.h"
#include "LakituBro.h"
#include "Stump.h"
#include "UnchainedChomp.h"

extern "C" {
/* ov077 0x02126e88 -- five two-record cells data_ov077_02127cd8..d18 */
int _ZN7HeaveHo8BehaviorEv(void *self)
{ return ((HeaveHo *)self)->HeaveHo::Behavior(); }
/* ov078 0x02126104 -- eighteen two-record cells from __sinit_ov078_02126660 */
int _ZN10KingBobOmb8BehaviorEv(void *self)
{ return ((KingBobOmb *)self)->KingBobOmb::Behavior(); }
/* ov085 0x0212eb18 -- eleven two-record cells from __sinit_ov085_0212fe4c */
int _ZN9LakituBro8BehaviorEv(void *self)
{ return ((LakituBro *)self)->LakituBro::Behavior(); }
/* ov091 0x021341ec -- the three FWOOSH cells 021356b0/c0/d0 */
int _ZN5Stump8BehaviorEv(void *self)
{ return ((Stump *)self)->Stump::Behavior(); }
/* ov100 0x02143d64 -- the cell data_ov100_021486f4, hi half at +8 */
int _ZN14UnchainedChomp8BehaviorEv(void *self)
{ return ((UnchainedChomp *)self)->UnchainedChomp::Behavior(); }
}  /* extern "C" */

/* run link100 lane PMFB7 gate 2. LakituBro's matched TU declares the two ov085
   State cells its Behavior compares a POINTER against -- `extern State
   data_ov085_021307d0;` at line 15 and _021307e0 at line 16 -- OUTSIDE its own
   extern "C" block, so MSVC decorates both references. The mount defines the
   plain names and walk_window.map already carries them; read off the TU's own
   object with dumpbin before the link, not guessed
   (runs/link100/out/PMFB7/undef_gate2.txt). The comparison itself is on the
   CELL ADDRESS and not on a pair's contents, so this gate does not disturb it. */
#pragma comment(linker, "/alternatename:?data_ov085_021307d0@@3UState@@A=_data_ov085_021307d0")
#pragma comment(linker, "/alternatename:?data_ov085_021307e0@@3UState@@A=_data_ov085_021307e0")
