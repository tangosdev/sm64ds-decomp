#include "types.h"
struct Ent { void *p0; void *p4; };
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *f, int b, int c, unsigned int d);

void func_ov002_020f6514(u8 *self, u8 *tbl, u8 arg)
{
    u8 *e;
    int v;
    *(u8 *)(self + 0x82) = arg;
    e = tbl + *(u8 *)(self + 0x82) * 4;
    v = (*(u8 *)(e + 2) == 0) ? 0x40000000 : 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        self,
        ((struct Ent **)*(void **)(self + 0x74))[*(u8 *)(e + 0)]->p4,
        v, 0x1000, 0);
    {
        void *ts = *(void **)(self + 0x7c);
        s8 idx;
        if (ts == 0) return;
        idx = *(s8 *)(e + 1);
        if (idx < 0) return;
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            ts,
            ((struct Ent **)*(void **)(self + 0x78))[idx]->p4,
            v, 0x1000, 0);
    }
}
