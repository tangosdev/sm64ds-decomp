extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern char data_ov002_0211092c;
extern char data_ov002_0210d9a8;
extern char data_ov002_0211093c;

int _ZN10StarMarker16CleanupResourcesEv(char* c) {
    if (*(unsigned char*)(c + 0x1d8) != 0) {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211092c);
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a8);
    } else {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211093c);
    }
    return 1;
}
