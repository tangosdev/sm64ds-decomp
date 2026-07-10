//cpp
extern "C" void UnloadSilverStarAndNumber(void);
extern "C" void _ZN5Actor11UntrackStarERa(void* self, signed char* star);
extern "C" void _ZN13SharedFilePtr7ReleaseEv(void* p);

extern char data_ov002_02110944;
extern char data_ov002_02110924;
extern char data_ov002_02110964;
extern char data_ov002_02110934;

extern "C" int _ZN9PowerStar16CleanupResourcesEv(char* self){
    int b = (*(unsigned short*)(self + 0xc) == 0xb2);
    if (b) {
        int v = *(int*)(self + 0x43c);
        if (v != 8) {
            if (v == 6)
                UnloadSilverStarAndNumber();
            _ZN5Actor11UntrackStarERa(self, (signed char*)(self + 0x498));
        }
    } else {
        _ZN5Actor11UntrackStarERa(self, (signed char*)(self + 0x498));
        UnloadSilverStarAndNumber();
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110944);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110924);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110964);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_02110934);
    return 1;
}
