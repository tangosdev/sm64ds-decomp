//cpp
extern "C" {
extern void* data_ov004_020beb68;
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool b, void* a, int x, int y, int z, int w, void* m);

void func_ov004_020af948(void* a, int b, int c, void* m)
{
    if (data_ov004_020beb68 == 0) return;
    if (*(int*)((char*)data_ov004_020beb68 + 0x4628) == 0) {
        int (**vt)(void*) = *(int(***)(void*))data_ov004_020beb68;
        if (vt[0x68/4]((void*)data_ov004_020beb68) == 2) {
            if (*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) != 0) return;
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(false, a, b, c, -1, -1, m);
            return;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(true, a, b, c, -1, -1, m);
}
}
