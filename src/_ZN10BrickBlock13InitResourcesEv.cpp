//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern int _ZN5Actor9TrackStarEjj(char* c, unsigned a, unsigned b);
extern void LoadSilverStarAndNumber(void);
extern int data_ov002_0210d9d8;
extern int data_ov002_0210da30;
extern int data_ov002_0210da18;
int _ZN10BrickBlock13InitResourcesEv(char* c) {
    *(signed char*)(c+0xd4) = *(int*)(c+8) & 0x7f;
    if (*(signed char*)(c+0xd4) == 0x7f) *(signed char*)(c+0xd4) = 0;
    switch (*(unsigned short*)(c+0xc)) {
    case 0x141:
        *(unsigned char*)(c+0xd7) = 0;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
        break;
    case 0x142:
        *(unsigned char*)(c+0xd7) = 1;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
        break;
    case 0x143:
        *(unsigned char*)(c+0xd7) = 2;
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da18);
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
