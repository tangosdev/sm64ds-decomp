//cpp
typedef int Fix12;
struct C;
typedef void (C::*PMF)();
struct Vector3_16f;
extern "C" {
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned int a, unsigned int b, Fix12 c, Fix12 d, Fix12 e, const Vector3_16f* f);
void* _ZN8Particle6System12FromUniqueIDEj(unsigned int id);
void func_ov006_020eef90(void);
int func_ov006_020ef3e0(char *c);
}
int func_ov006_020ef3e0(char *c)
{
    *(unsigned int*)(c + 0x5a6c) =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned int*)(c + 0x5a6c), 0xf0, 0x400000, 0x800000, -0x480000, 0);
    void *o = _ZN8Particle6System12FromUniqueIDEj(*(unsigned int*)(c + 0x5a6c));
    if (o != 0) {
        *(int*)((char*)o + 0x50) = 0x4000;
        *(unsigned char*)((char*)o + 0x58) = 0x2c;
    }
    func_ov006_020eef90();
    {
        C *cc = (C*)c;
        PMF *pp = (PMF*)(c + 0x5004);
        (cc->**pp)();
    }
    return 1;
}
