typedef int Fix12i;
struct Vector3;
struct Actor;
extern struct Actor* _ZN5Actor22ClosestNonVanishPlayerEv(void* self);
extern Fix12i Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern short Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern short AngleDiff(short a, short b);
extern void Scuttlebug_SetState(char* c, int mode);
void func_ov071_0211f0b4(char* c) {
    struct Actor* p;
    Fix12i d;
    short ang;
    if (*(unsigned short*)(c+0x3a8) != 0) return;
    p = _ZN5Actor22ClosestNonVanishPlayerEv(c);
    if (p == 0) return;
    d = Vec3_Dist((const struct Vector3*)(c+0x5c), (const struct Vector3*)((char*)p+0x5c));
    if (d > 0x5dc000) return;
    ang = Vec3_HorzAngle((const struct Vector3*)(c+0x5c), (const struct Vector3*)((char*)p+0x5c));
    if (AngleDiff(ang, *(short*)(c+0x8e)) > 0x12c) return;
    *(Fix12i*)(c+0x3a0) = d;
    *(short*)(c+0x3a4) = ang;
    Scuttlebug_SetState(c, 3);
}
