//cpp
struct Vector3 { int x, y, z; };

extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 *v);

extern "C" void _ZN6Player17PlayMammaMiaSoundEv(char *thiz)
{
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(thiz + 0x6d9), 0x27, (const Vector3 *)(thiz + 0x74));
}