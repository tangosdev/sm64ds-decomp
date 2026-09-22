/* HOST COPY of src/func_ov080_02124088.cpp -- MONTY_MOLE's defeat helper (the
 * anim flip, the poof dust, and the mole-hole census that pops the 1-Up when
 * the seventh mole of the group dies). The matched .cpp cannot be compiled
 * verbatim on the MSVC host: it declares its own `extern "C" void
 * func_ov080_02124088(char*)` while decl_common.h (which it includes) already
 * declares `extern void func_ov080_02124088(void*)`, and MSVC's C++ front end
 * rejects two extern "C" declarations that differ only in pointer type (C2733,
 * the FallBlockWf_InitVeneer case) -- a strictness mwccarm does not have.
 *
 * This is the port/unmatched/ host-copy pattern: the body is the matched
 * source's line for line, with only the self-declaration reconciled to the
 * decl_common (void*) spelling. The Actor::Spawn near the tail is id 0x114 =
 * 276 = ONE_UP_MUSHROOM, registered since gate 33; its result is discarded, so
 * a skipped spawn cannot fault.
 */
#include <cstdint>

typedef unsigned char u8;
typedef unsigned int u32;
struct Vector3 { int x, y, z; };
#define LAUND(p) ((void *)((((long long)(int)(p)))))

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a,
                                                 int b, unsigned e);
void func_0201267c(unsigned id, const Vector3 *v);
void _ZN8dActor_c10PoofDustAtERK7Vector3(void *self, const Vector3 &v);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned id, unsigned a,
                                                 const Vector3 &pos,
                                                 const void *vec16, int b,
                                                 int c2);
void func_ov080_02124360(void *c);
extern int data_ov080_021283d8[];
}

/* PORT_HOST_ABI: MSVC C2733 rejects the matched TU's second extern-C decl;
   body is the matched source's line for line, self-decl reconciled. */
extern "C" void func_ov080_02124088(void *self)
{
    char *c = (char *)self;
    Vector3 v1;
    Vector3 v3;
    Vector3 v2;
    u8 acc;
    int i;
    void *a;

    *(int *)(c + 0x17c) = 1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0xd4, (void *)data_ov080_021283d8[1], 0x40000000, 0x1000, 0);

    *(int *)(c + 0x12c) = 0;
    {
        u32 *p150 = (u32 *)LAUND(c + 0x150);
        *p150 |= 1;
    }
    {
        u32 *pb0 = (u32 *)LAUND(c + 0xb0);
        *pb0 &= ~0x10000000;
    }

    func_0201267c(0xd5, (const Vector3 *)(c + 0x74));

    v1.x = *(int *)(c + 0x5c);
    {
        int y1 = *(int *)(c + 0x60);
        v1.y = y1;
        v1.z = *(int *)(c + 0x64);
        v1.y = y1 + (*(int *)(c + 0x140) - 0x50000);
    }
    ((int *)&v2)[0] = ((int *)&v1)[0];
    ((int *)&v2)[1] = ((int *)&v1)[1];
    ((int *)&v2)[2] = ((int *)&v1)[2];
    _ZN8dActor_c10PoofDustAtERK7Vector3(c, v2);

    func_ov080_02124360(c);

    acc = *(u8 *)(c + 0x184);
    i = 0;
    while (i < *(u8 *)(c + 0x183)) {
        a = _ZN8dActor_c10FindWithIDEj(((u32 *)(c + 0x16c))[i]);
        i = i + 1;
        if (a != 0) {
            acc = (u8)(acc + *(u8 *)((char *)a + 0x184));
        }
    }

    if (acc < 7) goto tail_inc;
    if (acc != 7) return;

    v3.x = *(int *)(c + 0x5c);
    {
        int y3 = *(int *)(c + 0x60);
        v3.y = y3;
        v3.z = *(int *)(c + 0x64);
        v3.y = y3 + 0x64000;
    }
    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x114, 0, v3, 0,
                                                 (int)*(signed char *)(c + 0xcc),
                                                 -1);

    *(u8 *)(c + 0x184) = 8;
    return;

tail_inc:
    {
        u8 *p184 = (u8 *)LAUND(c + 0x184);
        (*p184)++;
    }
}
