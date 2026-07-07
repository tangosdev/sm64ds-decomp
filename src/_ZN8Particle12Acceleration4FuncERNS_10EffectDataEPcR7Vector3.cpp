//cpp
typedef short s16;
struct EffectData {
    s16 x;
    s16 y;
    s16 z;
};
struct Vector3 {
    int x;
    int y;
    int z;
};

extern "C" void _ZN8Particle12Acceleration4FuncERNS_10EffectDataEPcR7Vector3(EffectData &e, char *p, Vector3 &v)
{
    v.x += e.x;
    v.y += e.y;
    v.z += e.z;
}