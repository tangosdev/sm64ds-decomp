#include "types.h"
extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, u32 a, u32 b);
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern void func_02012694(u32 a, void* v);
extern int _ZN6Player12FinishedAnimEv(void* thiz);
extern void func_ov002_020ca108(char* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, u32 a, int b, Fix12i c, u32 d);

int func_ov002_020c897c(char* c)
{
    int id;

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
    if (_ZNK9Animation12WillHitFrameEi(*(void**)(c + id * 4 + 0xdc) + 0x50, 0x5f)) {
        func_02012694(0x3b, c + 0x74);
    } else {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi(*(void**)(c + id * 4 + 0xdc) + 0x50, 0x3c)) {
            func_02012694(0x42, c + 0x74);
        } else if (_ZN6Player12FinishedAnimEv(c)) {
            func_ov002_020ca108(c);
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
            *(u8*)(c + 0x743) = 0;
        }
    }
    return 0;
}
