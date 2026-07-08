//cpp
extern "C" {
extern int _ZN16MeshColliderBase9IsEnabledEv(void *p);
extern void _ZN16MeshColliderBase7DisableEv(void *p);
extern void _ZN13SharedFilePtr7ReleaseEv(void *p);
extern void _ZN5Actor11UntrackStarERa(void *self, void *p);
}
extern char FEATHER_MODEL_PTR[];
extern char SHELL_MODEL_PTRS[];
extern char ONE_UP_MUSHROOM_MODEL_PTR[];
extern char SUPER_MUSHROOM_MODEL_PTR[];
extern char POWER_FLOWER_OPEN_MODEL_PTR[];
extern char POWER_FLOWER_CLOSED_MODEL_PTR[];
extern char BOB_OMB_MODEL_PTR[];
extern char MARIO_CAP_MODEL_PTR[];
extern char LUIGI_CAP_MODEL_PTR[];
extern char WARIO_CAP_MODEL_PTR[];
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
        _ZN13SharedFilePtr7ReleaseEv(FEATHER_MODEL_PTR);

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
            _ZN13SharedFilePtr7ReleaseEv(SHELL_MODEL_PTRS);
            break;
        case 2:
            _ZN13SharedFilePtr7ReleaseEv(ONE_UP_MUSHROOM_MODEL_PTR);
            break;
        case 4:
            _ZN13SharedFilePtr7ReleaseEv(SUPER_MUSHROOM_MODEL_PTR);
            break;
        case 7:
            _ZN13SharedFilePtr7ReleaseEv(POWER_FLOWER_OPEN_MODEL_PTR);
            _ZN13SharedFilePtr7ReleaseEv(POWER_FLOWER_CLOSED_MODEL_PTR);
            _ZN13SharedFilePtr7ReleaseEv(BOB_OMB_MODEL_PTR);
            break;
        case 5:
            _ZN13SharedFilePtr7ReleaseEv(FEATHER_MODEL_PTR);
            _ZN13SharedFilePtr7ReleaseEv(POWER_FLOWER_OPEN_MODEL_PTR);
            _ZN13SharedFilePtr7ReleaseEv(POWER_FLOWER_CLOSED_MODEL_PTR);
            break;
        case 6:
            _ZN13SharedFilePtr7ReleaseEv(POWER_FLOWER_OPEN_MODEL_PTR);
            _ZN13SharedFilePtr7ReleaseEv(POWER_FLOWER_CLOSED_MODEL_PTR);
            break;
        }
    }

    switch (*(unsigned short *)(c + 0xc) - 0x14) {
    case 0:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7e8);
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e808);
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7f8);
        _ZN13SharedFilePtr7ReleaseEv(BOB_OMB_MODEL_PTR);
        break;
    case 1:
    case 2:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e800);
        break;
    case 3:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7f0);
        _ZN13SharedFilePtr7ReleaseEv(MARIO_CAP_MODEL_PTR);
        _ZN13SharedFilePtr7ReleaseEv(BOB_OMB_MODEL_PTR);
        break;
    case 5:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7d8);
        _ZN13SharedFilePtr7ReleaseEv(LUIGI_CAP_MODEL_PTR);
        _ZN13SharedFilePtr7ReleaseEv(BOB_OMB_MODEL_PTR);
        break;
    case 4:
        _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7e0);
        _ZN13SharedFilePtr7ReleaseEv(WARIO_CAP_MODEL_PTR);
        _ZN13SharedFilePtr7ReleaseEv(BOB_OMB_MODEL_PTR);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov102_0214e7d0);
    return 1;
}
