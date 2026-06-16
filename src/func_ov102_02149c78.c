typedef int s32;
typedef unsigned short u16;
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *, void *, s32);
void func_ov102_02149c78(void *c)
{
    s32 vec[3];
    void *r4 = c;
    vec[0] = *(s32*)((char*)r4 + 0x5c);
    vec[1] = *(s32*)((char*)r4 + 0x60);
    vec[2] = *(s32*)((char*)r4 + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(r4, vec, 0x5dc000);
    *(u16*)((char*)r4 + 0x3ec) = 0x4000;
    *(u16*)((char*)r4 + 0x3ee) = 7;
}
