//cpp
/* HAND-ASSEMBLED translation unit -- ov029/daObjWc_Obj05_c (9 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * PROMOTED: this file is compiled and linked into the ROM, and ov029's
 * delinks.txt gives it the whole .text run 0x02111ac4..0x02111ea4. The nine
 * legacy one-function sources listed below are deleted; they are named for
 * provenance only.
 *
 * The class is spelled as the cartridge spells it. ov029 _ZTI 0x02113e30 holds
 * _ZTS 0x02113e3c, reading `15daObjWc_Obj05_c`, and its base word 0x021089ec is
 * _ZTI10dBgActor_c. It is a leaf: a scan of all 977 .bin files under extracted/
 * for the word 0x02113e30 finds exactly two hits, both at file offset 0x2cd0 in
 * the same overlay under two paths, which is this class's own _ZTV - 4.
 *
 * The destructor is defined INLINE in include/daObjWc_Obj05_c.h. Out of line
 * mwccarm emits D2, D0, D1; the ROM has D1 then D0 and no D2. Because the
 * destructor is inline the key function is InitResources -- the first DECLARED
 * non-inline virtual -- so this TU also emits the class's vague-linkage
 * _ZTV/_ZTI/_ZTS plus the eight inherited base-chain records. All eleven are
 * licensed in the manifest's compiler_only_output and word-compared against the
 * cartridge by tools/romdata_check.py: 6 VERIFIED, 5 PARTIAL, 0 DIFFERS. The
 * PARTIALs are extent shortfalls, not disagreements -- every byte compared is
 * equal; see the manifest note for why the vtable's own extent is cut short.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111ac4  src/_ZN15daObjWc_Obj05_cD1Ev.cpp
 *   [1] 0x02111b08  src/_ZN15daObjWc_Obj05_cD0Ev.cpp
 *   [2] 0x02111b60  src/_ZN15daObjWc_Obj05_c16CleanupResourcesEv.cpp
 *   [3] 0x02111ba4  src/_ZN15daObjWc_Obj05_c6RenderEv.cpp
 *   [4] 0x02111bcc  src/_ZN15daObjWc_Obj05_c8BehaviorEv.cpp
 *   [5] 0x02111d6c  src/_ZN15daObjWc_Obj05_c13InitResourcesEv.cpp
 *   [6] 0x02111e40  src/func_ov029_02111e40.cpp
 *   [7] 0x02111e60  src/func_ov029_02111e60.c
 *   [8] 0x02111e74  src/daObjWc_Obj05_c_classInit.c
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObjWc_Obj05_c_classInit, 0x02111e74, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV15daObjWc_Obj05_c[];
/* Reconstructed source-style name; exact original SM64DS spelling is not
 * preserved. Historical alias: CageLift_Spawn. */
int *daObjWc_Obj05_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)&_ZTV15daObjWc_Obj05_c[2]; /* +8: this TU defines the vtable */ }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov029_02111e60, 0x02111e60, size 0x14 */
/* -------------------------------------------------------------------------- */
// Arg-shifting tail-call veneer. Drops the first argument and forwards the next two;
// #pragma long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call
// (bracketed with `off` so it cannot leak into later members). decl_common.h
// declares this name as `void(void)` for the callback-pointer view; the definition
// and the callee's int view live in namespaces, all binding the same C symbols.
namespace tail_e40 { extern "C" int func_ov029_02111e40(void *b, void *c); }
#pragma long_calls on
namespace fndef_e60 {
extern "C" int func_ov029_02111e60(void *a, void *b, void *c)
{
    return tail_e40::func_ov029_02111e40(b, c);
}
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov029_02111e40, 0x02111e40, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" void func_ov029_02111e40(char *a, char *b){ unsigned char ok = *(unsigned short*)(b+0xc)==0xbf; if(ok) *(unsigned char*)(a+0x32a)=1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN15daObjWc_Obj05_c13InitResourcesEv, 0x02111d6c, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c13InitResourcesEv
#include "daObjWc_Obj05_c.h"
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
// recovered name: daObjWc_Obj05_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_(void);

s32 daObjWc_Obj05_c::InitResources() {
    char* c = (char*)this;
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_0211428c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    void* mc = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov029_02114284);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x2ec, 0x1000, this->mAngleY, data_ov029_0211306c);
    func_020393d4(c + 0x124, (void*)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(c + 0x124, (void*)&func_ov029_02111e60);
    this->unk_32b = 0;
    this->unk_32a = this->unk_32b;
    this->unk_328 = 0;
    if (*(int*)(c + 8) & 1)
        this->unk_32c = 3;
    else
        this->unk_32c = 0;
    this->unk_320 = this->mPosY;
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN15daObjWc_Obj05_c8BehaviorEv, 0x02111bcc, size 0x1a0 */
/* -------------------------------------------------------------------------- */
#include "types.h"
// @symbol _ZN15daObjWc_Obj05_c8BehaviorEv
#include "daObjWc_Obj05_c.h"
// recovered name: daObjWc_Obj05_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::Behavior - recovered from vtable slot identity */
extern "C" {
extern void func_020393a4(int* p, int v);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void* pos, unsigned e);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* self);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* self);
extern void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void* self, void* mat, s16 s);
}

s32 daObjWc_Obj05_c::Behavior() {
    u8* thiz = (u8*)this;
    func_020393a4((int*)(thiz + 0x124), 0x250000);

    switch (thiz[0x32c]) {
    case 0:
        if (thiz[0x32a] != 0) {
            if (thiz[0x32b] == 0) {
                u8* p = (u8*)(((int)thiz + 0x32c));
                *p = *p + 1;
            }
        }
        thiz[0x32b] = thiz[0x32a];
        break;
    case 1: {
        int* p60 = (int*)(((int)thiz + 0x60));
        *p60 = *p60 - 0x14000;
        this->unk_324 = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *(unsigned*)(thiz + 0x324), 3, 0x8d, thiz + 0x74, 0);
        {
            int v = this->unk_320 + (int)0xff5d8000;
            if (this->mPosY <= v) {
                this->mPosY = v;
                {
                    u8* p = (u8*)(((int)thiz + 0x32c));
                    *p = *p + 1;
                }
                {
                    u8* base = (u8*)(((int)thiz + 0x300));
                    *(u16*)(base + 0x28) = 0;
                }
            }
        }
        break;
    }
    case 2: {
        /* Rematerialize timer base - do not keep live across calls */
        if (*(u16*)((u8*)(((unsigned)thiz + 0x300)) + 0x28) >= 0x6e) {
            int snd = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                *(unsigned*)(thiz + 0x324), 3, 0x8d, thiz + 0x74, 0);
            this->unk_324 = snd;
            {
                int* p60 = (int*)(((int)thiz + 0x60));
                *p60 = *p60 + 0xa000;
            }
            if (this->mPosY >= this->unk_320) {
                this->mPosY = this->unk_320;
                thiz[0x32c] = 0;
                {
                    /* Different launder spelling so CSE does not reuse case-2 head base */
                    u8* b2 = (u8*)(((int)thiz + 0x300));
                    *(u16*)(b2 + 0x28) = 0;
                }
            }
        } else {
            u16* p = (u16*)(((int)thiz + 0x328));
            *p = *p + 1;
        }
        break;
    }
    case 3:
        break;
    }

    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(thiz);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(thiz, 0, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(thiz);
    }
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(
        thiz + 0x124, thiz + 0x2ec, this->mAngleY);
    thiz[0x32a] = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN15daObjWc_Obj05_c6RenderEv, 0x02111ba4, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c6RenderEv
#include "daObjWc_Obj05_c.h"
// recovered name: daObjWc_Obj05_c_Render
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::Render - recovered from vtable slot identity */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
s32 daObjWc_Obj05_c::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN15daObjWc_Obj05_c16CleanupResourcesEv, 0x02111b60, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_c16CleanupResourcesEv
#include "daObjWc_Obj05_c.h"
// recovered name: daObjWc_Obj05_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov029_0211428c[];
}

s32 daObjWc_Obj05_c::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_0211428c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_02114284);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN15daObjWc_Obj05_cD0Ev, 0x02111b08, size 0x58 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol _ZN15daObjWc_Obj05_cD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
extern int _ZTV15daObjWc_Obj05_c[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified, renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void *data_020a0eac;
/* (no separate definition: the single ~daObjWc_Obj05_c() below emits the D0 and D1
 * variants together -- a hand-mangled D0 next to a real destructor is the
 * known mwccarm ICE, ELFgen.c:483.) */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN15daObjWc_Obj05_cD1Ev, 0x02111ac4, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj05_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then ~dBgActor_c. */
#include "daObjWc_Obj05_c.h"

/* (no out-of-line body: the destructor is defined inline in the header, which is
 * what makes mwccarm emit D1 then D0 and no D2 -- the ROM's own order.) */

