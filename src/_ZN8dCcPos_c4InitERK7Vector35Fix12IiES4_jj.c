typedef int Fix12i;
typedef unsigned int u32;

struct Vector3 { Fix12i x, y, z; };
struct CylinderClsnWithPos { void* vtable; char base[0x2c]; struct Vector3 pos; };

extern void _ZN12CylinderClsn4InitE5Fix12IiES1_jj(void* self, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);

void _ZN19CylinderClsnWithPos4InitERK7Vector35Fix12IiES4_jj(
    struct CylinderClsnWithPos* self,
    const struct Vector3* pos,
    Fix12i radius,
    Fix12i height,
    u32 flags,
    u32 vulnFlags)
{
    self->pos.x = pos->x;
    self->pos.y = pos->y;
    self->pos.z = pos->z;
    _ZN12CylinderClsn4InitE5Fix12IiES1_jj(self, radius, height, flags, vulnFlags);
}