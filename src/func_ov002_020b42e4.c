// @symbol func_ov002_020b42e4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct dActor_c {
    char pad4[4];
    int f4;     // 0x4
    char pad_to434[0x434 - 8];
    int f434;   // 0x434
};

extern void _ZN8dActor_c11UntrackStarERa(char *thiz, signed char *star);
extern struct dActor_c *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int a, unsigned int b, const struct Vector3 *pos,
    const struct Vector3_16 *rot, int e, int f);
extern void LinkSilverStarAndStarMarker(char *a, char *b);

void func_ov002_020b42e4(char *self)
{
    struct Vector3 v;
    struct dActor_c *a;
    struct dActor_c *b;

    _ZN8dActor_c11UntrackStarERa(self, (signed char *)(self + 0xd5));
    func_ov002_020b41b8((int *)&v, self);
    a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xb4, 0x50, &v, 0, *(signed char *)(self + 0xcc), -1);
    b = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xb3, (*(signed char *)(self + 0xd4)) | 0x10, &v, 0,
        *(signed char *)(self + 0xcc), -1);
    if (a == 0)
        return;
    if (b == 0)
        return;
    b->f434 = a->f4;
    LinkSilverStarAndStarMarker((char *)a, (char *)b);
}
