//cpp
typedef unsigned int u32;
typedef int s32;

struct FileSaveData { char data[0x44]; };

extern "C" {
s32 _ZN8SaveData16ReadDataFromCartEPcjj(char* data, u32 size, u32 fileID);
void _ZN8SaveData16SetDefaultValuesEP12FileSaveData(struct FileSaveData* data);
}

extern int SAVE_DATA;

extern "C" int _ZN8SaveData12ReadFileDataEjP12FileSaveData(u32 fileID, struct FileSaveData* dest)
{
    char* r5 = (char*)dest;
    s32 result = _ZN8SaveData16ReadDataFromCartEPcjj(r5, 0x44, fileID);
    if (result) {
        _ZN8SaveData16SetDefaultValuesEP12FileSaveData((struct FileSaveData*)r5);
        if (result == 2)
            return 1;
        return 0;
    }
    {
        int* p = (int*)(((long long)(int)(r5 + 0xc)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p & *(int*)((char*)&SAVE_DATA + 0x48);
    }
    return 1;
}
