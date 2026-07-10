typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_02012694(int a, void* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);

int _ZN29FloatOnWaterPlatformWdwSquare8BehaviorEv(u8* c)
{
    switch (c[0x327]) {
    case 0:
        if (c[0x326] != 0) {
            func_02012694(0x17b, c + 0x74);
            {
                u8* p = (u8*)(((int)c + 0x327) & 0xFFFFFFFFFFFFFFFFLL);
                *p = *p + 1;
            }
        }
        *(int*)(c + 0x98) = 0xa000;
        break;
    case 1:
    case 3:
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        {
            int* p = (int*)(((int)c + 0x320) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + *(int*)(c + 0x98);
        }
        if (*(int*)(c + 0x320) >= 0x177000) {
            *(int*)(c + 0x320) = 0;
            {
                u8* p = (u8*)(((int)c + 0x327) & 0xFFFFFFFFFFFFFFFFLL);
                *p = *p + 1;
            }
            {
                u8* base = (u8*)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
                *(u16*)(base + 0x24) = 0;
            }
        }
        break;
    case 2:
    case 4:
        if (*(u16*)((u8*)(((unsigned)c + 0x300)) + 0x24) >= 0x3c) {
            {
                s16* p = (s16*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFFLL);
                *p = *p + 0x8000;
            }
            if (c[0x327] == 4) {
                c[0x327] = 0;
            } else {
                {
                    u8* p = (u8*)(((int)c + 0x327) & 0xFFFFFFFFFFFFFFFFLL);
                    *p = *p + 1;
                }
                func_02012694(0x17b, c + 0x74);
            }
            {
                u8* b2 = (u8*)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
                *(u16*)(b2 + 0x24) = 0;
            }
        } else {
            u16* p = (u16*)(((int)c + 0x324) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        break;
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x100000, 0) != 0) {
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    }
    c[0x326] = 0;
    return 1;
}
