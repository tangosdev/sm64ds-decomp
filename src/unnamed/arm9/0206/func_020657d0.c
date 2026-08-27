#include "types.h"
struct S657 {
    int kind;        /* +0 */
    int pad4;        /* +4 */
    u16 id;          /* +8 */
    u16 pad_a;       /* +0xa */
    char pad_c[4];   /* +0xc */
    void *obj;       /* +0x10 */
    char rest[0x88]; /* +0x14 .. 0x9c */
};
extern void func_02065af0(struct S657 *s);
void func_020657d0(u16 id, void *obj)
{
    struct S657 s;
    s.kind = 2;
    s.id = id;
    s.obj = obj;
    func_02065af0(&s);
}
