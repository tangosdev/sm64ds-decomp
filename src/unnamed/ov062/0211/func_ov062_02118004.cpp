//cpp
struct dBgCh_Actr {
    char pad[1];
    bool IsOnGround() const;
};
extern "C" int _Z14ApproachLinearRiii(int &r, int target, int speed);
struct Particle {
    static void RunningSlidingDustAt(int a, int b, int c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);

extern "C" void func_ov062_02118004(void *c, int a1) {
    int r = ((dBgCh_Actr*)((char*)c + 0x144))->IsOnGround();
    if (r == 0) return;
    _Z14ApproachLinearRiii(*(int*)((char*)c + 0x98), 0, a1);
    int x = *(int*)((char*)c + 0x5c);
    int y = *(int*)((char*)c + 0x60);
    int z = *(int*)((char*)c + 0x64);
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(x, y, z);
}
