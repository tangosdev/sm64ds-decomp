//cpp
// NONMATCHING: different op / idiom (div=39). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
struct State;
struct Actor;
struct RaycastLine {
    char buf[0x78];
    RaycastLine();
    ~RaycastLine();
    void SetObjAndLine(const Vector3 &a, const Vector3 &b, Actor *act);
    int DetectClsn();
    Vector3 GetClsnPos();
};
struct WithMeshClsn { int IsOnGround() const; };
struct Player {
    char pad[0x5c];
    int x, y, z;
    bool IsState(State &s);
};
extern "C" State _ZN6Player7ST_SWIME;
extern "C" int WATER_HEIGHT;

extern "C" void func_ov002_020ce798(Player *self)
{
    Vector3 v1, v2, cp;
    if (!self->IsState(_ZN6Player7ST_SWIME)) return;
    if (self->y >= WATER_HEIGHT - 0x50000) {
        if (!((WithMeshClsn *)((char *)self + 0x380))->IsOnGround())
            self->y = WATER_HEIGHT - 0x50000;
    }
    {
        RaycastLine rl;
        v1.x = self->x;
        v1.y = self->y;
        v1.z = self->z;
        v2.x = self->x;
        v2.y = self->y;
        v2.z = self->z;
        v1.y = self->y + 0x50000;
        v2.y = self->y + 0x96000;
        rl.SetObjAndLine(v1, v2, (Actor *)self);
        if (rl.DetectClsn()) {
            cp = rl.GetClsnPos();
            if (self->y >= cp.y - 0x82000) {
                self->y -= 0x4000;
                if (self->y < cp.y - 0x82000)
                    self->y = cp.y - 0x82000;
            }
        }
    }
}
