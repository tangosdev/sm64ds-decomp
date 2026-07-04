typedef int Fix12i;
extern int _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN9ActorBase18MarkForDestructionEv(char* c);
extern int Vec3_HorzDist(char* a, char* b);
extern void func_ov002_020e9448(char* c);
extern void func_ov002_020e81e0(char* c);
extern void func_ov002_020e7e24(char* c);
extern void func_ov002_020e7d08(char* c);
extern signed char data_0209f2f8;

void func_ov002_020ea824(char* c)
{
    char* o = (char*)_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x434));
    if (o == 0) { _ZN9ActorBase18MarkForDestructionEv(c); return; }
    if (Vec3_HorzDist(c + 0x5c, o + 0x5c) < *(int*)(c + 0x98)) {
        int* src = (int*)(((long long)(int)(o + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        int* yp = (int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int*)(c + 0x5c) = src[0];
        *(int*)(c + 0x60) = src[1];
        *(int*)(c + 0x64) = src[2];
        *yp += 0xc8000;
        *(int*)(c + 0x98) = 0;
        *(int*)(c + 0xa8) = 0x18000;
        if (data_0209f2f8 == 0xb) {
            if (*(unsigned char*)(c + 0x49d) == 3) {
                *(int*)(c + 0xa8) = 0x10000;
                goto skip;
            }
        }
        *(int*)(c + 0xa8) = 0x18000;
    skip:
        func_ov002_020e9448(c);
        *(int*)(c + 0x440) = 3;
    }
    func_ov002_020e81e0(c);
    func_ov002_020e7e24(c);
    func_ov002_020e7d08(c);
}
