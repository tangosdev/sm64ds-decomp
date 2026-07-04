//cpp
typedef unsigned short u16;
struct Vector3 { int x, y, z; };
struct State;
extern "C" void func_02035860(void *a, struct Vector3 *v);
extern struct State data_ov002_021101cc;
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *self, struct State &s);

extern "C" void func_ov002_020db54c(unsigned char *self, int a, int b, short c)
{
    void *node;
    *(short *)(self + 0x94) = c;
    *(int *)(self + 0x98) = a;
    *(int *)(self + 0xa8) = b;
    node = *(void **)(self + 0x35c);
    if (node != 0) {
        int bb = (int)(*(u16 *)((char *)node + 0xc) == 0xbf);
        if (bb != 0) {
            struct Vector3 *src = (struct Vector3 *)(((int)node + 0x5c) & 0xFFFFFFFFFFFFFFFF);
            struct Vector3 v;
            int ty;
            v.x = src->x;
            ty = src->y;
            v.y = ty;
            v.z = src->z;
            v.y = ty + 0x64000;
            func_02035860(self + 0x380, &v);
            *(int *)(self + 0x68) = v.x;
            *(int *)(self + 0x6c) = v.y;
            *(int *)(self + 0x70) = v.z;
        }
    }
    *(int *)(self + 0x35c) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_021101cc);
}
