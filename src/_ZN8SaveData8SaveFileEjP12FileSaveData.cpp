//cpp
typedef unsigned int u32;

struct FileSaveData { int _00; int _04; };

extern "C" int _ZN8SaveData14SaveDataToCartEPcjj(void* data, u32 size, u32 count);

struct SaveData { static int SaveFile(u32 idx, FileSaveData* data); };

int SaveData::SaveFile(u32 idx, FileSaveData* data) {
    int* ip = (int*)(((long long)(int)((char*)data + 4)) & 0xFFFFFFFFFFFFFFFFLL);
    *ip = *ip | 1;
    if (_ZN8SaveData14SaveDataToCartEPcjj(data, 0x44, idx) == 0)
        return 1;
    return 0;
}
