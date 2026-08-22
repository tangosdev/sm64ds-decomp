// PORT_HOST_ABI.  dScMg3DEsp_c's twenty-three link faces: eighteen ordinary
// /alternatename rows and five shadow-class destructors.  Run mg9, lane PSY.
//
// Everything here was named by a link on the wired tree and nothing was
// invented ahead of one, which is the rule port/mg_fanout_costs.txt section 10
// states for face work: a face with no LNK2019 behind it is a guess with a
// symbol name on it.
//
// ---- 1. TWO C-NAMED SYMBOLS DECLARED AT C++ LINKAGE ----------------------
//
// The ordinary case hal/scene_boot.cpp section 1 already carries twenty-three
// of.  Two of this class's src TUs declare a mounted DS data symbol OUTSIDE
// their extern "C" block, so MSVC mangles it and the plain C name the ov006
// mount defines cannot satisfy the reference:
//
//   src/func_ov006_020e9e70.cpp  extern volatile M48 data_ov006_0213c88c;
//       -> ?data_ov006_0213c88c@@3UM48@@C
//   src/func_ov006_020e7660.cpp  extern void *data_ov006_0213c8c4;
//       -> ?data_ov006_0213c8c4@@3PAXA
//
// Both are DATA, so there is no calling convention to disagree about and an
// alias is correct rather than merely convenient -- the distinction section 4
// draws when it says "AN ALIAS CANNOT CHANGE A CALLING CONVENTION".
//
// data_ov006_0213c88c IS THE CLASS'S OWN CAMERA MATRIX BLOCK, twelve words at
// 0x0213c88c that InitResources copies, and data_ov006_0213c8c4 IS THE VTABLE
// ITSELF -- the D2 destructor stores it into the object's first word, which is
// the same store slot 17 makes and the same address the registry row seats.
// So the second alias binds the one symbol this whole lane is about, and a
// wrong binding there would be loud rather than subtle.
//
// ---- 2. THREE SHADOW-CLASS DESTRUCTORS, AND WHY THEY ARE FACES AND NOT ----
//         ALIASES
//
// src/func_ov006_020e7660.cpp (slot 16, D2) unwinds the object through LOCAL
// shadow classes:
//
//     struct TextureTransformer { ~TextureTransformer(); };
//     struct Model { ~Model(); };
//     namespace Particle { struct SysTracker { ~SysTracker(); }; }
//
// so MSVC emits three references to __thiscall destructors that nothing in the
// tree defines:
//
//     ??1TextureTransformer@@QAE@XZ
//     ??1Model@@QAE@XZ
//     ??1SysTracker@Particle@@QAE@XZ
//
// The port DOES have all three bodies, under their Itanium C names, and
// config/arm9/symbols.txt gives the addresses the ROM calls:
//
//     0x0201592c  _ZN18TextureTransformerD1Ev      size 0x24
//     0x02016d20  _ZN5ModelD1Ev                    size 0x38
//     0x02023194  _ZN8Particle10SysTrackerD1Ev     size 0x70
//
// AN ALIAS WOULD BE WRONG.  The MSVC names are __thiscall (the Q..AE in the
// mangle says so) and the three C bodies are __cdecl, so the receiver arrives
// in ecx on one side and on the stack on the other.  Each therefore gets a
// real face: a definition of the destructor MSVC is asking for, whose body
// forwards `this` as an ordinary first argument.
//
// THE ROM CALLS ALL THREE NON-VIRTUALLY, which is what makes the non-virtual
// (Q) mangle the right one to define.  Read off the D0 sibling at 0x020e76e4,
// which is the same body plus a Memory::Deallocate:
//
//     020e76fc  bl 0x0201592c     TextureTransformer at this+0x51f4
//     020e7714  bl 0x02016d20     Model at this+0x4f88
//     020e7720  bl 0x02016d20     Model at this+0x4f38
//     020e7734  bl 0x02023194     Particle::SysTracker at this+0x471c
//
// four plain `bl`s and not one vtable load, so there is no override for a
// qualified call to devirtualise away.  That is the opposite of the ruling
// unmatched/MgFlower_ModelRender_020c3bf4.cpp makes for the RENDER site, where
// the ROM does load a vptr and the dispatch has to stay virtual, and the two
// rulings are compatible for the reason that file gives: the test is what the
// ROM emits at the site, not what the class is.
//
// THE THREE FACES ARE THIS TU'S, NOT THE FAMILY'S.  If another lane's class
// ever spells the same shadow destructors, the definitions belong in a shared
// file rather than duplicated here -- a second definition of
// ??1Model@@QAE@XZ is an LNK2005, which is a loud failure and the right one.

// ---- 3. WAVE THREE ADDS TWO MORE DESTRUCTORS AND TWELVE MORE ALIASES -----
//
// src/func_ov006_020e80d8.cpp is the sub-object's own teardown and spells two
// further shadow classes, TextureSequence and ModelAnim.  Same ruling, same
// evidence, read off the ROM at 0x020e80d8:
//
//     020e811c  bl 0x02015a2c     _ZN15TextureSequenceD1Ev, at this+0x70
//     020e8124  bl 0x0201691c     _ZN9ModelAnimD1Ev,        at this+0xc
//
// two plain `bl`s, no vptr load, and both land on the D1 rather than the D2 or
// the D0 -- which is what says the face forwards to D1 and not to either
// sibling.  MODELANIM IS WORTH A SECOND LOOK RATHER THAN A COPY OF THE MODEL
// ROW: hal/cxxname_bridge.cpp builds a real host vtable for it and warns that
// ModelAnim2's ROM Render slot is its MSVC Virtual18 slot, so a shadow-class
// RENDER dispatch on that class cannot be served by the dual fill.  This is not
// that: it is a destructor, the ROM calls it directly, and nothing here goes
// through a table.
//
// THE TWELVE ALIASES ARE SIX ADDRESSES SPELLED TWICE.  The same six ov006
// SharedFilePtrs -- the ones __sinit_ov006_02130a08 constructs -- are declared
// `extern void *` by src/func_ov006_020e7fe8.cpp and `extern SharedFilePtr` by
// src/func_ov006_020e80d8.cpp, so MSVC emits two different mangles per address
// and both need a row onto the one C name the mount defines.  That is a fact
// about two src TUs disagreeing on a type, not about the port.
//
// AND ONE ALIAS WAS READ BEFORE IT WAS WRITTEN, because its spelling is the one
// port/mg_fanout_costs.txt section 10 finding 1 says slips facegen's guards.
// ?data_ov006_02133f24@@3PAPAUEnt@@A is `struct Ent **`, the PAU family, and
// the rule there is that an array-of-struct spelling can hide a member-pointer
// table.  It does not here: src/func_ov006_020e8e10.cpp declares
// `struct Ent { u16 f0, f2, f4, f6; }` and `Ent *data_ov006_02133f24[]`, walks
// the pointed-at record until f6 == 0xffff and hands it to
// func_ov004_020af770 -- a sprite layout list, not a pair run.  The array holds
// POINTERS, four bytes on both machines, and ov006's relocs.txt has four
// relocations inside it (from:0x02133f24 to:0x02133f80 and three more), so the
// ov006 mount rebases the interior pointers and an ordinary alias is correct.

extern "C" {
void _ZN18TextureTransformerD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN8Particle10SysTrackerD1Ev(void *self);
void _ZN15TextureSequenceD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
}

/* The three shadow classes, declared exactly as src/func_ov006_020e7660.cpp
   declares them so the mangles match byte for byte, and defined here. */
struct TextureTransformer { ~TextureTransformer(); };
struct Model { ~Model(); };
namespace Particle { struct SysTracker { ~SysTracker(); }; }
struct TextureSequence { ~TextureSequence(); };
struct ModelAnim { ~ModelAnim(); };

TextureTransformer::~TextureTransformer()
{ _ZN18TextureTransformerD1Ev(this); }

Model::~Model()
{ _ZN5ModelD1Ev(this); }

Particle::SysTracker::~SysTracker()
{ _ZN8Particle10SysTrackerD1Ev(this); }

TextureSequence::~TextureSequence()
{ _ZN15TextureSequenceD1Ev(this); }

ModelAnim::~ModelAnim()
{ _ZN9ModelAnimD1Ev(this); }

/* ---- the two ordinary alias rows ---------------------------------------- */

/* ?data_ov006_0213c88c@@3UM48@@C  <- the ov006 mount's _data_ov006_0213c88c.
   src/func_ov006_020e9e70.cpp declares it `extern volatile M48` at C++ linkage;
   the mount defines the plain C name. */
#pragma comment(linker, "/alternatename:?data_ov006_0213c88c@@3UM48@@C=_data_ov006_0213c88c")

/* ?data_ov006_0213c8c4@@3PAXA  <- the class's own vtable, declared
   `extern void *` at C++ linkage by src/func_ov006_020e7660.cpp. */
#pragma comment(linker, "/alternatename:?data_ov006_0213c8c4@@3PAXA=_data_ov006_0213c8c4")

/* wave 3: six ov006 SharedFilePtrs, each spelled twice by two different src
   TUs, plus the sprite-layout pointer table and the render half's idle
   sentinel. Every row maps a mangled C++-linkage name onto the plain C name
   the ov006 mount already defines at that address. */
#pragma comment(linker, "/alternatename:?data_ov006_02141e54@@3PAXA=_data_ov006_02141e54")
#pragma comment(linker, "/alternatename:?data_ov006_02141e5c@@3PAXA=_data_ov006_02141e5c")
#pragma comment(linker, "/alternatename:?data_ov006_02141e64@@3PAXA=_data_ov006_02141e64")
#pragma comment(linker, "/alternatename:?data_ov006_02141e7c@@3PAXA=_data_ov006_02141e7c")
#pragma comment(linker, "/alternatename:?data_ov006_02141e84@@3PAXA=_data_ov006_02141e84")
#pragma comment(linker, "/alternatename:?data_ov006_02141e8c@@3PAXA=_data_ov006_02141e8c")
#pragma comment(linker, "/alternatename:?data_ov006_02141e54@@3USharedFilePtr@@A=_data_ov006_02141e54")
#pragma comment(linker, "/alternatename:?data_ov006_02141e5c@@3USharedFilePtr@@A=_data_ov006_02141e5c")
#pragma comment(linker, "/alternatename:?data_ov006_02141e64@@3USharedFilePtr@@A=_data_ov006_02141e64")
#pragma comment(linker, "/alternatename:?data_ov006_02141e7c@@3USharedFilePtr@@A=_data_ov006_02141e7c")
#pragma comment(linker, "/alternatename:?data_ov006_02141e84@@3USharedFilePtr@@A=_data_ov006_02141e84")
#pragma comment(linker, "/alternatename:?data_ov006_02141e8c@@3USharedFilePtr@@A=_data_ov006_02141e8c")
/* the sprite-layout pointer table, read before it was aliased -- section 3 */
#pragma comment(linker, "/alternatename:?data_ov006_02133f24@@3PAPAUEnt@@A=_data_ov006_02133f24")
/* the RENDER half's idle sentinel, the pair func_ov006_020e7b44 compares the
   field at +0x210 against. It is compared and never dispatched, so an alias
   onto the mounted words is right and the pair keeps the ROM's own values --
   which is exactly what makes that comparison keep answering what the ROM
   answers. */
#pragma comment(linker, "/alternatename:?data_ov006_0213c744@@3PAHA=_data_ov006_0213c744")

/* ---- 4. WAVE FOUR IS TWO ROWS AND BOTH COME FROM ONE TU ------------------

   src/func_ov006_020e7f5c.cpp is the fifth writer of the field at the
   sub-object's +0x210 and it is the one that spells the eight-byte pair as a
   `double`:

       extern double data_ov006_0213c76c;
       *(double*)((char*)o + 0x210) = data_ov006_0213c76c;

   which is a correct eight-byte move on both machines and is why that writer
   needs no host copy -- only an alias for the mangle.  The second row is a
   THIRD spelling of an address two earlier TUs already declared: 0x02141e7c is
   `extern void *` in src/func_ov006_020e7fe8.cpp, `extern SharedFilePtr` in
   src/func_ov006_020e80d8.cpp and `extern int []` here, so it carries three
   mangles onto one C name.  Three src TUs disagreeing about a type is a fact
   about src, and the alias rows are where the port absorbs it. */
#pragma comment(linker, "/alternatename:?data_ov006_02141e7c@@3PAHA=_data_ov006_02141e7c")
#pragma comment(linker, "/alternatename:?data_ov006_0213c76c@@3NA=_data_ov006_0213c76c")
