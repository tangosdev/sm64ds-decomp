typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;

extern void CpuSet(void* src, void* dst, u32 ctrl);
extern int func_02057020(void);
extern u8 data_020a9de0[];

void func_0206a5c0(void) {
    u32 local = 0;
    CpuSet(&local, data_020a9de0, 0x5000001);
    *(u16*)(data_020a9de0 + 2) = (u16)func_02057020();
}
