//cpp
typedef int Fix12i;
struct Vector3_16f;
struct CylinderClsn;

struct C;
typedef void (C::*PMF)();
struct Holder { int pad[2]; PMF pmf; };  // pmf at offset 8

extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, CylinderClsn* c);
extern "C" void _ZN12CylinderClsn5ClearEv(void* c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* c);
extern "C" unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, Fix12i c, Fix12i d, Fix12i e, const Vector3_16f* f);

extern "C" int func_ov022_02112800(char* thiz)
{
    char* c = thiz;
    C* o = (C*)c;
    DecIfAbove0_Short((unsigned short*)(c + 0x110));
    {
        Holder* h = *(Holder**)(c + 0x108);
        if (*(int*)((char*)h + 8) != 0) {
            (o->*(h->pmf))();
        }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (CylinderClsn*)(c + 0xd4));
    _ZN12CylinderClsn5ClearEv(c + 0xd4);
    _ZN12CylinderClsn6UpdateEv(c + 0xd4);
    {
        int b = (int)((*(int*)(c + 0xb0) & 8) != 0);
        if (b == 0) {
            *(int*)(c + 0x114) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned int*)(c + 0x114), 0x129, *(int*)(c + 0x5c),
                *(int*)(c + 0x60), *(int*)(c + 0x64), 0);
        }
    }
    return 1;
}
