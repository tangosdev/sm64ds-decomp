extern int _ZN5Event6GetBitEj(unsigned int);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);

#define LD(p) ((int)(p))

int func_ov002_020b4bfc(char* c)
{
    int i;
    char* m;
    unsigned char* p;
    switch (*(unsigned char*)(c + 0xdca)) {
    case 0:
        if (_ZN5Event6GetBitEj(*(unsigned char*)(c + 0xdcb)))
            *(unsigned char*)(c + 0xdca) = 1;
        break;
    case 1:
        if (DecIfAbove0_Byte((unsigned char*)(c + 0xdc9)) == 0) {
            p = (unsigned char*)LD(c + 0xdc8);
            *p = *p - 1;
            *(unsigned char*)(c + 0xdc9) = 2;
        }
        if (*(unsigned char*)(c + 0xdc8) == 0) {
            *(unsigned char*)(c + 0xdc8) = 0;
            *(unsigned char*)(c + 0xdca) = 2;
        }
        break;
    case 2:
        if (_ZN5Event6GetBitEj(*(unsigned char*)(c + 0xdcb)) == 0)
            *(unsigned char*)(c + 0xdca) = 3;
        break;
    case 3:
        if (DecIfAbove0_Byte((unsigned char*)(c + 0xdc9)) == 0) {
            p = (unsigned char*)LD(c + 0xdc8);
            *p = *p + 1;
            *(unsigned char*)(c + 0xdc9) = 2;
        }
        if (*(unsigned char*)(c + 0xdc8) >= 4) {
            *(unsigned char*)(c + 0xdc8) = 4;
            *(unsigned char*)(c + 0xdca) = 0;
        }
        break;
    }
    m = c + 0x4b0;
    for (i = 0; i < 5; i++) {
        if (i == *(unsigned char*)(c + 0xdc8)) {
            if (!_ZN16MeshColliderBase9IsEnabledEv(m))
                _ZN16MeshColliderBase6EnableEP5Actor(m, c);
        } else {
            if (_ZN16MeshColliderBase9IsEnabledEv(m))
                _ZN16MeshColliderBase7DisableEv(m);
        }
        m += 0x1c8;
    }
    return 1;
}
