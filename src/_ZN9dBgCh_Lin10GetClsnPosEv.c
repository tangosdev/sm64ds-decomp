/* RaycastLine::GetClsnPos() at 0x020375d0
 * Returns this->clsnPos by value (Vector3 at 0x54). `res` is the hidden
 * return-value pointer the ABI passes as the first argument.
 */

typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };
struct RaycastLine { char pad[0x54]; struct Vector3 clsnPos; };

void _ZN11RaycastLine10GetClsnPosEv(struct Vector3* res, struct RaycastLine* self)
{
    res->x = self->clsnPos.x;
    res->y = self->clsnPos.y;
    res->z = self->clsnPos.z;
}
