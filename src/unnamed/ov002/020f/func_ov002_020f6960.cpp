//cpp
struct SharedFilePtr;
struct BMD_File;
extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File*, int, int);
extern "C" int func_ov002_020f6960(char* self, SharedFilePtr* fp, int n)
{
    *(SharedFilePtr**)(self + 0x5c) = fp;
    BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(*(*(SharedFilePtr**)(self + 0x5c)));
    return _ZN9ModelBase7SetFileEP8BMD_Fileii(self, f, 1, n) != 0 ? 1 : 0;
}
