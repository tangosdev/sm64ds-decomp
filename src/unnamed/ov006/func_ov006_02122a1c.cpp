//cpp
struct SharedFilePtr;
struct BMD_File;
struct Model { };
struct ModelBase { };
extern "C" SharedFilePtr data_ov006_02142f80;
extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File*, int, int);
extern "C" void func_ov006_02122a1c(void* self)
{
    BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov006_02142f80);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self, f, 1, -1);
}
