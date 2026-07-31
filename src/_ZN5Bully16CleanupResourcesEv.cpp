//cpp
extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void *);
}

struct Bully {
    int CleanupResources();
};

int Bully::CleanupResources()
{
    char * c = (char *)this;
_ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+0));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+4));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+8));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+0xc));
    _ZN13SharedFilePtr7ReleaseEv(*(void**)(*(char**)(c+0x330)+0x10));
    return 1;

}
