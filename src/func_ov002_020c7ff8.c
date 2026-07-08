typedef unsigned char u8;
typedef unsigned int u32;

extern void *CAMERA;
extern void func_0200d4b0(void *g, u8 playerID, int x);
extern void func_0200d508(void *g, u8 playerID);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *thiz, u32 anim, int a, int fix, u32 e);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void *v);
extern void func_02012790(int x);

void func_ov002_020c7ff8(char *self)
{
    u8 s = *(u8 *)(self + 0x70a);
    void *g = CAMERA;
    if (s == 0 || s == 1 || s == 0x11) {
        func_0200d4b0(g, *(u8 *)(self + 0x6d8), -1);
    } else if (s == 3) {
        func_0200d4b0(g, *(u8 *)(self + 0x6d8), 5);
    } else {
        func_0200d508(g, *(u8 *)(self + 0x6d8));
    }
    if (*(u8 *)(self + 0x706) != 0) {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0xac, 0x40000000, 0x1000, 0);
        *(int *)(self + 0x9c) = 0;
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x19, 0x40000000, 0x1000, 0);
    }
    {
        u8 t = *(u8 *)(self + 0x70a);
        if (t == 0 || t == 3) {
            *(u8 *)(self + 0x6e3) = 1;
        } else {
            *(u8 *)(self + 0x6e3) = 2;
        }
    }
    *(int *)(self + 0xa8) = 0;
    *(int *)(self + 0x9c) = *(int *)(self + 0xa8);
    *(u8 *)(self + 0x716) = 1;
    *(u8 *)(self + 0x713) = 0;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 0x1c, self + 0x74);
    if (*(u8 *)(self + 0x6e6) != 0) return;
    func_02012790(0x2d);
}
