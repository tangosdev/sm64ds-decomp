//cpp
#include "dBgCh_Lin.h"

struct V3 {
    int x, y, z;
    V3(int x, int y, int z) : x(x), y(y), z(z) {}
    V3() {}
};

struct dActor_c {
    char pad[0xb0];
    u32 flags;
    static dActor_c *FindWithID(u32 id);
};

extern "C" void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *out, dBgCh_Lin *line);

extern "C" int func_ov002_020c607c(char *self, int p1, int p2, int *outptr)
{
    *(int *)(self + 0x36c) = 0;
    *outptr = (int)0x80000000;

    dBgCh_Lin rc;

    V3 a(*(int *)(self + 0x5c), p1, *(int *)(self + 0x64));
    V3 b(*(int *)(self + 0x5c), p2, *(int *)(self + 0x64));
    V3 pos;
    rc.SetObjAndLine(*(Vector3*)&a, *(Vector3*)&b, (dActor_c *)self);
    if (rc.DetectClsn() != 0) {
        _ZN9dBgCh_Lin10GetClsnPosEv((Vector3*)&pos, &rc);
        *outptr = pos.y;
        if (rc.GetClsnID() != -1) {
            dActor_c *found = dActor_c::FindWithID((u32)rc.GetClsnID());
            *(int *)(self + 0x36c) = (int)found;
            if (found != 0) {
                int flag = (found->flags & 0x1000000) ? 1 : 0;
                if (flag != 0) {
                    return 1;
                }
                flag = (found->flags & 0x2000000) ? 1 : 0;
                if (flag != 0) {
                    return 2;
                }
            }
        }
    }
    return 0;
}
