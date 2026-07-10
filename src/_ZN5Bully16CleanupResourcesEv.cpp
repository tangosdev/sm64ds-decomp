//cpp
extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void *);
int _ZN5Bully16CleanupResourcesEv(char *c) {
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+0));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+4));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+8));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+0x10));
    return 1;
}
}
