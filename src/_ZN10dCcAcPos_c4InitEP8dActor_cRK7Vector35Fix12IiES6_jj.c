typedef int Fix12i;
typedef unsigned int u32;

struct Vector3 { Fix12i x, y, z; };
struct dActor_c { void* vtable; };

extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, struct dActor_c* actor, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* self, const struct Vector3* pos);

void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void* self,
    struct dActor_c* actor,
    const struct Vector3* offset,
    Fix12i radius,
    Fix12i height,
    u32 flags,
    u32 vulnFlags)
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(self, actor, radius, height, flags, vulnFlags);
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(self, offset);
}