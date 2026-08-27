//cpp
struct BMD_File;

extern "C" unsigned int func_02017060(BMD_File* file);

class SharedFilePtr {
public:
    char pad[4];
    BMD_File* file;

    unsigned int ReallocateModelFile();
};

unsigned int SharedFilePtr::ReallocateModelFile()
{
    return func_02017060(file);
}
