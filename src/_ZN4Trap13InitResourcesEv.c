// @symbol _ZN4Trap13InitResourcesEv
/* recovered: vtable identified, shared common types, declarations from a shared header
 *
 * Trap::InitResources -- vtable slot 0, ov010 0x02111654. Attributed by the
 * vtable: _ZTV4Trap (ov010 0x02112ae4) carries this address at slot 0, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043c80.
 *
 * Kept as a free function under the literal mangled name rather than a real
 * `Trap::InitResources()` method: `self` arrives as a raw pointer and the
 * body still reads/writes by hand offset, unconverted -- the same shape
 * include/PushBlock.h's InitResources file documents. include/Trap.h is
 * included only for the flat, non-C++ half of that header, which still
 * spells this object's fields as `struct Trap` for exactly this reason. */
#include "decl_common.h"
#include "Trap.h"

extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, struct Vector3* v, void* rot, int e, int f);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* p);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* p, void* f, int a, int b);
extern void func_ov010_0211146c(char* c);
extern void func_ov010_021113f0(char* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* p);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* p, void* f, void* m, int fix, short s, void* clps);
extern void func_020393c4(int* p, int v);
extern void _ZN16MeshColliderBase6EnableEP8dActor_c(void* p, void* a);

extern short data_02082214[];

int _ZN4Trap13InitResourcesEv(char* c)
{
    struct Trap *self = (struct Trap *)(void *)c;
    self->unk_3aa = 0;
    self->unk_3ac = 0;

    if ((*(int*)(c + 8) & 0xff) == 0xff) {
        struct Vector3 v;
        int idx;
        int sx, sz;
        int x, y, z;
        void* sp;

        self->unk_3ab = 1;
        self->unk_3a4 = 0;

        idx = ((int)(self->unk_08e) >> 4) * 2;
        sx = data_02082214[idx + 1];
        sz = data_02082214[idx];
        z = self->unk_064 + sz * 0x15d;
        x = self->unk_05c - sx * 0x15d;
        y = self->unk_060;
        v.x = x;
        v.y = y;
        v.z = z;
        sp = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x24, 0, &v, c + 0x8c, self->unk_0cc, -1);
        *(int*)((char*)sp + 0x3ac) = *(int*)(c + 4);

        idx = ((int)(self->unk_08e) >> 4) * 2;
        sz = data_02082214[idx];
        sx = data_02082214[idx + 1];
        z = self->unk_064 - sz * 0x15d;
        x = sx * 0x15d + self->unk_05c;
        y = self->unk_060;
        v.x = x;
        v.y = y;
        v.z = z;
        sp = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x24, 1, &v, c + 0x8c, self->unk_0cc, -1);
        *(int*)((char*)sp + 0x3ac) = *(int*)(c + 4);

        return 1;
    }

    self->unk_3ab = 0;
    {
        void* f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov010_02112d08);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x320, f, 1, -1);
    }
    func_ov010_0211146c(c);
    func_ov010_021113f0(c);
    {
        void* f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov010_02112d00);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, f, c + 0x370, 0x1000, self->unk_08e, &data_ov010_021122f8);
    }
    func_020393c4((int*)(c + 0x124), (int)func_ov010_02111984);
    _ZN16MeshColliderBase6EnableEP8dActor_c(c + 0x124, c);
    self->unk_3a8 = 0;
    self->unk_3a0 = 0;

    if ((*(int*)(c + 8) & 0xff) == 1) {
        short* pa = (short*)(((int)c + 0x8e));
        *pa = *pa + 0x8000;
    }
    return 1;
}
