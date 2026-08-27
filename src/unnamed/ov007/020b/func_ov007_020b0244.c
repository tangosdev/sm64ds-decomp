extern void func_ov007_020c0638(int a, int b, int c, int d);
extern char* data_ov007_0210342c;

#define G ((char*)data_ov007_0210342c)

void func_ov007_020b0244(void)
{
    *(short*)((char*)*(void**)((char*)*(void**)*(void**)(G + 0x14c) + 4) + 2) = 5;
    *(short*)((char*)*(void**)((char*)*(void**)*(void**)(G + 0x150) + 4) + 2) = 5;
    *(short*)((char*)*(void**)((char*)*(void**)*(void**)(G + 0x154) + 4) + 2) = 5;
    *(int*)(G + 0x5c) = -1;
    *(int*)(G + 0x58) = *(int*)(G + 0x5c);
    *(int*)(G + 0x60) = 0;
    *(int*)(G + 0x64) = 0x20000;
    *(int*)(G + 0x68) = 0x30000;
    **(int**)(G + 0x6c) = 0;
    **(int**)(G + 0x70) = 0;
    func_ov007_020c0638(*(int*)((char*)*(void**)(G + 0xe0) + 4), 0, 0, 0);
    func_ov007_020c0638(*(int*)((char*)*(void**)(G + 0xe4) + 4), 0, 0, 0);
}
