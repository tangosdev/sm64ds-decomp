//cpp
typedef unsigned int u32;

struct Vector3 {
    int x, y, z;
    Vector3(int x, int y, int z) : x(x), y(y), z(z) {}
    Vector3() {}
};

struct dBgPi {
    char pad[0x68];
    int GetClsnID() const;
};

struct dActor_c {
    char pad[0xb0];
    u32 flags;
    static dActor_c *FindWithID(u32 id);
};

struct dBgCh_Lin {
    char head[0x10];
    dBgPi mResult;
    dBgCh_Lin();
    ~dBgCh_Lin();
    void SetObjAndLine(const Vector3 &a, const Vector3 &b, dActor_c *actor);
    int DetectClsn();
    Vector3 GetClsnPos();
};

extern "C" int func_ov002_020c607c(char *self, int p1, int p2, int *outptr)
{
    Vector3 pos;

    *(int *)(self + 0x36c) = 0;
    *outptr = (int)0x80000000;

    dBgCh_Lin rc;

    Vector3 a(*(int *)(self + 0x5c), p1, *(int *)(self + 0x64));
    Vector3 b(*(int *)(self + 0x5c), p2, *(int *)(self + 0x64));
    rc.SetObjAndLine(a, b, (dActor_c *)self);
    if (rc.DetectClsn() != 0) {
        pos = rc.GetClsnPos();
        *outptr = pos.y;
        if (rc.mResult.GetClsnID() != -1) {
            dActor_c *found = dActor_c::FindWithID((u32)rc.mResult.GetClsnID());
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
