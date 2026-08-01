#include "types.h"
// @symbol func_ov002_020ce5f8
/* recovered: shared common types */
#include "common.h"
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* st);
extern int func_ov002_020ceb54(char* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const struct Vector3* pos);
extern void func_ov002_020ce8bc(void* c, int arg);
extern void func_ov002_020ceb7c(void* c);
extern u32 _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void MulVec3Mat4x3(void* v, void* m, void* dst);
extern void* Vec3_LslInPlace(void* v, int sh);
extern u32 func_020229f0(int x, int y, int z);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, const struct Vector3* pos);

extern int data_0209f32c;
extern char data_ov002_0211013c;

int func_ov002_020ce5f8(char* c) {
    int y, threshold;
    struct Vector3 pos;

    threshold = data_0209f32c - 0x50000;
    y = *(int*)(c + 0x60);
    if (y >= threshold) {
        if ((*(u8*)(c + 0x6e9) & 1) == 0) {
            if (*(int*)(c + 0xa8) >= 0) {
                *(int*)(c + 0x60) = threshold;
                *(int*)(c + 0xa8) = 0;
            }
        } else {
            if (threshold < y - 0xa000) {
                *(u8*)(c + 0x706) = 0;
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
                return 1;
            }
        }
    }

    if (func_ov002_020ceb54(c) == 0) {
        if (*(u8*)(c + 0x70c) == 0) {
            *(u8*)((((long long)(int)(c + 0x70c)))) |= 1;
            _ZN5Sound9PlayBank0EjRK7Vector3(0x17, (struct Vector3*)(c + 0x74));
            *(int*)(c + 0x628) = 0;
        }
        func_ov002_020ce8bc(c, *(int*)(c + 0x640));
    } else {
        *(u8*)(c + 0x70c) = 0;
    }
    func_ov002_020ceb7c(c);

    {
        u32 idx = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
        char* m1 = *(char**)(c + idx * 4 + 0xdc);
        char* base;
        u32 idx2;
        char* m2;
        base = *(char**)(m1 + 0x14) + 0x2d0;
        idx2 = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
        m2 = *(char**)(c + idx2 * 4 + 0xdc);
        MulVec3Mat4x3(base + 0x24, m2 + 0x1c, &pos);
    }
    Vec3_LslInPlace(&pos, 3);

    if (pos.y < data_0209f32c - 0x1e000) {
        func_020229f0(pos.x, *(volatile int*)&pos.y, pos.z);
        if (*(u16*)(c + 0x6a6) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0xb, (struct Vector3*)(c + 0x74));
            *(u16*)(c + 0x6a6) = 0xa;
        }
    }
    return 0;
}
