//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void *p);
extern void _ZN16MeshColliderBase7DisableEv(void *p);
extern void _ZN13SharedFilePtr7ReleaseEv(void *p);
extern void _ZN5Actor11UntrackStarERa(void *self, void *p);
}
extern char data_ov002_0210da58[];
extern char data_ov002_0210da18[];
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210d9b0[];
extern char data_ov002_0210d9d0[];
extern char data_ov002_0210d9e0[];
extern char data_ov002_0210da40[];
extern char data_ov002_0210d9a0[];
extern char data_ov002_0210d9c0[];
extern char data_ov102_0214e7e8[];
extern char data_ov102_0214e808[];
extern char data_ov102_0214e7f8[];
extern char data_ov102_0214e800[];
extern char data_ov102_0214e7f0[];
extern char data_ov102_0214e7d8[];
extern char data_ov102_0214e7e0[];
extern char data_ov102_0214e7d0[];

extern "C" int func_ov102_0214a08c(char *c)
{
    int b, b2, b3;
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124))
        _ZN16MeshColliderBase7DisableEv(c + 0x124);

    b = (int)(*(unsigned short *)(c + 0xc) == 0x16);
    if (b)
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da58);

    b2 = (int)(*(unsigned short *)(c + 0xc) == 0x14);
    if (b2)
        goto dosw;
    b3 = (int)(*(unsigned short *)(c + 0xc) == 0x15);
    if (b3) {
    dosw:
        switch (*(unsigned char *)(c + 0x3f3)) {
        case 1:
            _ZN5Actor11UntrackStarERa(c, c + 0x3f0);
            break;
        case 3:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da18);
            break;
        case 2:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8);
            break;
        case 4:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da30);
            break;
        case 7:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
            break;
        case 5:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da58);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
            break;
        case 6:
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
            break;
        }
    }

    switch (*(unsigned short *)(c + 0xc) - 0x14) {
    case 0:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7e8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e808);
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7f8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
        break;
    case 1:
    case 2:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e800);
        break;
    case 3:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7f0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da40);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
        break;
    case 5:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7d8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
        break;
    case 4:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7e0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9c0);
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e0);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7d0);
    return 1;
}
