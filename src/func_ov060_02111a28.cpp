//cpp
extern "C" {
struct V3 { int x, y, z; };
struct G { int w[2]; };
extern G data_ov060_0211acb0;
extern G data_ov060_0211ac00;
extern G data_ov060_0211ace0;
extern G data_ov060_0211aca0;
extern G data_ov060_0211abf8;
extern G data_ov060_0211ac90;
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *a, void *v, int b);
extern void _ZN5Sound4PlayEjjRK7Vector3(unsigned a, unsigned b, void *v);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *a, void *v, int f);

void func_ov060_02111a28(char *c)
{
    /* (unsigned short)(u32 >> 12) forces ROM prologue load order
       (frame@0x12c into r0, then flag@0x446 into r1) + lsl#4/lsr#16 extract. */
    int n = (unsigned short)((unsigned)*(int *)(c + 0x12c) >> 12);
    int r3 = (*(unsigned char *)(c + 0x446) != 0) ? 1 : 0;
    int v = *(int *)(c + 0x134);
    int r1 = 0;
    *(unsigned char *)(c + 0x446) = 0;

    if (v == data_ov060_0211acb0.w[1]) {
        if (n >= 0x14 && n <= 0x17)
            r1 = 2;
        else if (n == 0x29 || n < 2)
            r1 = 1;
    } else if (v == data_ov060_0211ac00.w[1]) {
        if (n >= 0xf && n <= 0x12)
            r1 = 2;
    } else if (v == data_ov060_0211ace0.w[1]) {
        if (n >= 0x1c)
            r1 = 1;
    } else if (v == data_ov060_0211aca0.w[1]) {
        if (n >= 6 && n <= 9)
            r1 = 2;
        else if (n >= 0xf)
            r1 = 1;
    } else if (v == data_ov060_0211abf8.w[1]) {
        if (n >= 0x10 && n <= 0x13)
            r1 = 1;
        else if (n >= 0x1e && n <= 0x21)
            r1 = 2;
        else if (n >= 0x2d && n <= 0x30)
            r1 = 1;
    } else if (v == data_ov060_0211ac90.w[1]) {
        if (n >= 0xa && n <= 0xd)
            r1 = 1;
        else if (n >= 0x18 && n <= 0x1b)
            r1 = 2;
    }

    if (r1 == 0)
        return;
    *(unsigned char *)(c + 0x446) = 1;
    if (r3 != 0)
        return;

    if (r1 == 1) {
        V3 dust;
        dust.x = *(int *)(c + 0x3d4);
        dust.y = *(int *)(c + 0x3d8);
        dust.z = *(int *)(c + 0x3dc);
        _ZN5Actor17HugeLandingDustAtER7Vector3b(c, &dust, 0);
    } else {
        V3 dust;
        dust.x = *(int *)(c + 0x3e0);
        dust.y = *(int *)(c + 0x3e4);
        dust.z = *(int *)(c + 0x3e8);
        _ZN5Actor17HugeLandingDustAtER7Vector3b(c, &dust, 0);
    }
    _ZN5Sound4PlayEjjRK7Vector3(3, 0xb0, c + 0x74);
    {
        V3 quake;
        quake.x = *(int *)(c + 0x5c);
        quake.y = *(int *)(c + 0x60);
        quake.z = *(int *)(c + 0x64);
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &quake, 0x320000);
    }
}
}
