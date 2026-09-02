/* ov029 method faces: the four water_city classes whose lifecycle bodies are
 * matched as REAL C++ METHODS (.cpp), not as Itanium C-named functions. The
 * matched src emits the MSVC decoration (?InitResources@ArrowLift@@QAEHXZ ...),
 * while hal/actor_classes_ov029.cpp's vtable fill -- and the ROM's own name --
 * spell the Itanium C symbol _ZN9ArrowLift13InitResourcesEv. This TU bridges the
 * two: each face forwards the C name into the matched method. It does not
 * reimplement anything.
 *
 * The shadow structs carry only the method declarations, no fields: MSVC mangles
 * a non-virtual member by class name + method name + signature + calling
 * convention, none of which depends on layout, so the forwarded call resolves to
 * the exact symbol the matched .cpp defines. No project headers on purpose (the
 * precedent is port/unmatched/Ov072_MethodFaces.cpp), so nothing else about
 * these classes can perturb a spelling. Only the .cpp-matched methods get a face;
 * the .c-matched ones (ArrowLift::CleanupResources, every D0, class-32's whole
 * set) are already C-linkage and are called directly by the fill.
 */
#include "hal/dsstate_seg.h"

struct FloatOnWaterPlatformWdwSquare { int InitResources(); int CleanupResources(); int Behavior(); int Render(); };
struct ArrowLift { int InitResources(); int Behavior(); int Render(); };
struct SwitchActivatedPlank { int InitResources(); int CleanupResources(); int Render(); };
struct RotatingPlatformWdw { int InitResources(); int CleanupResources(); int Behavior(); int Render(); };
/* Square and Rectangle InitResources call Actor::GetWaterHeightWDW, matched as a
   C++ method (src/_ZN5Actor17GetWaterHeightWDWEv.c, //cpp) -- another face. */
struct Actor { int GetWaterHeightWDW(); };

/* The InitResources C++ methods reference their SharedFilePtr/CLPS_Block cells
 * with the struct TYPE at file scope, so MSVC decorates each. The per-symbol
 * mount emits one C-named array per cell; data has no calling convention, so the
 * alias onto the one object is exact (the ov025 SharedFilePtr/CLPS recipe). */
#pragma comment(linker, "/alternatename:?data_ov029_02114250@@3USharedFilePtr@@A=_data_ov029_02114250")
#pragma comment(linker, "/alternatename:?data_ov029_02114248@@3USharedFilePtr@@A=_data_ov029_02114248")
#pragma comment(linker, "/alternatename:?data_ov029_0211302c@@3UCLPS_Block@@A=_data_ov029_0211302c")
#pragma comment(linker, "/alternatename:?data_ov029_0211432c@@3USharedFilePtr@@A=_data_ov029_0211432c")
#pragma comment(linker, "/alternatename:?data_ov029_02114324@@3USharedFilePtr@@A=_data_ov029_02114324")
#pragma comment(linker, "/alternatename:?data_ov029_0211304c@@3UCLPS_Block@@A=_data_ov029_0211304c")

extern "C" {

/* id 96 ArrowLift's live table is _ZTV29FloatOnWaterPlatformWdwSquare */
int _ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv(void *s)
{ return ((FloatOnWaterPlatformWdwSquare *)s)->InitResources(); }
int _ZN29FloatOnWaterPlatformWdwSquare16CleanupResourcesEv(void *s)
{ return ((FloatOnWaterPlatformWdwSquare *)s)->CleanupResources(); }
int _ZN29FloatOnWaterPlatformWdwSquare8BehaviorEv(void *s)
{ return ((FloatOnWaterPlatformWdwSquare *)s)->Behavior(); }
int _ZN29FloatOnWaterPlatformWdwSquare6RenderEv(void *s)
{ return ((FloatOnWaterPlatformWdwSquare *)s)->Render(); }

/* id 97 WaterDiamond's live table is _ZTV9ArrowLift (Actor 31) */
int _ZN9ArrowLift13InitResourcesEv(void *s)
{ return ((ArrowLift *)s)->InitResources(); }
int _ZN9ArrowLift8BehaviorEv(void *s)
{ return ((ArrowLift *)s)->Behavior(); }
int _ZN9ArrowLift6RenderEv(void *s)
{ return ((ArrowLift *)s)->Render(); }
/* ArrowLift's D1 (_ZN9ArrowLiftD1Ev.cpp) is a full C++ VIRTUAL destructor that
   drags in Actor::~Actor / MovingCylinderClsn::~ / Model::~ (none host-defined),
   so that TU is excluded from the slice and WaterDiamond's slot 16 is trapped
   instead of faced. */

/* id 98 SwitchActivatedPlank's own table */
int _ZN20SwitchActivatedPlank13InitResourcesEv(void *s)
{ return ((SwitchActivatedPlank *)s)->InitResources(); }
int _ZN20SwitchActivatedPlank16CleanupResourcesEv(void *s)
{ return ((SwitchActivatedPlank *)s)->CleanupResources(); }
/* SwitchActivatedPlank::Behavior is matched as a C-linkage _ZN20...8BehaviorEv
   (its .cpp defines the C name directly), so it needs no face and the fill calls
   it directly. */
int _ZN20SwitchActivatedPlank6RenderEv(void *s)
{ return ((SwitchActivatedPlank *)s)->Render(); }

/* id 101 WDW_Water's live table is _ZTV19RotatingPlatformWdw */
int _ZN19RotatingPlatformWdw13InitResourcesEv(void *s)
{ return ((RotatingPlatformWdw *)s)->InitResources(); }
int _ZN19RotatingPlatformWdw16CleanupResourcesEv(void *s)
{ return ((RotatingPlatformWdw *)s)->CleanupResources(); }
int _ZN19RotatingPlatformWdw8BehaviorEv(void *s)
{ return ((RotatingPlatformWdw *)s)->Behavior(); }
int _ZN19RotatingPlatformWdw6RenderEv(void *s)
{ return ((RotatingPlatformWdw *)s)->Render(); }

/* the Actor water-height face + the 3-entry WDW height table it indexes. The
   table is arm9 0x02075244, three non-relocated Fix12 heights read verbatim from
   extracted/arm9_dec.bin (0002d000, 003e8000, 00abe000); it is unmounted arm9
   data nothing else in the port had reached, so this TU carries the ROM bytes.
   It sits at a DS address, so it rides .dsstate like every hosted DS global
   (constant, so a restore rewrites the same three words). */
DSSTATE_BEGIN
int data_02075244[3] = { 0x0002d000, 0x003e8000, 0x00abe000 };
DSSTATE_END
int _ZN5Actor17GetWaterHeightWDWEv(void *s)
{ return ((Actor *)s)->GetWaterHeightWDW(); }

}
