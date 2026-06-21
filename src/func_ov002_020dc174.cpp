//cpp
struct Actor;
struct Vector3;
struct MovingCylinderClsnWithPos;

extern "C" {

void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    MovingCylinderClsnWithPos* self, Actor* actor, const Vector3& v,
    int fix, int d, unsigned int e, unsigned int f);

void func_ov002_020dc174(char* self, const Vector3& v, int fix, int d,
                         unsigned int stackA, unsigned int stackB)
{
    unsigned int flags = *(unsigned char*)(self + 0x703) ? 0x10 : 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        (MovingCylinderClsnWithPos*)(self + 0x314), (Actor*)self, v, fix, d,
        stackA | 2 | flags, stackB);
}

}
