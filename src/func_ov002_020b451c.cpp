//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern int _ZN5Actor9TrackStarEjj(char* c, unsigned a, unsigned b);
extern void LoadSilverStarAndNumber(void);
extern int ONE_UP_MUSHROOM_MODEL_PTR;
extern int SUPER_MUSHROOM_MODEL_PTR;
extern int SHELL_MODEL_PTRS;
int func_ov002_020b451c(char* c) {
    *(signed char*)(c+0xd4) = *(int*)(c+8) & 0x7f;
    if (*(signed char*)(c+0xd4) == 0x7f) *(signed char*)(c+0xd4) = 0;
    switch (*(unsigned short*)(c+0xc)) {
    case 0x141:
        *(unsigned char*)(c+0xd7) = 0;
        _ZN5Model8LoadFileER13SharedFilePtr(&ONE_UP_MUSHROOM_MODEL_PTR);
        break;
    case 0x142:
        *(unsigned char*)(c+0xd7) = 1;
        _ZN5Model8LoadFileER13SharedFilePtr(&SUPER_MUSHROOM_MODEL_PTR);
        break;
    case 0x143:
        *(unsigned char*)(c+0xd7) = 2;
        _ZN5Model8LoadFileER13SharedFilePtr(&SHELL_MODEL_PTRS);
        break;
    case 0x144:
        *(unsigned char*)(c+0xd7) = 3;
        *(unsigned char*)(c+0xd5) = _ZN5Actor9TrackStarEjj(c, *(unsigned char*)(c+0xd4), 1);
        LoadSilverStarAndNumber();
        break;
    }
    return 1;
}
}
