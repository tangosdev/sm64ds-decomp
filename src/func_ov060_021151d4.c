extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int _ZN5Actor13DistToCPlayerEv(void *self);

void func_ov060_021151d4(char *a)
{
    unsigned int m;

    if (*(unsigned char *)(a + 0x415) != 0)
        goto cold;

    if ((*(int *)(a + 0x418) & 2) == 0)
        goto setE;

    if (_ZN5Actor13DistToCPlayerEv(a) < 0x514000) {
        m = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10;
        if (m < *(unsigned char *)(a + 0x42a)) {
            *(int *)(a + 0x40c) = 0x10;
            if (*(unsigned char *)(a + 0x42a) > 3)
                *(unsigned char *)(a + 0x42a) = 3;
            else
                *(unsigned char *)(a + 0x42a) = 1;
        } else {
            *(int *)(a + 0x40c) = 9;
            *(unsigned char *)(a + 0x42a) = 5;
        }
        goto tail;
    }

    *(int *)(a + 0x40c) = 7;
    if (*(int *)(a + 0x3f4) > 0x1f4000 && *(int *)(a + 0x3f4) < 0x5dc000) {
        m = ((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10;
        if (m < 5)
            *(int *)(a + 0x40c) = 0xd;
    }
    goto tail;

setE:
    *(int *)(a + 0x40c) = 0xe;
tail:
    {
        unsigned char *p = (unsigned char *)(((long long)(int)(a + 0x415)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p + 1;
    }
    return;

cold:
    *(unsigned char *)(a + 0x415) = 0;
    *(int *)(a + 0x40c) = 0xe;
}
