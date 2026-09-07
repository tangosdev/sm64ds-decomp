//cpp
/* A chain-chomp shutter -- the barred gate the chomp is tethered in front of.
 * ov014/daObjWanwanShutter_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x02112e0c..0x021130ac, eight
 * functions and nothing else, and the production build links this object rather
 * than eight per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daObjWanwanShutter_c') is d_a_obj_wanwan_shutter.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov014 0x02114868 is a
 * __si_class_type_info whose _ZTS at 0x02114874 reads exactly
 * `20daObjWanwanShutter_c`, and whose one base, at subobject offset 0, is
 * ov002 0x021089ec -- _ZTI10dBgActor_c. That is why the header says
 * `struct daObjWanwanShutter_c : dBgActor_c` and why the RTTI below has ROM
 * homes to be compared against at all: a coined name is a length-prefixed
 * mangled string that matches nothing at any address, so it can never be
 * word-compared. The historical spelling was ChainChompFence.
 *
 * IT DERIVES FROM dBgActor_c, not dActor_c. (Seven other promoted entries do
 * too -- daObjCannonShutter_c, daObjHatenaSwitch_c, daObjPushblock_c,
 * daObjC1_Trap_c, daObjC0_Switch_c, daObjFm_Battan_c, daObjCtMecha05_c -- so
 * this is the campaign's normal shape, not a first.) Two consequences show up
 * below. The destructor is 0x44 bytes for
 * an empty body because it inlines dBgActor_c's own inline destructor, which
 * destroys the Model at 0xd4 and the dBgW_KcMbg at 0x124. And the base chain's
 * VTABLES are absent from this object even though its typeinfo records are
 * present, because dBgActor_c's key function is Kill() and that lives in
 * another TU.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * THE SIZE ARITHMETIC CLOSES. The factory's literal is 800 = 0x320, which is
 * exactly sizeof(dBgActor_c); this class adds only mDisabled at 0x31e, inside
 * the tail padding the base already had, so the derived size is the base's.
 *
 * THE VTABLE IS 32 SLOTS and diffs against _ZTV10dBgActor_c (ov002 0x0210ae38)
 * at exactly six: 0 InitResources, 3 CleanupResources, 6 Behavior, 9 Render,
 * 16 D1, 17 D0. Slot 31 is dBgActor_c::Kill at 0x020ee55c in both tables --
 * inherited, not overridden.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02112e0c  src/_ZN20daObjWanwanShutter_cD1Ev.cpp
 *   [1] 0x02112e50  src/_ZN20daObjWanwanShutter_cD0Ev.cpp
 *   [2] 0x02112ea8  src/func_ov014_02112ea8.cpp
 *   [3] 0x02112f3c  src/_ZN20daObjWanwanShutter_c16CleanupResourcesEv.cpp
 *   [4] 0x02112f80  src/_ZN20daObjWanwanShutter_c6RenderEv.cpp
 *   [5] 0x02112fc0  src/_ZN20daObjWanwanShutter_c8BehaviorEv.cpp
 *   [6] 0x02112ffc  src/_ZN20daObjWanwanShutter_c13InitResourcesEv.cpp
 *   [7] 0x0211307c  src/daObjWanwanShutter_c_classInit.c
 */
/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. The one ordering constraint that applies here -- common.h
 * ahead of Model.h -- is satisfied inside dBgActor_c.h itself, which this
 * file reaches through daObjWanwanShutter_c.h. Enrolled and compiling. */
#include "daObjWanwanShutter_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * STILL NOT reconciled against real project headers, and that is remaining
 * debt rather than a finding: each of these should be replaced by the real
 * include/*.h declaration, one at a time, re-proving the byte match after
 * each swap. They are load-bearing for the match as written -- see the
 * by-value-parameter note on func_ov014_02112ea8 for why a truer signature
 * can break it. */

/* shadow struct 'Sound' */
struct Sound { static void PlayBank3(unsigned int id, const Vector3 &v); };

/* shadow namespace 'Particle' */
namespace Particle { struct System { static void *NewSimple(unsigned int t, int x, int y, int z); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void * _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int t, int x, int y, int z);
 }

/* (dBgW: real header type in scope) */

/* shadow struct 'ActorS' */
struct ActorS {
    char pad0[0x5c];
    int px, py, pz;
    char pad1[0x74 - 0x68];
    Vector3 v74;
    char pad2[0x124 - 0x80];
    char col[4];  /* dBgW slot; the real class is abstract and cannot be a by-value member */
    char pad3[0x31e - 0x128];
    unsigned char flag;
    void PoofDustAt(const Vector3 &v);
};

/* shadow struct 'Base' */
struct Base {
    virtual int vf0(int);
    virtual int vf1(int);
    virtual int vf2(int);
    virtual int vf3(int);
    virtual int vf4(int);
    virtual int vfunc(int a);
};

/* shadow struct 'Obj' */
struct Obj {
    char pad[0xd4];
    Base sub;
};

extern "C" {
extern "C" void _ZN8dActor_c10PoofDustAtERK7Vector3(void *, const Vector3 &v);
extern int data_ov014_021149b8[];
extern int data_ov014_021149c0[];
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern int data_ov014_02114558[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjWanwanShutter_c_classInit, 0x0211307c, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daObjWanwanShutter_c_classInit
/* recovered: globals resolved, declarations from a shared header */
/* recovered: globals resolved */
/* resolved: VT = _ZTV20daObjWanwanShutter_c */
/* Reconstructed source-style name. Historical alias: ChainChompFence_Spawn. */
extern "C" int *daObjWanwanShutter_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)&_ZTV20daObjWanwanShutter_c[2]; /* +8: this TU defines the vtable */ }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN20daObjWanwanShutter_c13InitResourcesEv, 0x02112ffc, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* These three live in ov014, this file's own overlay. They were named off ov021 and
 * ov022, which share the same load window and so define their own symbols at the same
 * addresses -- but never at the same time as ov014, so those names cannot be what this
 * code reaches. Same address either way, so the bytes never noticed. */
int daObjWanwanShutter_c::InitResources()
{
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov014_021149c0);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)&mModel, m, 1, -1);
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  int k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov014_021149b8);
  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)&mMeshCollider, k, (char*)&mClsnMat, 0x1000, mAngleY, (void*)data_ov014_02114558);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN20daObjWanwanShutter_c8BehaviorEv, 0x02112fc0, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
int daObjWanwanShutter_c::Behavior()
{
    if (*(unsigned char*)((char*)&mDisabled) != 0) return 1;
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((void *)this), 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN20daObjWanwanShutter_c6RenderEv, 0x02112f80, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daObjWanwanShutter_c::Render()
{
    if (*(unsigned char*)((char*)&mDisabled) != 0) return 1;
    ((Obj *)this)->sub.vfunc(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN20daObjWanwanShutter_c16CleanupResourcesEv, 0x02112f3c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjWanwanShutter_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    }
    ((SharedFilePtr *)(data_ov014_021149c0))->Release();
    ((SharedFilePtr *)(data_ov014_021149b8))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov014_02112ea8, 0x02112ea8, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov014_02112ea8
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void func_ov014_02112ea8(ActorS *a)
{
    Vector3 v[2];
    Sound::PlayBank3(0xf, a->v74);
    {
        int ty = a->py;
        int tz = a->pz;
        int tx = a->px;
        v[0].x = tx;
        v[0].y = ty + 0x12c000;
        v[0].z = tz;
    }
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1e, v[0].x, v[0].y, v[0].z);
    v[1].x = v[0].x;
    v[1].y = v[0].y;
    v[1].z = v[0].z;
    _ZN8dActor_c10PoofDustAtERK7Vector3(a, v[1]);
    a->flag = 1;
    if (((dBgW *)a->col)->IsEnabled())
        ((dBgW *)a->col)->Disable();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN20daObjWanwanShutter_cD0Ev, 0x02112e50, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~daObjWanwanShutter_c()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
/* (no separate definition: the single ~daObjWanwanShutter_c() below emits the D0
 * and D1 variants together.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN20daObjWanwanShutter_cD1Ev, 0x02112e0c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN20daObjWanwanShutter_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjWanwanShutter_c : dBgActor_c`: its own vptr, then dBgActor_c's -- inlined,
 * because dBgActor_c's destructor is defined in its class body -- then
 * dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own.
 */
/* (no definition here: `virtual ~daObjWanwanShutter_c() {}` is in
 * include/daObjWanwanShutter_c.h, and that placement is load-bearing rather
 * than stylistic -- out of line, mwccarm emits D0 before D1 and adds a
 * homeless D2, and objisolate then refuses this whole TU. The header carries
 * the reasoning, the leaf measurement that makes it safe, and the note that
 * the 0x44 bytes come from inlining dBgActor_c's own inline destructor.) */
