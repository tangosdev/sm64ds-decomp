//cpp
// NONMATCHING: different op / idiom (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
struct Actor;
struct SurfaceInfo { void CopyNormalTo(Vector3&) const; };
struct RaycastLine {
    char _pad[0x14];
    SurfaceInfo surf;
    char _pad2[0x78 - 0x14 - 4];
    RaycastLine();
    ~RaycastLine();
    void SetObjAndLine(const Vector3&, const Vector3&, Actor*);
    int DetectClsn();
};
extern short SINE_TABLE[];

extern "C" int func_ov002_020c19d0(char* self, int arg1, int arg2);
int func_ov002_020c19d0(char* self, int arg1, int arg2) {
    Vector3 v1, v2, normal;
    int angle = *(unsigned short*)(self + 0x8e);
    RaycastLine rl;
    unsigned int idx = (unsigned int)(angle >> 4);
    {
        int t0 = SINE_TABLE[idx * 2];
        int t1 = SINE_TABLE[idx * 2 + 1];
        v1.y = *(int*)(self + 0x60) + (arg2 << 12);
        v1.z = *(int*)(self + 0x64);
        v1.x = *(int*)(self + 0x5c);
        v2.x = *(int*)(self + 0x5c) + arg1 * t0;
        v2.y = *(int*)(self + 0x60) + (arg2 << 12);
        v2.z = *(int*)(self + 0x64) + arg1 * t1;
    }
    rl.SetObjAndLine(v1, v2, (Actor*)self);
    if (!rl.DetectClsn()) {
        return 0;
    }
    rl.surf.CopyNormalTo(normal);
    if (normal.y != 0) {
        return 0;
    }
    return 1;
}
