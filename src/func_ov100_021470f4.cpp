//cpp
// @symbol func_ov100_021470f4
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named */
#include "daObjPathLift_c.h"
#include "math/Fix12.h"
// recovered name: daObjPathLift_c_Behavior
/* recovered: shared common types, renamed to Class_Method */
/* daObjPathLift_c::Behavior - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
typedef int Fix12i;

struct PathLift { void BaseBehavior(); };
extern "C" Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);
/* The fifth parameter is a SIGNED SHORT, not an unsigned int. Declared `unsigned
   int' this mangles _ZN5Sound8PlayLongEjjjRK7Vector3j, which is the symbol that
   correction removed -- and because this file declares the function in C++ and
   lets the compiler mangle it, rather than spelling the mangled name, a textual
   search for the old symbol could never find it. The pre-push reference check
   did: "new unresolved reference: func_ov100_021470f4". */
namespace Sound { unsigned int PlayLong(unsigned int, unsigned int, unsigned int, const Vector3&, short); }
/* `Platform2` and `Platform3` were phantom CLASSES, and nothing in the source text
   said so -- the names only exist after mangling. `_ZN10dBgActor_c19UpdateClsnPosAndRotEv`
   is dBgActor_c::UpdateClsnPosAndRot, but read as `_ZN9Platform2` + `19UpdateClsn...` it
   splits into a class `Platform2` whose method name is one character shorter. Two
   different shadow structs grew out of one class that way. The length prefix is the
   only thing that distinguishes them, and the retail relocations for both call sites
   land on dBgActor_c's methods. */
struct dActor_c;
struct dBgActor_c { void UpdateClsnPosAndRot(); };

/* IsClsnInRange takes two Fix12<int>, not two ints -- the S1_ back-reference in
   _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_ can only come from a repeated class type.
   It is declared by its final name rather than as a member of dBgActor_c above,
   because a faithful `int IsClsnInRange(Fix12<int>, Fix12<int>)` does not reproduce
   the call: Fix12<int> is an aggregate with no converting constructor from int, and
   every way of materialising a zero one (`= {0}`, `Fix12<int>()`, a named local)
   costs stack traffic the ROM does not have -- measured at 0x100 and 0x104 bytes
   against the cartridge's 0xec. The ROM passes the pair in registers exactly as two
   ints, so that is what the call site says. */
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern "C" void func_020393a4(int *p, int v);
extern "C" void func_02039394(int *p, int v);
struct MeshColliderBase { int IsEnabled(); void Enable(dActor_c *a); };
extern unsigned char data_0209f2d8;

extern "C" int func_ov100_021470f4(char* c)
{
    struct daObjPathLift_c *self = (struct daObjPathLift_c *)(void *)c;
    func_ov002_020efcf4(c);
    ((PathLift*)c)->BaseBehavior();
    if (Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x68)) != 0) {
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x4b0)) == 0) {
            self->unk_4a8 = Sound::PlayLong(self->unk_4a8, 3, 0x82, *(Vector3*)(c + 0x74), 0);
        }
    }
    func_ov100_0214700c(c);
    ((dBgActor_c*)c)->UpdateClsnPosAndRot();
    func_ov100_02146e70(c);
    func_020393a4((int*)(c + 0x124), 0x150000);
    func_02039394((int*)(c + 0x124), 0x1000);
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (((MeshColliderBase*)(c + 0x124))->IsEnabled() == 0) {
            ((MeshColliderBase*)(c + 0x124))->Enable((dActor_c*)(void*)c);
        }
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    }
    return 1;
}
