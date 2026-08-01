#include "types.h"
extern int func_02034d34(void* thiz);
extern void _ZN3OAM5ResetEv(void);
extern s32 func_0200f0bc(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, void* attr, int x, int y, int a, int cc, int fx, int mode);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, void* attr, int x, int y, int a, int cc, int fx0, int fy0, int rot, int mode);
extern void _ZN3OAM9RenderSubEP7OamAttrii(void* attr, int a, int b);
extern void func_0203083c(void);
extern void _ZN3OAM5FlushEv(void);
extern void _ZN3OAM4LoadEv(void);

extern void* data_0208a0f8[];
extern s16 data_02082214[];
extern int data_0208ee44;

int func_02034b40(char* self)
{
    if (*(void**)(self + 4) != 0)
        func_02034d34(*(void**)(self + 4));
    _ZN3OAM5ResetEv();

    if (*(u8*)(self + 0xd) != 0) {
        int id = func_0200f0bc();
        void* attr = data_0208a0f8[id];

        if (*(u8*)(self + 0xc) == 0) {
            int sb = *(int*)(self + 8);
            int x = 0x80;
            int one = 1;
            int arr[3];
            arr[0] = 0;
            arr[1] = 0;
            arr[2] = 0;

            do {
                int y;
                if (sb >= 0 && sb < 0x10000) {
                    u16 t = (u16)sb;
                    s16 ang = (s16)t;
                    u16 t2 = (u16)ang;
                    y = data_02082214[(t2 >> 4) * 2];
                    y = (y >> 10) + 0x80;
                    arr[0] = one;
                } else {
                    y = x;
                }

                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(arr[1], attr, x, y, -1, -1, 0x1000, arr[1]);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(one, attr, x, y, -1, -1, 0x1000, arr[2]);

                if (*(u16*)((char*)attr + 6) == 0xffff)
                    break;
                attr = (char*)attr + 8;
                sb -= 0x2000;
            } while (1);

            if (*(u8*)(self + 0xe) != 0) {
                *(int*)(((long long)(int)(self + 8))) += (data_0208ee44 << 10);
                if (arr[0] == 0)
                    *(u8*)(self + 0xc) = 0x78;
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, attr, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            _ZN3OAM9RenderSubEP7OamAttrii(attr, 0x80, 0x80);
            *(unsigned char*)(((long long)(int)(self + 0xc))) -= data_0208ee44;
            *(int*)(self + 8) = 0;
        }
    }

    func_0203083c();
    _ZN3OAM5FlushEv();
    _ZN3OAM4LoadEv();
    return 1;
}
