//cpp
/* Genuine production translation unit for ov002/daTree_c (8 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daTree_c -> d_a_tree.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `Tree`, a coined name. MEASURED in extracted/overlays/overlay_0002.bin
 * (ov002 base 0x020ad660, the `.text start:` on line 1 of
 * config/arm9/overlays/ov002/delinks.txt): the vtable object's preamble at
 * 0x0210abf8 is [offset-to-top 0, 0x0210abd0], and _ZTI8daTree_c at
 * 0x0210abd0 reads [0x0209a764, 0x0210abc4, 0x0208e390] --
 * _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), a typeinfo name
 * at 0x0210abc4 whose bytes are the string "8daTree_c", and _ZTI8dActor_c, so
 * the ROM states the direct base too. tools/class_rename.py performed the
 * rename, and it is what makes this promotion honest: ov002's symbols.txt
 * already carried _ZTS8daTree_c and _ZTI8daTree_c at those addresses, so under
 * the coined name the compiler's own _ZTS4Tree/_ZTI4Tree reached no symbol
 * home and could never be word-compared against the cartridge
 * (tools/tubuild.py apply_compiler_only_policy).
 *
 * That is why this TU now emits and licenses the whole RTTI/vtable group
 * rather than importing it. The first promotion of this entry kept the
 * destructors as extern "C" mangled shims precisely to force the vtable
 * symbol -- `_ZTV4Tree` at the time, the class still being coined -- to stay
 * an UNDEFINED import, and the cost was measurable: the eight legacy
 * per-function objects between them PROVED `_ZTV4Tree` against the ROM,
 * the shimmed TU proved nothing, and the tree's romdata-verified count fell by
 * one -- which is what the merge validator rejected.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Consolidates (and replaces) these legacy one-function sources, which this
 * promotion deletes (ROM address order):
 *   [0] 0x020ebf8c  src/_ZN4TreeD1Ev.cpp
 *   [1] 0x020ebfcc  src/_ZN4TreeD0Ev.cpp
 *   [2] 0x020ec020  src/_ZN4Tree16CleanupResourcesEv.cpp
 *   [3] 0x020ec0a0  src/_ZN4Tree16OnPendingDestroyEv.cpp
 *   [4] 0x020ec0a4  src/_ZN4Tree6RenderEv.cpp
 *   [5] 0x020ec1d8  src/_ZN4Tree8BehaviorEv.cpp
 *   [6] 0x020ec22c  src/_ZN4Tree13InitResourcesEv.cpp
 *   [7] 0x020ec32c  src/Tree_Spawn.cpp
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daTree_c.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win.
 * Model/Vector3 shadow structs dropped: real definitions come from
 * daTree_c.h's own includes. dCcPos_c stays an incomplete forward
 * declaration -- Init() is called through the extern "C" mangled form
 * below, not the real member, because the real signature takes two
 * Fix12<int> by value and this call site only has literal ints (same
 * materialization-cost trap documented on daObjPathLift_c/#1719). */
struct dCcPos_c;

/* shadow struct 'ModelBase' -- a vtable-slot probe reaching past the real
 * ModelBase.h's own 3 slots into Model's own extension, renamed from
 * 'ModelBase' to avoid colliding with the real class daTree_c.h now pulls in
 * transitively via Model.h. */
struct ModelBaseVProbe {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int arg);
};

extern "C" {
void _ZN8dCcPos_cD1Ev(void* c);
void _ZN6Memory16operator_delete2EPv(void* p);
extern char* data_ov002_02110a48[5];
struct Vec3 { int x, y, z; };
extern int data_0209f318;
extern int data_0209f43c;
extern int data_0209b3ec;
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void *a, void *b, void *c, int d, void *e);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int op, int z);
extern "C" void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(void *, const Vector3&, int, int, unsigned int, unsigned int);
extern "C" void* _Znwj(unsigned int);
extern "C" void _ZN8dCcPos_cC1Ev(void*);
extern "C" void Vec3_AsrInPlace(void*, int);
extern unsigned short data_ov002_0210abb8[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- Tree_Spawn, 0x020ec32c, size 0x5c */
/* -------------------------------------------------------------------------- */
#include "daTree_c.h"
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN5ModelC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void* _ZTV8daTree_c[];
int* Tree_Spawn(void){
  int* p = (int*)_ZN7fBase_cnwEj(sizeof(struct daTree_c));
  if(p){
    _ZN8dActor_cC2Ev(p);
    *(void***)p = (void**)&_ZTV8daTree_c[2]; /* +8: this TU defines the vtable */
    func_020733a8((char*)p+0xd4, 5, 0x50, _ZN5ModelC1Ev, _ZN5ModelD1Ev);
  }
  return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN8daTree_c13InitResourcesEv, 0x020ec22c, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daTree_c13InitResourcesEv
/* daTree_c::InitResources -- vtable slot 0, ov002 0x020ec22c.
 *
 * A REAL MEMBER ON PURPOSE, and it has to be. The destructor is defined
 * inline in include/daTree_c.h, so the class's key function is its first
 * DECLARED non-inline virtual -- this one (notes/, and the key-function rule:
 * first declared, not first slot). While this body was a hand-mangled
 * `extern "C"` free function, no TU anywhere defined the key function, so
 * mwccarm emitted neither the _ZTV/_ZTI/_ZTS group nor the inline
 * destructor's D1/D0 pair, and objisolate refused the whole TU with
 * `_ZN8daTree_cD1Ev has 0 defined symbols`. Written as a member it emits
 * both, and this entry's compiler_only_output licenses the RTTI group
 * record by record against the cartridge.
 *
 * Only the CALLEE declarations stay hand-spelled: dCcPos_c::Init's ROM name
 * carries by-value class parameters (Fix12<int>), which mwccarm passes
 * differently at the call site, so declaring the true types breaks the byte
 * match. See notes/mwccarm-codegen.md 6az. That exception is about the
 * callee signature, not daTree_c method ownership. */
int daTree_c::InitResources() {
    char* self = (char*)this;
    int idx = ((unsigned int)*(int*)(self + 8) >> 4) & 7;
    char** slot;
    char* p;
    if (idx >= 4) idx = 4;
    slot = &data_ov002_02110a48[idx];
    if (*slot == 0) {
        ((Model*)(self + 0xd4 + idx * 0x50))->LoadAndSetFile(data_ov002_0210abb8[idx], 1, 1);
    }
    p = (char*)_Znwj(0x4c);
    if (p) _ZN8dCcPos_cC1Ev(p + 0xc);
    *(int*)(p + 0) = *(int*)(self + 0x5c);
    *(int*)(p + 4) = *(int*)(self + 0x60);
    *(int*)(p + 8) = *(int*)(self + 0x64);
    Vec3_AsrInPlace(p, 3);
    {
        int* q = (int*)(((int)p + 4));
        *q = *q + 0x1e000;
    }
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj((dCcPos_c*)(p + 0xc), *(Vector3*)(self + 0x5c), 0x35555, 0x1f4000, 0x380000c, 0);
    *(int*)(p + 0x48) = (int)*slot;
    *slot = p;
    if (*(int*)(p + 0x48) != 0) {
        return 0;
    } else {
        return 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN8daTree_c8BehaviorEv, 0x020ec1d8, size 0x54 */
/* -------------------------------------------------------------------------- */
extern "C" {
extern int _ZN5dCc_c5ClearEv(void*);
extern int _ZN5dCc_c6UpdateEv(void*);
extern char* data_ov002_02110a48[5];
int _ZN8daTree_c8BehaviorEv(void){
  char** pp = data_ov002_02110a48;
  int i;
  for(i=0;i<5;i++){
    char* p = *pp;
    while(p){
      _ZN5dCc_c5ClearEv(p+0xc);
      _ZN5dCc_c6UpdateEv(p+0xc);
      p = *(char**)(p+0x48);
    }
    pp++;
  }
  return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN8daTree_c6RenderEv, 0x020ec0a4, size 0x134 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daTree_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daTree_c::Render()
{
    char *base = (char *)data_0209f318;
    char **iter = data_ov002_02110a48;
    char *sl = ((char *)this) + 0xd4;
    int i = 0;
    int z5 = 0;
    int z4 = 0;
    int c1f = 0x1f;
    int c1e000 = 0x1e000;
    Vec3 buf;
    int *sb;
    char *r8;
    char *r0;

    do {
        r8 = sl + 0x1c;
        Matrix4x3_FromRotationY(r8, *(short *)(base + 0x17c));
        sb = (int *)*iter;
        while (sb != 0) {
            int v = _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
                        &data_0209f43c, &data_0209b3ec, sb, c1e000, &buf);
            if (v > 0x11000 && v < 0x578000) {
                int op = c1f;
                if (v < 0x2f000) {
                    op = ((v - 0x10000) >> 12) & 0xff;
                }
                _ZN9ModelBase12ApplyOpacityEj(sl, op, z5);
                r0 = sl;
                *(int *)(r8 + 0x24) = sb[0];
                *(int *)(r8 + 0x28) = sb[1] - 0x1e000;
                *(int *)(r8 + 0x2c) = sb[2];
                ((ModelBaseVProbe *)r0)->m(z4);
            }
            sb = (int *)sb[0x12];
        }
        sl += 0x50;
        i++;
        iter++;
    } while (i < 5);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN8daTree_c16OnPendingDestroyEv, 0x020ec0a0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daTree_c16OnPendingDestroyEv
/* daTree_c::OnPendingDestroy -- vtable slot 12. The ROM body is empty: the
 * override exists only to occupy the slot. */
void daTree_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8daTree_c16CleanupResourcesEv, 0x020ec020, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daTree_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
int daTree_c::CleanupResources()
{
  char* r7 = ((char*)this) + 0xd4;
  char** r6 = data_ov002_02110a48;
  int i;
  for (i = 0; i < 5; i++){
    if (*(int*)(r7 + 0x4c) != 0){
      while (*r6 != 0){
        char* r4 = *r6;
        *r6 = *(char**)(r4 + 0x48);
        if (r4 != 0){
          _ZN8dCcPos_cD1Ev(r4 + 0xc);
          _ZN6Memory16operator_delete2EPv(r4);
        }
      }
    }
    r7 += 0x50;
    r6 += 1;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN8daTree_cD1Ev 0x020ebf8c size 0x40 and          */
/* _ZN8daTree_cD0Ev 0x020ebfcc size 0x54 -- are NOT written here.             */
/*                                                                            */
/* The destructor is defined INLINE in include/daTree_c.h. Written out-of-line*/
/* here the real destructor makes mwccarm emit D0 BEFORE D1, the reverse of the*/
/* cartridge's order, which objisolate refuses for the whole TU, and it emits a*/
/* third D2 body with no ROM home. The inline definition gives the retail D1/D0*/
/* pair in ROM order and no D2, and it replaces the two hand-written extern "C"*/
/* mangled shims this file used to carry: those existed only to keep          */
/* _ZTV8daTree_c an UNDEFINED import while the class still had its coined name,*/
/* and they cost D0 and D1 their place in the CONVERTED tier. Both are real   */
/* compiler-emitted C++ member bodies again, and the two entries in           */
/* config/converted-backslide-exceptions.jsonl that recorded the loss are gone.*/
/*                                                                            */
/* This TU therefore emits the class's complete _ZTV/_ZTI/_ZTS group, which is*/
/* the point: every record is licensed in this entry's compiler_only_output as*/
/* `deadstrip-data` with a canonical module and address, so romdata_check     */
/* word-compares each against the cartridge. Under the coined name _ZTS4Tree and*/
/* _ZTI4Tree reached no symbol home and the group could not be licensed at all.*/
/* -------------------------------------------------------------------------- */
