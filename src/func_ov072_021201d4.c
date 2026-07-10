typedef int Fix12i;
struct Vector3 { int x, y, z; };

extern int _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, const struct Vector3 *v, Fix12i mag);
extern int func_0201267c(int a, void *b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12i dist, int loop);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_ov072_021205d4(void *self, int a);

int func_ov072_021201d4(char *self)
{
    struct Vector3 v;
    unsigned char *st;
    switch (*(unsigned char *)(self + 0x334))
    {
    case 0:
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, self + 0x138);
        if (*(int *)(self + 0xa8) < 0 && *(int *)(self + 0x60) < (int)0xffc427c0)
        {
            v.x = *(int *)(self + 0x5c);
            v.y = *(int *)(self + 0x60);
            v.z = *(int *)(self + 0x64);
            _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(self, &v, 0x5dc000);
            func_0201267c(0x115, self + 0x74);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10f, *(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x110, *(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
            st = (unsigned char *)(((int)self + 0x334) & 0xFFFFFFFFFFFFFFFFLL);
            *st = *st + 1;
        }
        break;
    case 1:
        if (DecIfAbove0_Byte((unsigned char *)(self + 0x335)) == 0)
        {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x15666, 0);
            func_ov072_021205d4(self, 3);
        }
        break;
    }
    return 1;
}
