typedef int Fix12i;
typedef short s16;

extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);

struct Actor {
    char pad[0x5c];
};

s16 _ZN5Actor12ReflectAngleE5Fix12IiES1_s(struct Actor *thiz, Fix12i normalX, Fix12i normalZ, s16 angToReflect)
{
    s16 normalAngle = _ZN4cstd5atan2E5Fix12IiES1_(normalX, normalZ);
    s16 delta = (s16)((angToReflect + 0x8000) - normalAngle);
    return (s16)(normalAngle - delta);
}
