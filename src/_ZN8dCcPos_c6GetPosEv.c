/* CylinderClsnWithPos::GetPos() at 0x02014820
 * vtable slot 2. Returns a reference to this->pos (Vector3 at 0x30).
 */

typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };
struct CylinderClsnWithPos;

struct Vector3* _ZN19CylinderClsnWithPos6GetPosEv(struct CylinderClsnWithPos* self)
{
    return (struct Vector3*)((char*)self + 0x30); // this->pos
}
