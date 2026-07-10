// NONMATCHING: different op / idiom (div=48). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_02012694(int a, void* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
int _ZN29FloatOnWaterPlatformWdwSquare8BehaviorEv(char* c) {
    switch (*(unsigned char*)(c+0x327)) {
    case 0:
        if (*(unsigned char*)(c+0x326) != 0) {
            func_02012694(0x17b, c+0x74);
            (*(unsigned char*)(c+0x327))++;
        }
        *(int*)(c+0x98) = 0xa000;
        break;
    case 1:
    case 3:
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        *(int*)(c+0x320) = *(int*)(c+0x320) + *(int*)(c+0x98);
        if (*(int*)(c+0x320) < 0x177000) break;
        *(int*)(c+0x320) = 0;
        (*(unsigned char*)(c+0x327))++;
        *(unsigned short*)(c+0x324) = 0;
        break;
    case 2:
    case 4:
        if (*(unsigned short*)(c+0x324) >= 0x3c) {
            *(short*)(c+0x94) = *(short*)(c+0x94) + 0x8000;
            if (*(unsigned char*)(c+0x327) == 4) {
                *(unsigned char*)(c+0x327) = 0;
            } else {
                (*(unsigned char*)(c+0x327))++;
                func_02012694(0x17b, c+0x74);
                *(unsigned short*)(c+0x324) = 0;
            }
            break;
        }
        *(unsigned short*)(c+0x324) = *(unsigned short*)(c+0x324) + 1;
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x100000, 0) != 0) {
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
            *(unsigned char*)(c+0x326) = 0;
        }
        break;
    }
    return 1;
}
