/* PORT_HOST_ABI. SHARK (225, 9daShark_c), ov090. Run rel0215 wave 2, lane
 * cast-ov090. Three sites; see unmatched/Skeeter_HostSites.cpp for the full
 * derivation of the ModelAnim and PMF shapes and
 * unmatched/MantaRay_HostSites.cpp for the PathPtr one.
 *
 * (1) src/_ZN5Shark6RenderEv.cpp -- the ModelAnim slot-5 collision (T1).
 *     Unconditional, null scale, the same body as MantaRay's.
 * (2) src/func_ov090_021338b4.cpp -- the state setter, record 0 (ENTER).
 * (3) src/_ZN5Shark8BehaviorEv.cpp -- the record-1 (TICK) pointer-to-member
 *     call plus the three PathPtr __thiscall member calls and the
 *     ApproachLinear overload. Shark's path fields are at 0x388 / 0x390 /
 *     0x38c where MantaRay's are 0x37c / 0x384 / 0x380, its horizontal
 *     approach step is 0x180 where MantaRay's is 0x60 and its forward speed is
 *     0x14000 where MantaRay's is 0xa000 -- read off Shark's own source, not
 *     copied across from the sibling.
 */
#include "ModelAnim.h"

extern "C" {

/* _ZN5Shark6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* ---- (2) and (3) ------------------------------------------------------- */
struct PortOv090Pmf { unsigned int fn; int delta; };
typedef int (*PortOv090StateFn)(void *);

/* func_ov090_021338b4 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov090_021338b4.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
struct ShVec3 { int x, y, z; };

unsigned short DecIfAbove0_Short(unsigned short *p);
void Vec3_Sub(ShVec3 *out, ShVec3 *a, ShVec3 *b);
int  LenVec3(ShVec3 *v);
short Vec3_HorzAngle(ShVec3 *v0, ShVec3 *v1);
short Vec3_VertAngle(ShVec3 *v0, ShVec3 *v1);
void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short angle);
void MulVec3Mat4x3(ShVec3 *v, void *m, ShVec3 *out);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *thiz, void *clsn);
void func_ov090_02133904(void *c);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);
char *_ZN5Actor13ClosestPlayerEv(void *c);
void _Z14ApproachLinearRsss(short *v, short target, short step);
void _ZN7PathPtrC1Ev(void *self);
void _ZN7PathPtr6FromIDEj(void *self, unsigned id);
void _ZNK7PathPtr7GetNodeER7Vector3j(const void *self, ShVec3 *out, unsigned idx);
extern char data_020a0e68[];

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN5Shark8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}  /* extern "C" */
