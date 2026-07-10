//cpp
typedef int Fix12;

struct SharedFilePtr { void* p; void* file; };
struct BMD_File;
struct BTP_File;

extern "C" {
    void _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
    void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
    int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
    void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp);
    void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, Fix12 b, unsigned int c);
}

extern SharedFilePtr data_ov002_0210da08;
extern SharedFilePtr data_ov002_0210da28;
extern SharedFilePtr data_ov002_0210d9e8;
extern SharedFilePtr data_ov002_0210d9a8;

extern "C" int _ZN15InvisibleSecret13InitResourcesEv(char* self)
{
    if (*(int*)(self + 8) & 0x10) {
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210da08);
        void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da28);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, m, 1, 1) == 0)
            return 0;
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
            data_ov002_0210da28.file, data_ov002_0210da08.file);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            self + 0x124, data_ov002_0210da08.file, 0x40000000, 0, 0);
        *(int*)(self + 0x12c) = (int)((((unsigned int)(*(int*)(self + 8) & 0xf) % 10) << 16) >> 4);
    } else {
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210d9e8);
        void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a8);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, m, 1, 1) == 0)
            return 0;
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
            data_ov002_0210d9a8.file, data_ov002_0210d9e8.file);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            self + 0x124, data_ov002_0210d9e8.file, 0x40000000, 0, 0);
        *(int*)(self + 0x12c) = (int)((((unsigned int)(*(int*)(self + 8) & 0xf) % 10) << 16) >> 4);
    }

    *(unsigned char*)(self + 0x14e) = 0;
    *(int*)(self + 0x13c) = *(int*)(self + 0x5c);
    *(int*)(self + 0x140) = *(int*)(self + 0x60);
    *(int*)(self + 0x144) = *(int*)(self + 0x64);
    *(int*)(self + 0xa8) = 0x14000;
    *(int*)(self + 0x9c) = -0x2000;
    *(int*)(self + 0xa0) = -0x32000;
    *(short*)(self + 0x100 + 0x4c) = 0;
    *(int*)(self + 0x138) = 0;
    *(int*)(self + 0x148) = 0;
    return 1;
}
