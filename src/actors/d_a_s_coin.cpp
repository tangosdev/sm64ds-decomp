//cpp
/* ov002 / daSCoin_c -- the hidden "secret" coin (registry profile spelled
 * InvisibleSecret_SpawnInfo, RTTI name at ov002:0x0210aff4).
 *
 * One translation unit for the whole ROM run 0x020f03c4..0x020f0894: the three
 * class methods, the destructor pair, the three file-local helpers between
 * them, and the factory, plus the .data run 0x0210aff4..0x0210b0ac that holds
 * the class's RTTI pair, its profile descriptor and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * .data objects are laid out by mwld in ASCENDING SIZE: the RTTI pair (0xc
 * each) first, the 0x1c profile next, the 0x84 vtable object last -- the ROM's
 * own order for this run.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daSCoin_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daSCoin_c.h"
#include "decl_common.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'dActor_c' */
struct dActor_c;

extern "C" {
extern struct dActor_c* _ZN8dActor_c4NextEPKS_(const struct dActor_c* a);
extern int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* prev);
extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* v, void* w, int e, int f);
extern void _ZN9PowerStar13AddStarMarkerEv(void* thiz);
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int data_ov002_0210d9a8;
extern int _ZTV9daSCoin_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daSCoin_c_Spawn, 0x020f085c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daSCoin_c_Spawn
/* recovered: vtable identified, declarations from a shared header. Was
 * InvisibleSecret_Spawn -- renamed to match the RTTI class name
 * (_ZTS9daSCoin_c), see include/daSCoin_c.h. */
/* vtable identified: VT0 = _ZTV9daSCoin_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daSCoin_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(276);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x0210b028 and +8 is what reaches
         * the 0x0210b030 address point the ROM stores. */
        p[0] = (int)&_ZTV9daSCoin_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* InvisibleSecret_SpawnInfo, 0x0210b00c, size 0x1c                           */
/* -------------------------------------------------------------------------- */
/* The registry descriptor. Its symbol keeps the project's existing coined
 * spelling; the class it names is ROM-proven daSCoin_c (_ZTS9daSCoin_c). */
struct SCoinSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x0149 */
    s16 drawOrder;                  /* 0x014a */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char SCoinSpawnInfo_size_must_be_0x1c[
    sizeof(SCoinSpawnInfo) == 0x1c ? 1 : -1];

// @symbol InvisibleSecret_SpawnInfo
extern "C" SCoinSpawnInfo InvisibleSecret_SpawnInfo = {
    daSCoin_c_Spawn, 0x0149, 0x014a, 0x00000000,
    0x00000000, 0x00320000, 0x01f40000, 0x00050000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN9daSCoin_c13InitResourcesEv, 0x020f07dc, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSCoin_c13InitResourcesEv
/* daSCoin_c::InitResources -- vtable slot 0. Real C++ method over the shared
 * header, named members. */
s32 daSCoin_c::InitResources(){
    char* c = (char*)this;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0xd4, c, 0x64000, 0x40000, 0x800002, 0);
    unk_10d = *(unsigned int*)(c + 8) & 0xf;
    mGroupId = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    mGroupRole = 0;
    mLeaderUniqueID = 0;
    mCollectedCount = 0;
    mDeathTimer = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN9daSCoin_c8BehaviorEv, 0x020f06c0, size 0x11c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSCoin_c8BehaviorEv
/* daSCoin_c::Behavior -- vtable slot 6. Real C++ method over the shared header,
 * named members and real inherited method calls. */
s32 daSCoin_c::Behavior()
{
    char *c = (char *)this;
    daSCoin_c *self = this;
    if (self->mDeathTimer) {
        if (DecIfAbove0_Byte((unsigned char *)(c + 0x113)) == 0) {
            func_ov002_020f05f4(c);
            ((fBase_c *)c)->MarkForDestruction();
        }
        return 1;
    }
    if (self->mGroupRole == 0) {
        unsigned char st = self->mGroupId;
        if (st == 0 || st == 0xf) {
            {
                dActor_c *o = 0;
                self->mGroupRole = 1;
                self->mLeaderUniqueID = *(int *)(c + 4);
                for (;;) {
                    o = dActor_c::FindWithActorID(0x149, o);
                    if (o == 0) break;
                    if (o != (dActor_c *)c) {
                        *(unsigned char *)((char *)o + 0x10f) = 2;
                        *(int *)((char *)o + 0x108) = *(int *)(c + 4);
                    }
                }
            }
        }
    }
    if (self->mGroupRole == 1 && self->mCollectedCount == 5) {
        ((fBase_c *)c)->MarkForDestruction();
        return 1;
    }
    func_ov002_020f051c(c);
    if (*(int *)(c + 0xf8)) {
        func_ov002_020f0438(c);
    }
    ((dCc_c *)(c + 0xd4))->Clear();
    if (self->mClsnDisabled == 0) {
        ((dCc_c *)(c + 0xd4))->Update();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN9daSCoin_c16CleanupResourcesEv, 0x020f069c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSCoin_c16CleanupResourcesEv
/* daSCoin_c::CleanupResources -- vtable slot 3. Releases the one shared file the
 * class holds and reports success; it never touches `this`, which is why the
 * legacy C form could declare itself nullary and still reproduce. */
s32 daSCoin_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a8);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020f05f4, 0x020f05f4, size 0xa8 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f05f4(void* self)
{
    char* c = (char*)self;
    char* a = 0;
    for (;;) {
        a = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0xb4, a);
        if (a == 0) return;
        if (*(unsigned char*)(c + 0x10d) == *(unsigned char*)(a + 0x1d9)) {
            int* base = (int*)(a + 0x5c);
            Vector3 pos;
            pos.x = base[0];
            pos.y = base[1];
            pos.z = base[2];
            pos.y += 0x12c000;
            {
                char* p = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, (*(unsigned char*)(c + 0x10d)) | 0x40, &pos, 0, *(signed char*)(c + 0xcc), -1);
                if (p != 0) {
                    _ZN9PowerStar13AddStarMarkerEv(p);
                }
            }
            return;
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov002_020f051c, 0x020f051c, size 0xd8 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov002_020f051c(void* self)
{
    char* c = (char*)self;
    struct dActor_c* a;
    u32 t;
    int b;
    if (*(u8*)(c+0x112) != 0) return;
    a = _ZN8dActor_c4NextEPKS_(0);
    if (a == 0) goto done;
    do {
        t = *(u16*)((char*)a + 0xc);
        b = (t == 0x14);
        if (b == 0) {
            b = (t == 0x15);
            if (b == 0) goto chk2;
        }
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)((char*)a + 0x5c)) < 0xc8000) {
            *(u8*)(c+0x111) = 1;
            *(int*)((char*)a + 0x3f4) = (int)c;
            goto done;
        }
        goto next;
      chk2:
        b = (t == 0xc0);
        if (b == 0) goto next;
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)((char*)a + 0x5c)) < 0xc8000) {
            *(u8*)(c+0x111) = 1;
            *(int*)((char*)a + 0x4f0) = (int)c;
            goto done;
        }
      next:
        a = _ZN8dActor_c4NextEPKS_(a);
    } while (a != 0);
  done:
    *(u8*)(c+0x112) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020f0438, 0x020f0438, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020f0438
/* The legacy file carried its own flat `struct dActor_c` shadow whose fields
 * at 0x108..0x113 are in fact daSCoin_c's own group bookkeeping. Reconciled
 * onto the real class; the calls keep their literal mangled spellings, which
 * is what the ROM symbols are. */
extern "C" {
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void func_02012790(int);
short _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(void* thiz, const Vector3& v,
        unsigned int a, bool b, unsigned short c, void* d);
void _ZN7fBase_c18MarkForDestructionEv(void* thiz);

void func_ov002_020f0438(void* self)
{
  daSCoin_c* thiz = (daSCoin_c*)self;
  daSCoin_c* o = (daSCoin_c*)_ZN8dActor_c10FindWithIDEj(thiz->mLeaderUniqueID);
  if (!o) return;
  if (thiz->mGroupId != 0xf && thiz->mGroupId != o->mCollectedCount) return;
  func_02012790(0x25);
  o->mCollectedCount++;
  {
    Vector3 v;
    v.x = thiz->mPosX;
    v.y = thiz->mPosY;
    v.z = thiz->mPosZ;
    _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_(thiz, v, o->mCollectedCount, false, 0, 0);
  }
  *(int*)((int)thiz + 0xec) |= 1;
  if (o->mCollectedCount == 5) { thiz->mDeathTimer = 0x1e; return; }
  if (thiz->mGroupRole != 2) return;
  _ZN7fBase_c18MarkForDestructionEv(thiz);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN9daSCoin_cD1Ev  0x020f03c4  size 0x30  (complete-object destructor)   */
/*   _ZN9daSCoin_cD0Ev  0x020f03f4  size 0x44  (deleting destructor)          */
/* -------------------------------------------------------------------------- */
/* Defined in include/daSCoin_c.h's class body, and deliberately NOT repeated
 * out of line here: out of line mwccarm emits D2, D0, D1, and the cartridge
 * holds D1, D0 with no D2 at all. Both bodies are automatic -- this class's
 * vptr, the dCcAc_c at 0xd4 and dActor_c's own teardown -- and D0 additionally
 * returns the object to its heap through the inline operator delete it
 * inherits. */

// @symbol _ZN9daSCoin_cD0Ev
// @symbol _ZN9daSCoin_cD1Ev
