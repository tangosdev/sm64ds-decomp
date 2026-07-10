//cpp
// _ZN10StarSwitch16CleanupResourcesEv at 0x020ba568
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void*);
void _ZN16MeshColliderBase7DisableEv(void*);
void _ZN13SharedFilePtr7ReleaseEv(void*);
void UnloadSilverStarAndNumber(void);
}

extern char data_ov002_021098e8;
extern char data_ov002_021098ec;
extern char data_ov002_0211092c;

extern "C" int _ZN10StarSwitch16CleanupResourcesEv(char* c) {
    int t;
    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
        _ZN16MeshColliderBase7DisableEv(c + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(&data_ov002_021098e8 + *(unsigned char*)(c + 0x34c) * 0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(&data_ov002_021098ec + *(unsigned char*)(c + 0x34c) * 0xc));
    t = *(unsigned short*)(c + 0xc) == 0xc;
    if (t != false) {
        UnloadSilverStarAndNumber();
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211092c);
    }
    return 1;
}
