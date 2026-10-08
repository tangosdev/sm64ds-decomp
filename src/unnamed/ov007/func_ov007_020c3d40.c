extern int func_ov007_020c3df4(int a, int b);
extern void MultiStore32Bytes(unsigned val, int* dst, int len);
extern void MultiStore_Int(int val, int* dst, int len);
extern void MultiStore16(unsigned short val, char* dst, int nbytes);
extern void func_0205a588(int a, int b, int c);

int func_ov007_020c3d40(int a, int n)
{
    int* r = (int*)func_ov007_020c3df4(a, n);
    volatile unsigned short v16;
    volatile unsigned v32;
    volatile int vint;
    switch (n - (n & ~3)) {
    case 0:
        if (n - (n & ~0x1f) != 0) {
            v32 = 0;
            MultiStore32Bytes(v32, r, n);
        } else {
            vint = 0;
            MultiStore_Int(vint, r, n);
        }
        break;
    case 2:
        v16 = 0;
        MultiStore16(v16, (char*)r, n);
        break;
    case 1:
    case 3:
        func_0205a588((int)r, 0, n);
        break;
    }
    return (int)r;
}
