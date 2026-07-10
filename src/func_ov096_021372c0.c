//cpp
extern "C" {
extern int _ZN5Actor13DistToCPlayerEv(void *self);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const void *v, unsigned int e);
extern void func_ov096_02136e54(void *self, int n);

void func_ov096_021372c0(void *self);
}

#define M(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

void func_ov096_021372c0(void *self)
{
    char *c = (char*)self;

    if (*(unsigned short*)(c + 0x354) >= 0x384) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x340);
        *(int*)(c + 0x60) = *(int*)(c + 0x344);
        *(int*)(c + 0x64) = *(int*)(c + 0x348);
    }
    if (*(unsigned char*)(c + 0x360) == 0) {
        *(int*)(c + 0x368) = 0;
        *(int*)(c + 0x364) = *(int*)(c + 0x368);
        *(int*)(c + 0x80) = 0;
        *(int*)(c + 0x84) = 0;
        *(int*)(c + 0x88) = 0;
        *(unsigned short*)(c + 0x352) = 0;
        if (_ZN5Actor13DistToCPlayerEv(c) < 0x5dc000) {
            unsigned char *p = (unsigned char*)(int)M(c + 0x360);
            *p = *p + 1;
        }
        *(unsigned short*)(c + 0x350) = 0;
        *(int*)(int)M(c + 0xec) &= ~1;
        *(unsigned short*)(c + 0x354) = 0;
    } else {
        *(int*)(c + 0x36c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(unsigned int*)(c + 0x36c), 3, 0x85, c + 0x74, 0);
        {
            int idx = *(unsigned short*)(c + 0x350);
            func_ov096_02136e54(c, (idx << 12) / 60);
        }
        if (*(unsigned short*)(c + 0x350) >= 0x3c)
            *(int*)(c + 0x35c) = 1;
    }
}
