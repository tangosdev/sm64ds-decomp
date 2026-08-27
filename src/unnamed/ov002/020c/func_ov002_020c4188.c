#include "types.h"
extern int* data_0209f318;
extern int data_0209b454;
extern short data_ov002_020ff26c[];
extern unsigned char data_0209d660;

extern void func_0200d3f8(void* thiz, unsigned char playerID, void* ptr);
extern void _ZN6Camera9SetFlag_3Ev(void* thiz);
extern void func_0201f32c(int arg0);
extern void* _ZN8dActor_c13SpawnSoundObjEj(void* thiz, unsigned int a);
extern void func_0200d81c(void* thiz, int playerID);
extern void func_ov002_020e444c(char* c);

int func_ov002_020c4188(char* self)
{
    int* p;

    if (*(u8*)(self + 0x71e) == 0) {
        return 0;
    }

    p = data_0209f318;

    switch (*(u8*)(self + 0x71f)) {
    case 0:
        if (*(u8*)(self + 0x71e) == 6) {
            (*(u8*)((int)(self + 0x71f)))++;
            goto case1;
        }
        if (*(u8*)(self + 0x71e) < 7) {
            func_0200d3f8(p, *(u8*)(self + 0x6d8), 0);
        } else {
            _ZN6Camera9SetFlag_3Ev(p);
        }
        (*(u8*)((int)(self + 0x71f)))++;
        if (*(u8*)(self + 0x71e) >= 7) {
            break;
        }
        return 0;
    case 1:
    case1:
        *(int*)((int)(self + 0xb0)) |= 0x800000;
        data_0209b454 |= 0x800000;
        *(u8*)(self + 0x720) = 0xa;
        (*(u8*)((int)(self + 0x71f)))++;
        break;
    case 2:
        if (*(u8*)(self + 0x720) != 0) {
            (*(u8*)((int)(self + 0x720)))--;
            break;
        }
        func_0201f32c(data_ov002_020ff26c[*(u8*)(self + 0x71e) - 1]);
        if (*(u8*)(self + 0x71e) == 6) {
            *(void**)(self + 0x364) = _ZN8dActor_c13SpawnSoundObjEj(self, 5);
        }
        (*(u8*)((int)(self + 0x71f)))++;
        break;
    case 3:
        if (data_0209d660 != 0) {
            break;
        }
        (*(u8*)((int)(self + 0x71f)))++;
        *(int*)((int)(self + 0xb0)) &= ~0x800000;
        data_0209b454 &= ~0x800000;
        if (*(u8*)(self + 0x71e) >= 7) {
            *(int*)(((int)p + 0x154)) &= ~8;
        }
        func_0200d81c(p, *(u8*)(self + 0x6d8));
        if (*(u8*)(self + 0x71e) == 6) {
            int* obj = *(int**)(self + 0x364);
            if (obj[0x35] == 0x2a) {
                *(short*)((char*)obj + 0xde) = 0;
            }
        }
        break;
    case 4:
        if (p[0x55] & 0x8000) {
            break;
        }
        *(u8*)(self + 0x71e) = 0;
        *(u8*)(self + 0x71f) = 0;
        *(int*)((int)(self + 0xb0)) &= ~0x800000;
        data_0209b454 &= ~0x800000;
        break;
    }

    func_ov002_020e444c(self);
    return 1;
}
