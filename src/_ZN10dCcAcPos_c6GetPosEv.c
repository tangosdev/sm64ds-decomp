/* dCcAcPos_c::GetPos() at 0x02014a2c
 * vtable slot 2. Returns a reference to this->pos (Vector3 at 0x34).
 */

typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };
struct dCcAcPos_c;

struct Vector3* _ZN10dCcAcPos_c6GetPosEv(struct dCcAcPos_c* self)
{
    return (struct Vector3*)((char*)self + 0x34); // this->pos
}
