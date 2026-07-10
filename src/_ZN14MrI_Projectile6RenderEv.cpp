//cpp
extern "C" {
int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    unsigned int a, unsigned int b, int c, int d, int e, const void* f);
}

extern "C" int _ZN14MrI_Projectile6RenderEv(char* c)
{
    *(int*)(c + 0x328) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        *(int*)(c + 0x328), 0x46, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64), 0);
    *(int*)(c + 0x32c) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        *(int*)(c + 0x32c), 0x47, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64), 0);
    return 1;
}
