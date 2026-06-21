//cpp
struct SharedFilePtr;
extern "C" {

void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr* self);

extern SharedFilePtr data_ov002_0211092c;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0211093c;

unsigned int func_ov002_020eabcc(char* self)
{
    if (*(unsigned char*)(self + 0x1d8)) {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211092c);
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9a8);
    } else {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0211093c);
    }
    return 1;
}

}
