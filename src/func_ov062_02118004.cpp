//cpp
struct WithMeshClsn {
    char pad[1];
    bool IsOnGround() const;
};
extern "C" int _Z14ApproachLinearRiii(int &r, int target, int speed);
struct Particle {
    static void RunningSlidingDustAt(int a, int b, int c);
};
extern "C" void func_ov062_02118004(void *c, int a1) {
    int r = ((WithMeshClsn*)((char*)c + 0x144))->IsOnGround();
    if (r == 0) return;
    _Z14ApproachLinearRiii(*(int*)((char*)c + 0x98), 0, a1);
    int x = *(int*)((char*)c + 0x5c);
    int y = *(int*)((char*)c + 0x60);
    int z = *(int*)((char*)c + 0x64);
    Particle::RunningSlidingDustAt(x, y, z);
}
