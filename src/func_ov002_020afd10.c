typedef unsigned int u32;
typedef int Fix12i;

struct Vec3 { Fix12i x, y, z; };

extern void func_ov002_020aefb8(char* self);
extern int func_ov002_020af218(char* c, int n);
extern int _ZN5Actor15IsPlayerInRangeEi(void* thiz, int n);
extern void func_ov002_020afc68(char* c);
extern int func_ov002_020af248(char* c, int n);
extern void func_ov002_020af3a8(char* c);
extern void func_ov002_020aeee4(char* c);

void func_ov002_020afd10(char* c)
{
    volatile Fix12i v[3];

    if (*(int*)(c + 0x388) != 0) {
        v[0] = *(Fix12i*)(c + 0xa4);
        v[1] = *(Fix12i*)(c + 0xa8);
        v[2] = *(Fix12i*)(c + 0xac);
        func_ov002_020aefb8(c);
        *(Fix12i*)(c + 0xa4) = v[0];
        *(Fix12i*)(c + 0xac) = v[2];
    }

    switch (*(int*)(c + 0x388)) {
    case 0:
        func_ov002_020af218(c, 0xbb8);
        if (_ZN5Actor15IsPlayerInRangeEi(c, 0x3e8)) {
            *(Fix12i*)(c + 0x9c) = -0x4000;
            *(int*)(c + 0x388) = 1;
        }
        break;
    case 1:
        func_ov002_020afc68(c);
        break;
    case 2:
        func_ov002_020af248(c, 0x1e);
        break;
    }

    func_ov002_020af3a8(c);
    func_ov002_020aeee4(c);
}
