typedef unsigned int u32;
typedef int s32;
typedef s32 Fix12i;

extern u32 func_02022cbc(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, const void* dir);
extern char* data_0209f32c;

void func_ov002_020ce8bc(char* self, int arg1)
{
    int z = *(int*)(self + 0x64);
    volatile int x = *(int*)(self + 0x5c);
    volatile int y = (int)(data_0209f32c + 0x3000);
    volatile int zshadow = z;

    if (*(unsigned char*)(self + 0x703) != 0) {
        if (arg1 != 0) {
            *(int*)(self + 0x634) = func_02022cbc(*(volatile int*)(self + 0x634), 0x65, x, y, z, 0);
        } else {
            *(int*)(self + 0x638) = func_02022cbc(*(volatile int*)(self + 0x638), 0x66, x, y, z, 0);
        }
    } else {
        if (arg1 != 0) {
            *(int*)(self + 0x634) = func_02022cbc(*(volatile int*)(self + 0x634), 0xe8, x, y, z, 0);
        } else {
            *(int*)(self + 0x638) = func_02022cbc(*(volatile int*)(self + 0x638), 0xe9, x, y, z, 0);
        }
    }
}
