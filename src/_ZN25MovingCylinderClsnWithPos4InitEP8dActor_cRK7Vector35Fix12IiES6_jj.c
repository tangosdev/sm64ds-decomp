typedef int Fix12i;
typedef unsigned int u32;

struct Vector3 { Fix12i x, y, z; };
struct Actor { void* vtable; };

extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, struct Actor* actor, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, const struct Vector3* pos);

void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void* self,
    struct Actor* actor,
    const struct Vector3* offset,
    Fix12i radius,
    Fix12i height,
    u32 flags,
    u32 vulnFlags)
{
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self, actor, radius, height, flags, vulnFlags);
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self, offset);
}