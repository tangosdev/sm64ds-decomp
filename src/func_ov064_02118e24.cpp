//cpp
struct Vector3 { int x, y, z; };

extern "C" void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int n, const struct Vector3 *v);

extern "C" void func_ov064_02118e24(void *thiz, int a1, int a2, int a3)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned int st = *(unsigned short *)((c + 0x300) + 0x34);

    if (st < 0x14) {
        if (st & 1) {
            *(int *)(c + 0x330) = -0x6000;
        } else {
            *(int *)(c + 0x330) = 0;
        }
        return;
    }

    if (st == 0x14) {
        *(int *)(c + 0x330) = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0xe7, (const struct Vector3 *)(c + 0x74));
    }

    if ((int)*(unsigned short *)((c + 0x300) + 0x34) >= a3 + 0x14) {
        *(unsigned char *)(c + 0x336) = 1;
        *(unsigned char *)(c + 0x339) = 0;
        return;
    }

    {
        int *px = (int *)(((int)c + 0x5c) & 0xFFFFFFFFFFFFFFFF);
        int *pz = (int *)(((int)c + 0x64) & 0xFFFFFFFFFFFFFFFF);
        *px += a1;
        *pz += a2;
    }
}
