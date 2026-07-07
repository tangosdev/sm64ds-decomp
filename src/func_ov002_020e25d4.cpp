//cpp
struct Vector3 { int x, y, z; };

extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 *v);

extern "C" void func_ov002_020e25d4(char *c)
{
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(c + 0x6d9), 0x10, (const Vector3 *)(c + 0x74));
}