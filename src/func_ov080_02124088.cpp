//cpp
// @symbol func_ov080_02124088
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;



#define LAUND(p) ((void*)((((long long)(int)(p)))))

extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* file, int a, int b, unsigned int e);
extern void func_0201267c(unsigned int id, const Vector3* v);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& v);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int a, const Vector3& pos, const void* vec16, int b, int c2);

}

extern "C" void func_ov080_02124088(char* c)
{
    Vector3 v1;
    Vector3 v3;
    Vector3 v2;
    u8 acc;
    int i;
    void* a;

    *(int*)(c + 0x17c) = 1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov080_021283d8[1], 0x40000000, 0x1000, 0);

    *(int*)(c + 0x12c) = 0;
    {
        u32* p150 = (u32*)LAUND(c + 0x150);
        *p150 |= 1;
    }
    {
        u32* pb0 = (u32*)LAUND(c + 0xb0);
        *pb0 &= ~0x10000000;
    }

    func_0201267c(0xd5, (const Vector3*)(c + 0x74));

    v1.x = *(int*)(c + 0x5c);
    {
        int y1 = *(int*)(c + 0x60);
        v1.y = y1;
        v1.z = *(int*)(c + 0x64);
        v1.y = y1 + (*(int*)(c + 0x140) - 0x50000);
    }
    ((int*)&v2)[0] = ((int*)&v1)[0];
    ((int*)&v2)[1] = ((int*)&v1)[1];
    ((int*)&v2)[2] = ((int*)&v1)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(c, v2);

    func_ov080_02124360(c);

    acc = *(u8*)(c + 0x184);
    i = 0;
    while (i < *(u8*)(c + 0x183)) {
        a = _ZN5Actor10FindWithIDEj(((u32*)(c + 0x16c))[i]);
        i = i + 1;
        if (a != 0) {
            acc = (u8)(acc + *(u8*)((char*)a + 0x184));
        }
    }

    if (acc < 7) goto tail_inc;
    if (acc != 7) return;

    v3.x = *(int*)(c + 0x5c);
    {
        int y3 = *(int*)(c + 0x60);
        v3.y = y3;
        v3.z = *(int*)(c + 0x64);
        v3.y = y3 + 0x64000;
    }
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x114, 0, v3, 0, (int)*(signed char*)(c + 0xcc), -1);

    *(u8*)(c + 0x184) = 8;
    return;

tail_inc:
    {
        u8* p184 = (u8*)LAUND(c + 0x184);
        (*p184)++;
    }
}
