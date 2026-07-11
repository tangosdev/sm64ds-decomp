//cpp
struct C { char pad[4]; };
typedef void (C::*PMF)();
extern "C" {
extern int _ZN9Animation7AdvanceEv(void*);
extern void func_ov006_020e7818(void*);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, int, int, int, const void*, void*);
extern int data_ov006_0213c704[2];

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov006_020e7be8(char* c)
{
    int* p = (int*)AT(c, 0x210);
    int* d = data_ov006_0213c704;
    if (p[0] == d[0]) {
        if (p[1] == d[1]) return;
        if (*(int*)(c + 0x210) == 0) return;
    }

    (((C*)c)->**(PMF*)(c + 0x210))();

    _ZN9Animation7AdvanceEv(c + 0x5c);
    func_ov006_020e7818(c + 0x84);

    if (*(int*)(c + 0x208) == 0) return;

    *(void**)(c + 0x204) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x204), 0xec, 0x48c000, 0x140000, 0x200000, 0, 0);
}
}
