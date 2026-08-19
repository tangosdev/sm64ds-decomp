/* MovingCylinderClsnWithPos::GetPos() at 0x02014a2c
 * vtable slot 2. Returns a reference to this->pos (Vector3 at 0x34).
 */

typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };
struct MovingCylinderClsnWithPos;

struct Vector3* _ZN25MovingCylinderClsnWithPos6GetPosEv(struct MovingCylinderClsnWithPos* self)
{
    return (struct Vector3*)((char*)self + 0x34); // this->pos
}
